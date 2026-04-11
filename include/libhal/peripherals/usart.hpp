#pragma once

#include <concepts>
#include <cstddef>
#include <span>

namespace hal {

// ---------------------------------------------------------------------------
// Data-frame configuration tag types
// ---------------------------------------------------------------------------

struct data_bits_7_t {};  ///< Tag type selecting 7-bit data frames.
struct data_bits_8_t {};  ///< Tag type selecting 8-bit data frames.
struct data_bits_9_t {};  ///< Tag type selecting 9-bit data frames.

inline constexpr data_bits_7_t data_bits_7{};
inline constexpr data_bits_8_t data_bits_8{};
inline constexpr data_bits_9_t data_bits_9{};

// ---------------------------------------------------------------------------

struct stop_bits_1_t   {};  ///< Tag type selecting 1 stop bit.
struct stop_bits_0_5_t {};  ///< Tag type selecting 0.5 stop bits.
struct stop_bits_2_t   {};  ///< Tag type selecting 2 stop bits.
struct stop_bits_1_5_t {};  ///< Tag type selecting 1.5 stop bits.

inline constexpr stop_bits_1_t   stop_bits_1{};
inline constexpr stop_bits_0_5_t stop_bits_0_5{};
inline constexpr stop_bits_2_t   stop_bits_2{};
inline constexpr stop_bits_1_5_t stop_bits_1_5{};

// ---------------------------------------------------------------------------

struct parity_none_t {};  ///< Tag type disabling parity checking.
struct parity_even_t {};  ///< Tag type selecting even parity.
struct parity_odd_t  {};  ///< Tag type selecting odd parity.

inline constexpr parity_none_t parity_none{};
inline constexpr parity_even_t parity_even{};
inline constexpr parity_odd_t  parity_odd{};

// ---------------------------------------------------------------------------
// Concept
// ---------------------------------------------------------------------------

/**
 * @brief Concept for a USART / UART peripheral driver.
 *
 * A conforming type must expose:
 *  - @c write(std::span<const std::byte>) — transmit all bytes (blocking).
 *  - @c read(std::span<std::byte>) → @c unsigned — read into buffer, return
 *    actual byte count.
 *  - @c available() → @c unsigned — bytes waiting in the RX buffer.
 *
 * Frame format (data bits, stop bits, parity) is selected at compile time
 * via the @ref data_bits_8_t, @ref stop_bits_1_t, @ref parity_none_t tag
 * types passed to the concrete bus manager.
 *
 * @par Example
 * @code{.cpp}
 * template <hal::usart_peripheral Uart>
 * void println(Uart& uart, std::span<const std::byte> msg) {
 *     uart.write(msg);
 * }
 * @endcode
 */
template <typename T>
concept usart_peripheral = requires(T t,
                                    std::span<const std::byte> tx,
                                    std::span<std::byte>       rx) {
    t.write(tx);
    { t.read(rx) }    -> std::convertible_to<unsigned>;
    { t.available() } -> std::convertible_to<unsigned>;
};

} // namespace hal
