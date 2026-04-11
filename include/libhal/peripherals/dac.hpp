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

struct dac8_t  {};  ///< Tag type selecting  8-bit DAC resolution.
struct dac10_t {};  ///< Tag type selecting 10-bit DAC resolution.
struct dac12_t {};  ///< Tag type selecting 12-bit DAC resolution.
struct dac16_t {};  ///< Tag type selecting 16-bit DAC resolution.

inline constexpr dac8_t  dac8{};
inline constexpr dac10_t dac10{};
inline constexpr dac12_t dac12{};
inline constexpr dac16_t dac16{};

// ---------------------------------------------------------------------------
// Sample-type mapping (kept for use by bus-manager implementations)
// ---------------------------------------------------------------------------

namespace detail {

template<typename BitWidth> struct dac_sample_type;
template<> struct dac_sample_type<dac8_t>  { using type = u8;  };
template<> struct dac_sample_type<dac10_t> { using type = u16; };
template<> struct dac_sample_type<dac12_t> { using type = u16; };
template<> struct dac_sample_type<dac16_t> { using type = u16; };

/// @brief Compile-time bit-depth for a DAC resolution tag.
template<typename BitWidth>
inline constexpr unsigned dac_bit_depth =
    std::is_same_v<BitWidth, dac8_t>  ?  8u :
    std::is_same_v<BitWidth, dac10_t> ? 10u :
    std::is_same_v<BitWidth, dac12_t> ? 12u : 16u;

} // namespace detail

// ---------------------------------------------------------------------------
// Concept
// ---------------------------------------------------------------------------

/**
 * @brief Concept for a DAC channel driver parameterised by sample type.
 *
 * A conforming type must expose a @c write(SampleT) member that sets the
 * DAC output register.  The resolution is encoded by the @ref dac8_t …
 * @ref dac16_t tag type passed to the concrete driver.
 *
 * @tparam T       The DAC driver type to check.
 * @tparam SampleT The raw sample integer type (e.g. @c u8, @c u16).
 *
 * @par Example
 * @code{.cpp}
 * template <hal::dac_channel<u16> Dac>
 * void set_half_scale(Dac& dac) { dac.write(2047u); }
 * @endcode
 */
template <typename T, typename SampleT>
concept dac_channel = requires(T t, SampleT v) {
    t.write(v);
};

} // namespace hal
