#pragma once

#include <chrono>
#include <cstdint>
#include <numbers>
#include <ratio>

namespace hal {

using byte = std::uint8_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using uptr = std::uintptr_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using time = std::chrono::seconds; // seconds
using freq = float;                // Hertz
using force = float;               // Newton
using current = float;             // Ampere
using voltage = float;             // Volt
using temperature = float;         // Kelvin
using speed = float;               // m/s
using rpm = float;                 // rpm
using length = float;              // Meters
using angle = float;               // Radians
using acceleration = float;        // Meters per second squared

namespace literals {

// freq

[[nodiscard]] consteval auto operator""_Hz(long double p_value) noexcept {
  return static_cast<freq>(p_value);
}

[[nodiscard]] consteval auto operator""_kHz(long double p_value) noexcept {
  return static_cast<freq>(p_value * std::kilo::num);
}

[[nodiscard]] consteval auto operator""_MHz(long double p_value) noexcept {
  return static_cast<freq>(p_value * std::mega::num);
}

[[nodiscard]] consteval auto operator""_GHz(long double p_value) noexcept {
  return static_cast<freq>(p_value * std::giga::num);
}

[[nodiscard]] consteval auto
operator""_Hz(unsigned long long p_value) noexcept {
  return static_cast<u32>(p_value);
}

[[nodiscard]] consteval auto
operator""_kHz(unsigned long long p_value) noexcept {
  return static_cast<u32>(p_value * std::kilo::num);
}

[[nodiscard]] consteval auto
operator""_MHz(unsigned long long p_value) noexcept {
  return static_cast<u32>(p_value * std::mega::num);
}

[[nodiscard]] consteval auto
operator""_GHz(unsigned long long p_value) noexcept {
  return static_cast<u32>(p_value * std::giga::num);
}

// time_duration

[[nodiscard]] consteval auto
operator""_ns(unsigned long long p_value) noexcept {
  return time(static_cast<time::rep>(p_value / std::nano::den));
}

[[nodiscard]] consteval auto
operator""_us(unsigned long long p_value) noexcept {
  return time(static_cast<time::rep>(p_value) / std::micro::den);
}

[[nodiscard]] consteval auto
operator""_ms(unsigned long long p_value) noexcept {
  return time(static_cast<time::rep>(p_value) / std::milli::den);
}

[[nodiscard]] consteval auto operator""_s(unsigned long long p_value) noexcept {
  return time(static_cast<time::rep>(p_value));
}

// force (Newton)

[[nodiscard]] consteval auto operator""_N(long double p_value) noexcept {
  return static_cast<force>(p_value);
}

[[nodiscard]] consteval auto operator""_mN(long double p_value) noexcept {
  return static_cast<force>(p_value / std::milli::den);
}

[[nodiscard]] consteval auto operator""_kN(long double p_value) noexcept {
  return static_cast<force>(p_value * std::kilo::num);
}

// current (Ampere)

[[nodiscard]] consteval auto operator""_A(long double p_value) noexcept {
  return static_cast<current>(p_value);
}

[[nodiscard]] consteval auto operator""_mA(long double p_value) noexcept {
  return static_cast<current>(p_value / std::milli::den);
}

[[nodiscard]] consteval auto operator""_uA(long double p_value) noexcept {
  return static_cast<current>(p_value / std::micro::den);
}

// voltage (Volt)

[[nodiscard]] consteval auto operator""_V(long double p_value) noexcept {
  return static_cast<voltage>(p_value);
}

[[nodiscard]] consteval auto operator""_mV(long double p_value) noexcept {
  return static_cast<voltage>(p_value / std::milli::den);
}

[[nodiscard]] consteval auto operator""_uV(long double p_value) noexcept {
  return static_cast<voltage>(p_value / std::micro::den);
}

[[nodiscard]] consteval auto operator""_kV(long double p_value) noexcept {
  return static_cast<voltage>(p_value * std::kilo::num);
}

// temperature (Kelvin)

[[nodiscard]] consteval auto operator""_K(long double p_value) noexcept {
  return static_cast<temperature>(p_value);
}

[[nodiscard]] consteval auto operator""_C(long double p_value) noexcept {
  return static_cast<temperature>(p_value + 273.15L);
}

[[nodiscard]] consteval auto operator""_F(long double p_value) noexcept {
  return static_cast<temperature>((p_value - 32.0L) * (5.0L / 9.0L) + 273.15L);
}

// rpm

[[nodiscard]] consteval auto operator""_rpm(long double p_value) noexcept {
  return static_cast<rpm>(p_value);
}

[[nodiscard]] consteval auto
operator""_rpm(unsigned long long p_value) noexcept {
  return static_cast<rpm>(p_value);
}

// length (Meter)

[[nodiscard]] consteval auto operator""_m(long double p_value) noexcept {
  return static_cast<length>(p_value);
}

[[nodiscard]] consteval auto operator""_km(long double p_value) noexcept {
  return static_cast<length>(p_value * std::kilo::num);
}

[[nodiscard]] consteval auto operator""_cm(long double p_value) noexcept {
  return static_cast<length>(p_value / std::centi::den);
}

[[nodiscard]] consteval auto operator""_mm(long double p_value) noexcept {
  return static_cast<length>(p_value / std::milli::den);
}

[[nodiscard]] consteval auto operator""_um(long double p_value) noexcept {
  return static_cast<length>(p_value / std::micro::den);
}

[[nodiscard]] consteval auto operator""_inch(long double p_value) noexcept {
  constexpr long double inch_to_meter = 0.0254L;
  return static_cast<length>(p_value * inch_to_meter);
}

[[nodiscard]] consteval auto operator""_yards(long double p_value) noexcept {
  constexpr long double yard_to_meter = 0.9144L;
  return static_cast<length>(p_value * yard_to_meter);
}

[[nodiscard]] consteval auto operator""_miles(long double p_value) noexcept {
  constexpr long double miles_to_meter = 1609.344L;
  return static_cast<length>(p_value * miles_to_meter);
}

// speed (m/s)
[[nodiscard]] consteval auto operator""_mps(long double p_value) noexcept {
  return static_cast<length>(p_value);
}

[[nodiscard]] consteval auto operator""_kph(long double p_value) noexcept {
  constexpr long double kph_to_mps = (1000.0L / 3600.0L);
  return static_cast<speed>(p_value * kph_to_mps);
}

[[nodiscard]] consteval auto operator""_knots(long double p_value) noexcept {
  constexpr long double knots_to_mps = 0.514444L;
  return static_cast<speed>(p_value * knots_to_mps);
}

// angle (Radian)

[[nodiscard]] consteval auto operator""_rad(long double p_value) noexcept {
  return static_cast<angle>(p_value);
}

[[nodiscard]] consteval auto operator""_deg(long double p_value) noexcept {
  return static_cast<angle>(p_value * std::numbers::pi_v<long double> / 180.0L);
}

// acceleration (m/s²) 

[[nodiscard]] consteval auto operator""_mps2(long double p_value) noexcept {
  return static_cast<acceleration>(p_value);
}

// Standard gravity: 1 G = 9.80665 m/s²
[[nodiscard]] consteval auto operator""_G(long double p_value) noexcept {
  return static_cast<acceleration>(p_value * 9.80665L);
}

// integer types

[[nodiscard]] consteval auto
operator""_u8(unsigned long long p_value) noexcept {
  return static_cast<u8>(p_value);
}

[[nodiscard]] consteval auto
operator""_u16(unsigned long long p_value) noexcept {
  return static_cast<u16>(p_value);
}

[[nodiscard]] consteval auto
operator""_u32(unsigned long long p_value) noexcept {
  return static_cast<u32>(p_value);
}

[[nodiscard]] consteval auto
operator""_u64(unsigned long long p_value) noexcept {
  return static_cast<u64>(p_value);
}

[[nodiscard]] consteval auto
operator""_i8(unsigned long long p_value) noexcept {
  return static_cast<i8>(p_value);
}

[[nodiscard]] consteval auto
operator""_i16(unsigned long long p_value) noexcept {
  return static_cast<i16>(p_value);
}

[[nodiscard]] consteval auto
operator""_i32(unsigned long long p_value) noexcept {
  return static_cast<i32>(p_value);
}

[[nodiscard]] consteval auto
operator""_i64(unsigned long long p_value) noexcept {
  return static_cast<i64>(p_value);
}

} // namespace literals
} // namespace hal
