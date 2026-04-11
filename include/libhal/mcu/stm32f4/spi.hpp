#pragma once

#include <cstdint>

/// @file spi.hpp
/// @brief STM32F4 SPI baud-rate helpers and bus-manager type aliases.
///
/// The STM32F4 SPI baud rate is set by a 3-bit divider field (CR1.BR)
/// applied to the APBx peripheral clock.  Helper functions and constants
/// here compute the correct divider code for a target bit rate.
///
/// @par Usage
/// @code{.cpp}
/// #include <libhal/buses/spi_bus.hpp>
/// #include <libhal/mcu/stm32f4/spi.hpp>
///
/// using bus1 = hal::spi_bus<mcu::stm32::spi1, "spi1_ev", "spi1_err">;
/// @endcode

namespace hal::mcu::stm32f4 {

/// @brief SPI CR1.BR baud rate divider codes.
enum class spi_baud : std::uint8_t {
    div2   = 0b000u,  ///< f_pclk / 2
    div4   = 0b001u,  ///< f_pclk / 4
    div8   = 0b010u,  ///< f_pclk / 8
    div16  = 0b011u,  ///< f_pclk / 16
    div32  = 0b100u,  ///< f_pclk / 32
    div64  = 0b101u,  ///< f_pclk / 64
    div128 = 0b110u,  ///< f_pclk / 128
    div256 = 0b111u,  ///< f_pclk / 256
};

/// @brief Select the smallest divider that keeps the SPI clock ≤ @p max_hz.
/// @param pclk_hz  APBx peripheral clock frequency in Hz.
/// @param max_hz   Maximum SPI clock frequency in Hz.
/// @return @ref spi_baud divider code.
constexpr spi_baud spi_baud_for(std::uint32_t pclk_hz,
                                std::uint32_t max_hz) noexcept {
    if (pclk_hz / 2   <= max_hz) return spi_baud::div2;
    if (pclk_hz / 4   <= max_hz) return spi_baud::div4;
    if (pclk_hz / 8   <= max_hz) return spi_baud::div8;
    if (pclk_hz / 16  <= max_hz) return spi_baud::div16;
    if (pclk_hz / 32  <= max_hz) return spi_baud::div32;
    if (pclk_hz / 64  <= max_hz) return spi_baud::div64;
    if (pclk_hz / 128 <= max_hz) return spi_baud::div128;
    return spi_baud::div256;
}

/// @name Bus instance type aliases
/// Uncomment after including buses/spi_bus.hpp and the stm32 subproject.
/// @{

// using spi1_bus = hal::spi_bus<mcu::stm32::spi1, "spi1_ev", "spi1_err">;
// using spi2_bus = hal::spi_bus<mcu::stm32::spi2, "spi2_ev", "spi2_err">;
// using spi3_bus = hal::spi_bus<mcu::stm32::spi3, "spi3_ev", "spi3_err">;

/// @}

} // namespace hal::mcu::stm32f4
