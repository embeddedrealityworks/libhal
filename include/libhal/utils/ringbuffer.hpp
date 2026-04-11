#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

namespace hal {

/**
 * @brief Fixed-capacity, allocation-free ring buffer (FIFO queue).
 *
 * Stores up to @p N elements of type @p T in a statically allocated backing
 * array.  No heap allocation is ever performed.
 *
 * @par Design
 * Head and tail are free-running @c std::size_t counters that increment
 * monotonically and wrap naturally on unsigned overflow.  The live element
 * count is always @c tail_ - @c head_, which remains correct even across
 * overflow boundaries as long as @c N ≤ SIZE_MAX/2 (trivially satisfied for
 * any realistic embedded buffer).  When @p N is a power of two the index
 * calculation uses a bitmask instead of modulo, costing zero extra cycles.
 *
 * Storage is uninitialized (@c std::byte) so that elements are only
 * constructed when pushed and destroyed when popped, making the buffer
 * suitable for non-trivially-constructible types without paying a default-
 * construction penalty for the whole array.
 *
 * @par SPSC safety
 * Safe for single-producer / single-consumer use across an ISR boundary
 * without additional locking, provided the compiler sees the indices as
 * volatile or the caller inserts appropriate memory barriers.
 *
 * @tparam T  Element type.  Must be nothrow-move-constructible and
 *            nothrow-destructible.
 * @tparam N  Maximum number of elements.  Must be greater than zero.
 *
 * @par Usage
 * @code{.cpp}
 * hal::ring_buffer<std::byte, 64> rx_buf;
 *
 * // ISR producer:
 * rx_buf.push(received_byte);
 *
 * // Main-loop consumer:
 * if (auto b = rx_buf.pop()) {
 *     process(*b);
 * }
 * @endcode
 */
template <typename T, std::size_t N>
class ring_buffer {
    static_assert(N > 0, "ring_buffer capacity must be greater than zero");
    static_assert(std::is_nothrow_destructible_v<T>,
                  "ring_buffer requires T to be nothrow-destructible");

public:
    using value_type      = T;
    using size_type       = std::size_t;
    using reference       = T&;
    using const_reference = const T&;

    /// @brief Maximum number of elements the buffer can hold.
    static constexpr size_type capacity = N;

    ring_buffer() = default;

    /// @brief Destroys all remaining elements in the buffer.
    ~ring_buffer() { clear(); }

    ring_buffer(const ring_buffer&)            = delete;
    ring_buffer& operator=(const ring_buffer&) = delete;
    ring_buffer(ring_buffer&&)                 = delete;
    ring_buffer& operator=(ring_buffer&&)      = delete;

    // -----------------------------------------------------------------------
    // Capacity queries
    // -----------------------------------------------------------------------

    /// @brief Returns @c true if no elements are stored.
    [[nodiscard]] bool empty() const noexcept { return head_ == tail_; }

    /// @brief Returns @c true if the buffer holds @ref capacity elements.
    [[nodiscard]] bool full() const noexcept { return size() == N; }

    /// @brief Returns the current number of stored elements.
    [[nodiscard]] size_type size() const noexcept { return tail_ - head_; }

    // -----------------------------------------------------------------------
    // Element access (undefined behaviour if the buffer is empty)
    // -----------------------------------------------------------------------

    /**
     * @brief Access the oldest element (front of the queue) without removing it.
     * @pre @c !empty()
     */
    [[nodiscard]] reference front() noexcept {
        return *slot(index_of(head_));
    }
    /// @copydoc front()
    [[nodiscard]] const_reference front() const noexcept {
        return *slot(index_of(head_));
    }

    /**
     * @brief Access the most-recently pushed element (back of the queue)
     *        without removing it.
     * @pre @c !empty()
     */
    [[nodiscard]] reference back() noexcept {
        return *slot(index_of(tail_ - 1));
    }
    /// @copydoc back()
    [[nodiscard]] const_reference back() const noexcept {
        return *slot(index_of(tail_ - 1));
    }

    // -----------------------------------------------------------------------
    // Modifiers
    // -----------------------------------------------------------------------

    /**
     * @brief Copy-push a value onto the back of the queue.
     * @param value  Value to copy into the buffer.
     * @return @c true on success; @c false if the buffer is full.
     */
    bool push(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        if (full()) return false;
        std::construct_at(slot(index_of(tail_)), value);
        ++tail_;
        return true;
    }

    /**
     * @brief Move-push a value onto the back of the queue.
     * @param value  Value to move into the buffer.
     * @return @c true on success; @c false if the buffer is full.
     */
    bool push(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (full()) return false;
        std::construct_at(slot(index_of(tail_)), std::move(value));
        ++tail_;
        return true;
    }

    /**
     * @brief Construct an element in-place at the back of the queue.
     *
     * Avoids creating a temporary; forwards all arguments directly to @c T's
     * constructor.
     *
     * @tparam Args  Constructor argument types.
     * @param  args  Arguments forwarded to @c T's constructor.
     * @return @c true on success; @c false if the buffer is full.
     */
    template <typename... Args>
    bool emplace(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        if (full()) return false;
        std::construct_at(slot(index_of(tail_)), std::forward<Args>(args)...);
        ++tail_;
        return true;
    }

    /**
     * @brief Remove and return the front element.
     * @return The oldest element wrapped in @c std::optional, or
     *         @c std::nullopt if the buffer is empty.
     */
    [[nodiscard]] std::optional<T> pop() noexcept(
        std::is_nothrow_move_constructible_v<T>) {
        if (empty()) return std::nullopt;
        T* ptr = slot(index_of(head_));
        std::optional<T> result{std::move(*ptr)};
        std::destroy_at(ptr);
        ++head_;
        return result;
    }

    /**
     * @brief Remove and discard the front element.
     *
     * Slightly cheaper than @ref pop() when the value is not needed.
     *
     * @pre @c !empty()
     */
    void pop_discard() noexcept {
        std::destroy_at(slot(index_of(head_)));
        ++head_;
    }

    /**
     * @brief Destroy all stored elements and reset the buffer to empty.
     */
    void clear() noexcept {
        while (!empty()) pop_discard();
    }

private:
    // Map a free-running counter to a slot index in [0, N).
    // Uses bitmask for power-of-two N; modulo otherwise.
    static constexpr size_type index_of(size_type i) noexcept {
        if constexpr ((N & (N - 1u)) == 0u) {
            return i & (N - 1u);
        } else {
            return i % N;
        }
    }

    // Return a T* pointing into the backing storage at slot index i.
    T* slot(size_type i) noexcept {
        return std::launder(reinterpret_cast<T*>(storage_[i].data));
    }
    const T* slot(size_type i) const noexcept {
        return std::launder(reinterpret_cast<const T*>(storage_[i].data));
    }

    // Uninitialized storage: one aligned slot per element.
    struct alignas(T) slot_t { std::byte data[sizeof(T)]; };
    std::array<slot_t, N> storage_{};

    size_type head_{0};
    size_type tail_{0};
};

} // namespace hal
