#pragma once

#include <type_traits>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// Bit-width tag types for DAC resolution selection
// (mirrors the ADC tag pattern)
// ---------------------------------------------------------------------------

struct dac8_t  {};  ///< Tag type selecting  8-bit DAC resolution.
struct dac10_t {};  ///< Tag type selecting 10-bit DAC resolution.
struct dac12_t {};  ///< Tag type selecting 12-bit DAC resolution.
struct dac16_t {};  ///< Tag type selecting 16-bit DAC resolution.

inline constexpr dac8_t  dac8{};
inline constexpr dac10_t dac10{};
inline constexpr dac12_t dac12{};
inline constexpr dac16_t dac16{};

namespace detail {

template<typename BitWidth>
struct dac_sample_type;

template<> struct dac_sample_type<dac8_t>  { using type = u8;  };
template<> struct dac_sample_type<dac10_t> { using type = u16; };
template<> struct dac_sample_type<dac12_t> { using type = u16; };
template<> struct dac_sample_type<dac16_t> { using type = u16; };

} // namespace detail

/**
 * @brief CRTP base for DAC channels parameterized by bit-width.
 *
 * @tparam Derived  The concrete DAC driver type.
 * @tparam BitWidth Resolution tag: one of @ref dac8_t, @ref dac10_t,
 *                  @ref dac12_t, @ref dac16_t.
 *
 * @par Usage
 * @code{.cpp}
 * class my_dac : public hal::dac<my_dac, hal::dac12_t> {
 *   friend class hal::dac<my_dac, hal::dac12_t>;
 *   void write_impl(u16 value) { ... } // set DAC output register
 * };
 * @endcode
 */
template<typename Derived, typename BitWidth>
class dac {
public:
  using sample_type = typename detail::dac_sample_type<BitWidth>::type;
  using bit_width   = BitWidth;

  static constexpr unsigned bit_depth = [] {
    if constexpr (std::is_same_v<BitWidth, dac8_t>)  return 8u;
    else if constexpr (std::is_same_v<BitWidth, dac10_t>) return 10u;
    else if constexpr (std::is_same_v<BitWidth, dac12_t>) return 12u;
    else return 16u;
  }();

  /// @brief Write a raw sample to the DAC output.
  /// @param value Raw value in the range [0, 2^bit_depth - 1].
  void write(this auto& self, sample_type value) {
    self.write_impl(value);
  }

protected:
  dac()  = default;
  ~dac() = default;
};

} // namespace hal
