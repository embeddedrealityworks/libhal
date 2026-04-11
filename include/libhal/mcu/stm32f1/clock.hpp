#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32F1 RCC clock initialisation.
///
/// @par STM32F1 clock limits
///  - SYSCLK max: 72 MHz
///  - HSI: 8 MHz, HSE: 4–16 MHz
///  - PLL source: HSI/2 or HSE; multiplier × 2–16
///  - AHB max: 72 MHz
///  - APB1 max: 36 MHz, APB2 max: 72 MHz
///
/// @par Usage
/// @code{.cpp}
/// using clock = hal::mcu::stm32f1::clock_init<
///     hal::mcu::stm32f1::clock_cfg{ .hse_hz = 8'000'000, .sysclk_hz = 72'000'000 }>;
/// @endcode

namespace hal::mcu::stm32f1 {

/// @brief Compile-time clock configuration for STM32F1.
struct clock_cfg {
    std::uint32_t hse_hz    = 0;           ///< HSE frequency in Hz (0 = use HSI 8 MHz).
    std::uint32_t sysclk_hz = 8'000'000;   ///< Target SYSCLK (max 72 MHz).
    std::uint32_t ahb_div   = 1;           ///< AHB prescaler.
    std::uint32_t apb1_div  = 2;           ///< APB1 prescaler (max 36 MHz → div≥2 at 72 MHz).
    std::uint32_t apb2_div  = 1;           ///< APB2 prescaler (max 72 MHz).
};

/**
 * @brief CIB component that configures the STM32F1 PLL and bus clocks.
 * @tparam Cfg  Compile-time @ref clock_cfg.
 * @todo Implement groov writes to @c mcu::stm32::rcc.
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: HSE/HSI enable, FLASH latency (≥2 wait states at 72 MHz),
        //       PLL-MUL/PLLSRC, APB1/APB2 prescalers, SW switch to PLL
    }
};

} // namespace hal::mcu::stm32f1
