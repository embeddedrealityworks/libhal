#pragma once

#include <cstddef>
#include <span>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// Sample format tag types
// ---------------------------------------------------------------------------

struct i2s_16bit_t {};  ///< 16-bit samples, 16-bit frame.
struct i2s_24bit_t {};  ///< 24-bit samples, 32-bit frame (zero-padded LSBs).
struct i2s_32bit_t {};  ///< 32-bit samples, 32-bit frame.

inline constexpr i2s_16bit_t i2s_16bit{};
inline constexpr i2s_24bit_t i2s_24bit{};
inline constexpr i2s_32bit_t i2s_32bit{};

// ---------------------------------------------------------------------------
// Standard tag types
// ---------------------------------------------------------------------------

struct i2s_philips_t {};  ///< Philips / I2S standard (data one clock late).
struct i2s_msb_t     {};  ///< MSB-justified (left-justified).
struct i2s_lsb_t     {};  ///< LSB-justified (right-justified).
struct i2s_pcm_t     {};  ///< PCM short or long frame sync.

inline constexpr i2s_philips_t i2s_philips{};
inline constexpr i2s_msb_t     i2s_msb{};
inline constexpr i2s_lsb_t     i2s_lsb{};
inline constexpr i2s_pcm_t     i2s_pcm{};

/**
 * @brief CRTP base for I2S audio interfaces.
 *
 * @p samples spans hold the raw interleaved L/R word stream as bytes.
 * Interleaving order is left-then-right per the I2S standard.
 *
 * @tparam Derived   Concrete I2S driver (e.g. @c hal::stm32f4::i2s<spi2>).
 * @tparam Format    Sample width: @ref i2s_16bit_t, @ref i2s_24bit_t, or
 *                   @ref i2s_32bit_t.
 * @tparam Standard  Framing standard: @ref i2s_philips_t, @ref i2s_msb_t,
 *                   @ref i2s_lsb_t, or @ref i2s_pcm_t.
 *
 * @par Usage
 * @code{.cpp}
 * class my_i2s : public hal::i2s<my_i2s, hal::i2s_16bit_t> {
 *   friend class hal::i2s<my_i2s, hal::i2s_16bit_t>;
 *   void write_impl(std::span<const std::byte> samples);
 *   void read_impl (std::span<std::byte>       samples);
 * };
 * @endcode
 */
template<typename Derived,
         typename Format   = i2s_16bit_t,
         typename Standard = i2s_philips_t>
class i2s {
public:
  using format   = Format;
  using standard = Standard;

  /// @brief Transmit interleaved L/R samples (blocking).
  /// @param samples Raw byte span of interleaved left/right channel words.
  void write(this auto& self, std::span<const std::byte> samples) {
    self.write_impl(samples);
  }

  /// @brief Capture interleaved L/R samples (blocking).
  /// @param samples Destination buffer for interleaved left/right channel words.
  void read(this auto& self, std::span<std::byte> samples) {
    self.read_impl(samples);
  }

protected:
  i2s()  = default;
  ~i2s() = default;
};

} // namespace hal
