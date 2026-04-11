#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32F7 RCC clock initialisation.
///
/// @par STM32F7 clock limits
///  - SYSCLK max: 216 MHz
///  - HSI: 16 MHz, HSE: 4–26 MHz
///  - PLL: PLLM/N/P/Q/R (same topology as F4, wider N range)
///  - AHB max: 216 MHz
///  - APB1 max: 54 MHz, APB2 max: 108 MHz
///  - FLASH wait states: 0–7 (7 required at 216 MHz, VOS scale 1)
///
/// @note F7 RCC register layout is nearly identical to F4.  Key difference:
///       PLLCFGR.PLLR field added for a 4th PLL output.

namespace hal::mcu::stm32f7 {

/// @brief Compile-time clock configuration for STM32F7.
struct clock_cfg {
    std::uint32_t hse_hz    = 0;            ///< HSE frequency in Hz (0 = use HSI 16 MHz).
    std::uint32_t sysclk_hz = 16'000'000;   ///< Target SYSCLK (max 216 MHz).
    std::uint32_t ahb_div   = 1;
    std::uint32_t apb1_div  = 4;            ///< APB1 max 54 MHz → div≥4 at 216 MHz.
    std::uint32_t apb2_div  = 2;            ///< APB2 max 108 MHz → div≥2 at 216 MHz.
};

/**
 * @brief CIB component that configures the STM32F7 PLL and bus clocks.
 * @tparam Cfg  Compile-time @ref clock_cfg.
 * @todo Implement groov writes to @c mcu::stm32::rcc.
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: identical flow to STM32F4; add PWR VOS scale 1 for 216 MHz
    }
};

} // namespace hal::mcu::stm32f7
