#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32F3 RCC clock initialisation.
///
/// @par STM32F3 clock limits
///  - SYSCLK max: 72 MHz
///  - HSI: 8 MHz, HSE: 4–32 MHz
///  - PLL source: HSI/2 or HSE; multiplier × 2–16 (same as F1 but with HSI support)
///  - AHB max: 72 MHz
///  - APB1 max: 36 MHz, APB2 max: 72 MHz
///
/// @note F3 has the same PLL topology as F1 but adds the FPU and a richer
///       peripheral set.  GPIO layout is identical to F4.

namespace hal::mcu::stm32f3 {

/// @brief Compile-time clock configuration for STM32F3.
struct clock_cfg {
    std::uint32_t hse_hz    = 0;           ///< HSE frequency in Hz (0 = use HSI 8 MHz).
    std::uint32_t sysclk_hz = 8'000'000;   ///< Target SYSCLK (max 72 MHz).
    std::uint32_t ahb_div   = 1;
    std::uint32_t apb1_div  = 2;           ///< APB1 max 36 MHz.
    std::uint32_t apb2_div  = 1;           ///< APB2 max 72 MHz.
};

/**
 * @brief CIB component that configures the STM32F3 PLL and bus clocks.
 * @tparam Cfg  Compile-time @ref clock_cfg.
 * @todo Implement groov writes to @c mcu::stm32::rcc.
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: similar to F1 flow; F3 also supports HSI as direct PLL source
    }
};

} // namespace hal::mcu::stm32f3
