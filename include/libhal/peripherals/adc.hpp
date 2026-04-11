#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// Resolution tag types
// ---------------------------------------------------------------------------

struct adc16_t {};  ///< Tag type selecting 16-bit ADC resolution.
struct adc24_t {};  ///< Tag type selecting 24-bit ADC resolution.
struct adc32_t {};  ///< Tag type selecting 32-bit ADC resolution.

inline constexpr adc16_t adc16{};
inline constexpr adc24_t adc24{};
inline constexpr adc32_t adc32{};

// ---------------------------------------------------------------------------
// Sample-type mapping (kept for use by bus-manager implementations)
// ---------------------------------------------------------------------------

namespace detail {

template<typename BitWidth> struct adc_sample_type;
template<> struct adc_sample_type<adc16_t> { using type = u16; };
template<> struct adc_sample_type<adc24_t> { using type = u32; };  ///< 24-bit packed into 32-bit
template<> struct adc_sample_type<adc32_t> { using type = u32; };

/// @brief Compile-time bit-depth for an ADC resolution tag.
template<typename BitWidth>
inline constexpr unsigned adc_bit_depth =
    std::is_same_v<BitWidth, adc16_t> ? 16u :
    std::is_same_v<BitWidth, adc24_t> ? 24u : 32u;

} // namespace detail

// ---------------------------------------------------------------------------
// Concept
// ---------------------------------------------------------------------------

/**
 * @brief Concept for an ADC channel driver.
 *
 * A conforming type must expose a @c read() member returning a raw integer
 * sample.  The resolution (16 / 24 / 32-bit) is encoded by the @ref adc16_t,
 * @ref adc24_t, or @ref adc32_t tag type passed to the concrete driver.
 *
 * @par Example
 * @code{.cpp}
 * template <hal::adc_channel Adc>
 * voltage to_voltage(Adc& adc, voltage vref) {
 *     return vref * adc.read() / 65535.0f;
 * }
 * @endcode
 */
template <typename T>
concept adc_channel = requires(T t) {
    { t.read() };
};

} // namespace hal
