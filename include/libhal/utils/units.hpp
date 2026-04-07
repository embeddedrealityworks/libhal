#pragma once

#include <chrono>
#include <cstdint>
#include <numbers>
#include <ratio>

/// @file units.hpp
/// @brief Physical-unit type aliases and compile-time literal operators.
///
/// All physical quantities are represented as primitive C++ types — @c float
/// for real-valued measurements and @c std::uint32_t for exact integer
/// frequencies.  The internal SI base unit for each quantity is noted in the
/// type alias documentation below.
///
/// Bring the literals into scope with:
/// @code{.cpp}
/// using namespace hal::units::literals;
/// @endcode

namespace hal::units {

/// @name Integer type aliases
/// Shorthand aliases for fixed-width integer types.
/// @{
using byte = std::uint8_t;   ///< Unsigned 8-bit byte.
using u8   = std::uint8_t;   ///< Unsigned 8-bit integer.
using u16  = std::uint16_t;  ///< Unsigned 16-bit integer.
using u32  = std::uint32_t;  ///< Unsigned 32-bit integer.
using u64  = std::uint64_t;  ///< Unsigned 64-bit integer.
using uptr = std::uintptr_t; ///< Unsigned integer wide enough to hold a pointer.

using i8   = std::int8_t;    ///< Signed 8-bit integer.
using i16  = std::int16_t;   ///< Signed 16-bit integer.
using i32  = std::int32_t;   ///< Signed 32-bit integer.
using i64  = std::int64_t;   ///< Signed 64-bit integer.
/// @}

/// @name Physical quantity type aliases
/// Each alias names its SI base unit in a comment.
/// @{
using time          = std::chrono::seconds; ///< Duration — seconds.
using freq          = float;                ///< Frequency — Hertz.
using force         = float;                ///< Force — Newton.
using current       = float;                ///< Electric current — Ampere.
using voltage       = float;                ///< Voltage — Volt.
using temperature   = float;                ///< Temperature — Kelvin.
using speed         = float;                ///< Speed — metres per second.
using rpm           = float;                ///< Rotational speed — revolutions per minute.
using length        = float;                ///< Length — metres.
using angle         = float;                ///< Angle — radians.
using acceleration  = float;                ///< Acceleration — metres per second squared.
using mass          = float;                ///< Mass — kilograms.
/// @}

/// @name Temperature helper functions
/// Accept any numeric value including negatives and return a @c temperature
/// (Kelvin).  The literal operators @c _K, @c _C, @c _F provide the same
/// conversion at compile time.
/// @{

/**
 * @brief Convert a Celsius value to Kelvin.
 * @param p_value Temperature in °C.
 * @return Equivalent temperature in Kelvin.
 */
[[nodiscard]] constexpr temperature celsius(float p_value) noexcept {
  return static_cast<temperature>(static_cast<long double>(p_value) + 273.15L);
}

/**
 * @brief Convert a Fahrenheit value to Kelvin.
 * @param p_value Temperature in °F.
 * @return Equivalent temperature in Kelvin.
 */
[[nodiscard]] constexpr temperature fahrenheit(float p_value) noexcept {
  return static_cast<temperature>(
      static_cast<long double>((p_value - 32.0L) * (5.0L / 9.0L) + 273.15L));
}

/**
 * @brief Identity conversion — Kelvin to Kelvin.
 * @param p_value Temperature in Kelvin.
 * @return The same value as a @c temperature.
 */
[[nodiscard]] constexpr temperature kelvin(float p_value) noexcept {
  return p_value;
}
/// @}

namespace literals {

/// @name Frequency literals
/// Floating-point overloads return @c hal::units::freq (float).
/// Integer overloads return @c hal::units::u32 (exact bit-pattern).
/// @{
[[nodiscard]] consteval auto operator""_Hz(long double p_value) noexcept
    { return static_cast<freq>(p_value); }                          ///< Hertz (float).
[[nodiscard]] consteval auto operator""_kHz(long double p_value) noexcept
    { return static_cast<freq>(p_value * std::kilo::num); }         ///< Kilohertz (float).
[[nodiscard]] consteval auto operator""_MHz(long double p_value) noexcept
    { return static_cast<freq>(p_value * std::mega::num); }         ///< Megahertz (float).
[[nodiscard]] consteval auto operator""_GHz(long double p_value) noexcept
    { return static_cast<freq>(p_value * std::giga::num); }         ///< Gigahertz (float).

[[nodiscard]] consteval auto operator""_Hz(unsigned long long p_value) noexcept
    { return static_cast<u32>(p_value); }                           ///< Hertz (u32).
[[nodiscard]] consteval auto operator""_kHz(unsigned long long p_value) noexcept
    { return static_cast<u32>(p_value * std::kilo::num); }          ///< Kilohertz (u32).
[[nodiscard]] consteval auto operator""_MHz(unsigned long long p_value) noexcept
    { return static_cast<u32>(p_value * std::mega::num); }          ///< Megahertz (u32).
[[nodiscard]] consteval auto operator""_GHz(unsigned long long p_value) noexcept
    { return static_cast<u32>(p_value * std::giga::num); }          ///< Gigahertz (u32).
/// @}

/// @name Time literals
/// All time literals return @c hal::units::time (std::chrono::seconds).
/// Sub-second values are truncated to whole seconds.
/// @{
[[nodiscard]] consteval auto operator""_ns(unsigned long long p_value) noexcept
    { return time(static_cast<time::rep>(p_value / std::nano::den)); }    ///< Nanoseconds → seconds.
[[nodiscard]] consteval auto operator""_us(unsigned long long p_value) noexcept
    { return time(static_cast<time::rep>(p_value) / std::micro::den); }  ///< Microseconds → seconds.
[[nodiscard]] consteval auto operator""_ms(unsigned long long p_value) noexcept
    { return time(static_cast<time::rep>(p_value) / std::milli::den); }  ///< Milliseconds → seconds.
[[nodiscard]] consteval auto operator""_s(unsigned long long p_value) noexcept
    { return time(static_cast<time::rep>(p_value)); }                     ///< Seconds.
/// @}

/// @name Force literals
/// All force literals return @c hal::units::force (float, Newtons).
/// @{
[[nodiscard]] consteval auto operator""_N(long double p_value) noexcept
    { return static_cast<force>(p_value); }                                ///< Newtons (float).
[[nodiscard]] consteval auto operator""_mN(long double p_value) noexcept
    { return static_cast<force>(p_value / std::milli::den); }             ///< Millinewtons → N (float).
[[nodiscard]] consteval auto operator""_kN(long double p_value) noexcept
    { return static_cast<force>(p_value * std::kilo::num); }              ///< Kilonewtons → N (float).

[[nodiscard]] consteval auto operator""_N(unsigned long long p_value) noexcept
    { return operator""_N(static_cast<long double>(p_value)); }           ///< Newtons (u32 input).
[[nodiscard]] consteval auto operator""_mN(unsigned long long p_value) noexcept
    { return operator""_mN(static_cast<long double>(p_value)); }          ///< Millinewtons → N (u32 input).
[[nodiscard]] consteval auto operator""_kN(unsigned long long p_value) noexcept
    { return operator""_kN(static_cast<long double>(p_value)); }          ///< Kilonewtons → N (u32 input).
/// @}

/// @name Current literals
/// All current literals return @c hal::units::current (float, Amperes).
/// @{
[[nodiscard]] consteval auto operator""_A(long double p_value) noexcept
    { return static_cast<current>(p_value); }                              ///< Amperes (float).
[[nodiscard]] consteval auto operator""_mA(long double p_value) noexcept
    { return static_cast<current>(p_value / std::milli::den); }           ///< Milliamperes → A (float).
[[nodiscard]] consteval auto operator""_uA(long double p_value) noexcept
    { return static_cast<current>(p_value / std::micro::den); }           ///< Microamperes → A (float).

[[nodiscard]] consteval auto operator""_A(unsigned long long p_value) noexcept
    { return operator""_A(static_cast<long double>(p_value)); }           ///< Amperes (u32 input).
[[nodiscard]] consteval auto operator""_mA(unsigned long long p_value) noexcept
    { return operator""_mA(static_cast<long double>(p_value)); }          ///< Milliamperes → A (u32 input).
[[nodiscard]] consteval auto operator""_uA(unsigned long long p_value) noexcept
    { return operator""_uA(static_cast<long double>(p_value)); }          ///< Microamperes → A (u32 input).
/// @}

/// @name Voltage literals
/// All voltage literals return @c hal::units::voltage (float, Volts).
/// @{
[[nodiscard]] consteval auto operator""_V(long double p_value) noexcept
    { return static_cast<voltage>(p_value); }                              ///< Volts (float).
[[nodiscard]] consteval auto operator""_mV(long double p_value) noexcept
    { return static_cast<voltage>(p_value / std::milli::den); }           ///< Millivolts → V (float).
[[nodiscard]] consteval auto operator""_uV(long double p_value) noexcept
    { return static_cast<voltage>(p_value / std::micro::den); }           ///< Microvolts → V (float).
[[nodiscard]] consteval auto operator""_kV(long double p_value) noexcept
    { return static_cast<voltage>(p_value * std::kilo::num); }            ///< Kilovolts → V (float).

[[nodiscard]] consteval auto operator""_V(unsigned long long p_value) noexcept
    { return operator""_V(static_cast<long double>(p_value)); }           ///< Volts (u32 input).
[[nodiscard]] consteval auto operator""_mV(unsigned long long p_value) noexcept
    { return operator""_mV(static_cast<long double>(p_value)); }          ///< Millivolts → V (u32 input).
[[nodiscard]] consteval auto operator""_uV(unsigned long long p_value) noexcept
    { return operator""_uV(static_cast<long double>(p_value)); }          ///< Microvolts → V (u32 input).
[[nodiscard]] consteval auto operator""_kV(unsigned long long p_value) noexcept
    { return operator""_kV(static_cast<long double>(p_value)); }          ///< Kilovolts → V (u32 input).
/// @}

/// @name Temperature literals
/// All temperature literals return @c hal::units::temperature (float, Kelvin).
/// @{
[[nodiscard]] consteval auto operator""_K(long double p_value) noexcept
    { return static_cast<temperature>(p_value); }                          ///< Kelvin (float).
[[nodiscard]] consteval auto operator""_C(long double p_value) noexcept
    { return static_cast<temperature>(p_value + 273.15L); }               ///< Celsius → K (float).
[[nodiscard]] consteval auto operator""_F(long double p_value) noexcept
    { return static_cast<temperature>((p_value - 32.0L) * (5.0L / 9.0L) + 273.15L); } ///< Fahrenheit → K (float).

[[nodiscard]] consteval auto operator""_K(unsigned long long p_value) noexcept
    { return operator""_K(static_cast<long double>(p_value)); }           ///< Kelvin (u32 input).
[[nodiscard]] consteval auto operator""_C(unsigned long long p_value) noexcept
    { return operator""_C(static_cast<long double>(p_value)); }           ///< Celsius → K (u32 input).
[[nodiscard]] consteval auto operator""_F(unsigned long long p_value) noexcept
    { return operator""_F(static_cast<long double>(p_value)); }           ///< Fahrenheit → K (u32 input).
/// @}

/// @name Rotational-speed literals
/// All RPM literals return @c hal::units::rpm (float).
/// @{
[[nodiscard]] consteval auto operator""_rpm(long double p_value) noexcept
    { return static_cast<rpm>(p_value); }                                  ///< Revolutions per minute (float).
[[nodiscard]] consteval auto operator""_rpm(unsigned long long p_value) noexcept
    { return static_cast<rpm>(p_value); }                                  ///< Revolutions per minute (u32 input).
/// @}

/// @name Length literals
/// All length literals return @c hal::units::length (float, metres).
/// @{
[[nodiscard]] consteval auto operator""_m(long double p_value) noexcept
    { return static_cast<length>(p_value); }                               ///< Metres (float).
[[nodiscard]] consteval auto operator""_km(long double p_value) noexcept
    { return static_cast<length>(p_value * std::kilo::num); }             ///< Kilometres → m (float).
[[nodiscard]] consteval auto operator""_cm(long double p_value) noexcept
    { return static_cast<length>(p_value / std::centi::den); }            ///< Centimetres → m (float).
[[nodiscard]] consteval auto operator""_mm(long double p_value) noexcept
    { return static_cast<length>(p_value / std::milli::den); }            ///< Millimetres → m (float).
[[nodiscard]] consteval auto operator""_um(long double p_value) noexcept
    { return static_cast<length>(p_value / std::micro::den); }            ///< Micrometres → m (float).
[[nodiscard]] consteval auto operator""_inch(long double p_value) noexcept {
  constexpr long double inch_to_meter = 0.0254L;
  return static_cast<length>(p_value * inch_to_meter); }                  ///< Inches → m (float, 1 in = 0.0254 m).
[[nodiscard]] consteval auto operator""_yards(long double p_value) noexcept {
  constexpr long double yard_to_meter = 0.9144L;
  return static_cast<length>(p_value * yard_to_meter); }                  ///< Yards → m (float, 1 yd = 0.9144 m).
[[nodiscard]] consteval auto operator""_miles(long double p_value) noexcept {
  constexpr long double miles_to_meter = 1609.344L;
  return static_cast<length>(p_value * miles_to_meter); }                 ///< Miles → m (float, 1 mi = 1609.344 m).

[[nodiscard]] consteval auto operator""_m(unsigned long long p_value) noexcept
    { return operator""_m(static_cast<long double>(p_value)); }           ///< Metres (u32 input).
[[nodiscard]] consteval auto operator""_km(unsigned long long p_value) noexcept
    { return operator""_km(static_cast<long double>(p_value)); }          ///< Kilometres → m (u32 input).
[[nodiscard]] consteval auto operator""_cm(unsigned long long p_value) noexcept
    { return operator""_cm(static_cast<long double>(p_value)); }          ///< Centimetres → m (u32 input).
[[nodiscard]] consteval auto operator""_mm(unsigned long long p_value) noexcept
    { return operator""_mm(static_cast<long double>(p_value)); }          ///< Millimetres → m (u32 input).
[[nodiscard]] consteval auto operator""_um(unsigned long long p_value) noexcept
    { return operator""_um(static_cast<long double>(p_value)); }          ///< Micrometres → m (u32 input).
[[nodiscard]] consteval auto operator""_inch(unsigned long long p_value) noexcept
    { return operator""_inch(static_cast<long double>(p_value)); }        ///< Inches → m (u32 input).
[[nodiscard]] consteval auto operator""_yards(unsigned long long p_value) noexcept
    { return operator""_yards(static_cast<long double>(p_value)); }       ///< Yards → m (u32 input).
[[nodiscard]] consteval auto operator""_miles(unsigned long long p_value) noexcept
    { return operator""_miles(static_cast<long double>(p_value)); }       ///< Miles → m (u32 input).
/// @}

/// @name Speed literals
/// All speed literals return @c hal::units::speed (float, m/s).
/// @{
[[nodiscard]] consteval auto operator""_mps(long double p_value) noexcept
    { return static_cast<speed>(p_value); }                                ///< Metres per second (float).
[[nodiscard]] consteval auto operator""_kph(long double p_value) noexcept {
  constexpr long double kph_to_mps = (1000.0L / 3600.0L);
  return static_cast<speed>(p_value * kph_to_mps); }                      ///< km/h → m/s (float).
[[nodiscard]] consteval auto operator""_knots(long double p_value) noexcept {
  constexpr long double knots_to_mps = 1852.0L / 3600.0L;
  return static_cast<speed>(p_value * knots_to_mps); }                    ///< Knots → m/s (float, 1 kn = 1852/3600 m/s).

[[nodiscard]] consteval auto operator""_mps(unsigned long long p_value) noexcept
    { return operator""_mps(static_cast<long double>(p_value)); }         ///< Metres per second (u32 input).
[[nodiscard]] consteval auto operator""_kph(unsigned long long p_value) noexcept
    { return operator""_kph(static_cast<long double>(p_value)); }         ///< km/h → m/s (u32 input).
[[nodiscard]] consteval auto operator""_knots(unsigned long long p_value) noexcept
    { return operator""_knots(static_cast<long double>(p_value)); }       ///< Knots → m/s (u32 input).
/// @}

/// @name Angle literals
/// All angle literals return @c hal::units::angle (float, radians).
/// @{
[[nodiscard]] consteval auto operator""_rad(long double p_value) noexcept
    { return static_cast<angle>(p_value); }                                ///< Radians (float).
[[nodiscard]] consteval auto operator""_deg(long double p_value) noexcept
    { return static_cast<angle>(p_value * std::numbers::pi_v<long double> / 180.0L); } ///< Degrees → radians (float).

[[nodiscard]] consteval auto operator""_rad(unsigned long long p_value) noexcept
    { return operator""_rad(static_cast<long double>(p_value)); }         ///< Radians (u32 input).
[[nodiscard]] consteval auto operator""_deg(unsigned long long p_value) noexcept
    { return operator""_deg(static_cast<long double>(p_value)); }         ///< Degrees → radians (u32 input).
/// @}

/// @name Acceleration literals
/// All acceleration literals return @c hal::units::acceleration (float, m/s²).
/// @{
[[nodiscard]] consteval auto operator""_mps2(long double p_value) noexcept
    { return static_cast<acceleration>(p_value); }                         ///< Metres per second squared (float).
[[nodiscard]] consteval auto operator""_G(long double p_value) noexcept
    { return static_cast<acceleration>(p_value * 9.80665L); }             ///< Standard gravity → m/s² (float, 1 G = 9.80665 m/s²).

[[nodiscard]] consteval auto operator""_mps2(unsigned long long p_value) noexcept
    { return operator""_mps2(static_cast<long double>(p_value)); }        ///< Metres per second squared (u32 input).
[[nodiscard]] consteval auto operator""_G(unsigned long long p_value) noexcept
    { return operator""_G(static_cast<long double>(p_value)); }           ///< Standard gravity → m/s² (u32 input).
/// @}

/// @name Mass literals
/// All mass literals return @c hal::units::mass (float, kilograms).
/// @{
[[nodiscard]] consteval auto operator""_kg(long double p_value) noexcept
    { return static_cast<mass>(p_value); }                                 ///< Kilograms (float).
[[nodiscard]] consteval auto operator""_g(long double p_value) noexcept
    { return static_cast<mass>(p_value / std::kilo::num); }               ///< Grams → kg (float).
[[nodiscard]] consteval auto operator""_mg(long double p_value) noexcept
    { return static_cast<mass>(p_value / std::mega::num); }               ///< Milligrams → kg (float).

[[nodiscard]] consteval auto operator""_kg(unsigned long long p_value) noexcept
    { return operator""_kg(static_cast<long double>(p_value)); }          ///< Kilograms (u32 input).
[[nodiscard]] consteval auto operator""_g(unsigned long long p_value) noexcept
    { return operator""_g(static_cast<long double>(p_value)); }           ///< Grams → kg (u32 input).
[[nodiscard]] consteval auto operator""_mg(unsigned long long p_value) noexcept
    { return operator""_mg(static_cast<long double>(p_value)); }          ///< Milligrams → kg (u32 input).
/// @}

/// @name Integer cast literals
/// Compact notation for constructing exact fixed-width integers at compile
/// time.  Useful when a function expects a specific integer width.
/// @{
[[nodiscard]] consteval auto operator""_u8(unsigned long long p_value) noexcept
    { return static_cast<u8>(p_value); }   ///< Cast to @c u8.
[[nodiscard]] consteval auto operator""_u16(unsigned long long p_value) noexcept
    { return static_cast<u16>(p_value); }  ///< Cast to @c u16.
[[nodiscard]] consteval auto operator""_u32(unsigned long long p_value) noexcept
    { return static_cast<u32>(p_value); }  ///< Cast to @c u32.
[[nodiscard]] consteval auto operator""_u64(unsigned long long p_value) noexcept
    { return static_cast<u64>(p_value); }  ///< Cast to @c u64.

[[nodiscard]] consteval auto operator""_i8(unsigned long long p_value) noexcept
    { return static_cast<i8>(p_value); }   ///< Cast to @c i8.
[[nodiscard]] consteval auto operator""_i16(unsigned long long p_value) noexcept
    { return static_cast<i16>(p_value); }  ///< Cast to @c i16.
[[nodiscard]] consteval auto operator""_i32(unsigned long long p_value) noexcept
    { return static_cast<i32>(p_value); }  ///< Cast to @c i32.
[[nodiscard]] consteval auto operator""_i64(unsigned long long p_value) noexcept
    { return static_cast<i64>(p_value); }  ///< Cast to @c i64.
/// @}

} // namespace literals
} // namespace hal::units
