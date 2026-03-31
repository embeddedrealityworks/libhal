#pragma once

#include <chrono>
#include <cstdint>

namespace hal {

using time_duration = std::chrono::nanoseconds;

using byte = std::uint8_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint32_t;
using uptr = std::uintptr_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using freq = float;        // Hertz
using force = float;       // Newton
using current = float;     // Ampere
using voltage = float;     // Voltage
using temperature = float; // Kelvin
using rpm = float;         // rpm
using length = float;      // Meters
using angle = float;       // Radians
using acceleration = float;

namespace literals {

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
} // namespace literals
} // namespace hal
