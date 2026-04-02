#pragma once

#include <cstddef>
#include <span>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

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

/**
 * @brief CRTP base for USART / UART peripherals.
 *
 * @tparam Derived   Concrete USART driver (e.g. @c hal::stm32f4::usart<usart1>).
 * @tparam DataBits  Frame width: @ref data_bits_7_t, @ref data_bits_8_t, or
 *                   @ref data_bits_9_t.
 * @tparam StopBits  Stop-bit count: @ref stop_bits_1_t, @ref stop_bits_2_t,
 *                   etc.
 * @tparam Parity    Parity mode: @ref parity_none_t, @ref parity_even_t, or
 *                   @ref parity_odd_t.
 *
 * @par Usage
 * @code{.cpp}
 * class my_uart : public hal::usart<my_uart> {
 *   friend class hal::usart<my_uart>;
 *   void     write_impl(std::span<const std::byte> data);
 *   unsigned read_impl (std::span<std::byte> buf);
 *   unsigned available_impl();
 * };
 * @endcode
 */
template<typename Derived,
         typename DataBits = data_bits_8_t,
         typename StopBits = stop_bits_1_t,
         typename Parity   = parity_none_t>
class usart {
public:
  using data_bits = DataBits;
  using stop_bits = StopBits;
  using parity    = Parity;

  /// @brief Transmit all bytes in @p data (blocking).
  /// @param data Bytes to transmit.
  void write(this auto& self, std::span<const std::byte> data) {
    self.write_impl(data);
  }

  /// @brief Read up to @p buf.size() bytes from the receive buffer.
  /// @param buf Destination buffer.
  /// @return Number of bytes actually read.
  [[nodiscard]] unsigned read(this auto& self, std::span<std::byte> buf) {
    return self.read_impl(buf);
  }

  /// @brief Query the number of bytes waiting in the receive buffer.
  /// @return Byte count (0 if the buffer is empty).
  [[nodiscard]] unsigned available(this auto& self) {
    return self.available_impl();
  }

protected:
  usart()  = default;
  ~usart() = default;
};

} // namespace hal
