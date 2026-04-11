#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32F2 RCC clock initialisation.
///
/// @par STM32F2 clock limits
///  - SYSCLK max: 120 MHz
///  - HSI: 16 MHz, HSE: 4–26 MHz
///  - PLL: PLLM/N/P/Q, same structure as STM32F4
///  - AHB max: 120 MHz
///  - APB1 max: 30 MHz, APB2 max: 60 MHz
///
/// @note Register layout is identical to STM32F4.  Reuse @c stm32f4::clock_cfg
///       with adjusted frequency targets.

namespace hal::mcu::stm32f2 {

/// @brief Compile-time clock configuration for STM32F2.
struct clock_cfg {
    std::uint32_t hse_hz    = 0;            ///< HSE frequency in Hz (0 = use HSI 16 MHz).
    std::uint32_t sysclk_hz = 16'000'000;   ///< Target SYSCLK (max 120 MHz).
    std::uint32_t ahb_div   = 1;
    std::uint32_t apb1_div  = 4;            ///< APB1 max 30 MHz → div≥4 at 120 MHz.
    std::uint32_t apb2_div  = 2;            ///< APB2 max 60 MHz → div≥2 at 120 MHz.
};

/**
 * @brief CIB component that configures the STM32F2 PLL and bus clocks.
 * @tparam Cfg  Compile-time @ref clock_cfg.
 * @todo Implement groov writes (same RCC register layout as STM32F4).
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: identical flow to STM32F4 clock init with different PLL-M/N/P values
    }
};

} // namespace hal::mcu::stm32f2
