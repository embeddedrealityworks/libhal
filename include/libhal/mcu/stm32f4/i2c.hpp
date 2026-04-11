#pragma once

#include <cstdint>

/// @file i2c.hpp
/// @brief STM32F4 I2C timing constants and bus-manager type aliases.
///
/// The STM32F4 I2C peripheral uses a single TIMINGR register to set all
/// timing parameters (PRESC, SCLDEL, SDADEL, SCLH, SCLL).  The values
/// below are pre-computed for APB1 = 42 MHz with the analog filter enabled
/// and digital noise filter disabled (DNF = 0).
///
/// @par Usage
/// @code{.cpp}
/// #include <libhal/buses/i2c_bus.hpp>
/// #include <libhal/mcu/stm32f4/i2c.hpp>
///
/// using bus1 = hal::i2c_bus<mcu::stm32::i2c1, "i2c1_ev", "i2c1_err">;
///
/// // In your clock init, write the timing before enabling the bus:
/// groov::sync_write<groov::blocking>(
///     mcu::stm32::i2c1("timingr"_r = hal::mcu::stm32f4::i2c_timing_fm_400k));
/// @endcode

namespace hal::mcu::stm32f4 {

/// @name I2C TIMINGR pre-computed values (APB1 = 42 MHz)
/// @{

/// Standard-mode 100 kHz — PRESC=1, SCLDEL=4, SDADEL=2, SCLH=0x0F, SCLL=0x13
inline constexpr std::uint32_t i2c_timing_sm_100k  = 0x10420F13u;

/// Fast-mode 400 kHz — PRESC=0, SCLDEL=3, SDADEL=1, SCLH=0x03, SCLL=0x09
inline constexpr std::uint32_t i2c_timing_fm_400k  = 0x00310309u;

/// Fast-mode Plus 1 MHz — PRESC=0, SCLDEL=2, SDADEL=0, SCLH=0x00, SCLL=0x04
/// @note Requires SDA/SCL pins configured with high drive strength.
inline constexpr std::uint32_t i2c_timing_fmp_1mhz = 0x00200004u;

/// @}

/// @name Bus instance type aliases
/// Uncomment after including buses/i2c_bus.hpp and the stm32 subproject.
/// @{

// using i2c1_bus = hal::i2c_bus<mcu::stm32::i2c1, "i2c1_ev", "i2c1_err">;
// using i2c2_bus = hal::i2c_bus<mcu::stm32::i2c2, "i2c2_ev", "i2c2_err">;
// using i2c3_bus = hal::i2c_bus<mcu::stm32::i2c3, "i2c3_ev", "i2c3_err">;

/// @}

} // namespace hal::mcu::stm32f4
