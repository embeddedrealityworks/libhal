#pragma once

#include <type_traits>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

struct adc16_t {};  ///< Tag type selecting 16-bit ADC resolution.
struct adc24_t {};  ///< Tag type selecting 24-bit ADC resolution.
struct adc32_t {};  ///< Tag type selecting 32-bit ADC resolution.

inline constexpr adc16_t adc16{};
inline constexpr adc24_t adc24{};
inline constexpr adc32_t adc32{};

namespace detail {
template<typename BitWidth>
struct adc_sample_type;

template<>
struct adc_sample_type<adc16_t> {
  using type = u16;
};

template<>
struct adc_sample_type<adc24_t> {
  using type = u32; // 24-bit packed into 32-bit
};

template<>
struct adc_sample_type<adc32_t> {
  using type = u32;
};
} // namespace detail

/**
 * @brief CRTP base for ADC interfaces parameterized by bit-width.
 *
 * @tparam Derived  The concrete ADC driver type.
 * @tparam BitWidth Resolution tag: one of @ref adc16_t, @ref adc24_t,
 *                  @ref adc32_t.
 *
 * @par Usage
 * @code{.cpp}
 * class my_adc : public hal::adc<my_adc, hal::adc16_t> {
 *   friend class hal::adc<my_adc, hal::adc16_t>;
 *   u16 read_impl() { ... }
 * };
 * @endcode
 */
template<typename Derived, typename BitWidth>
class adc {
public:
  using sample_type = typename detail::adc_sample_type<BitWidth>::type;
  using bit_width   = BitWidth;

  static constexpr unsigned bit_depth = [] {
    if constexpr (std::is_same_v<BitWidth, adc16_t>) return 16u;
    else if constexpr (std::is_same_v<BitWidth, adc24_t>) return 24u;
    else return 32u;
  }();

  /// @brief Read a raw sample from the ADC.
  /// @return Raw sample in the range [0, 2^bit_depth - 1].
  [[nodiscard]] sample_type read(this auto& self) {
    return self.read_impl();
  }

protected:
  adc()  = default;
  ~adc() = default;
};

} // namespace hal
