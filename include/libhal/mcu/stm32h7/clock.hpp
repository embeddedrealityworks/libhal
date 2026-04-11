#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32H7 RCC clock initialisation.
///
/// @par STM32H7 clock limits (rev V / single-core variants)
///  - SYSCLK max: 480 MHz (VOS0, LDO mode)
///  - HSI: 64 MHz, HSE: 4–48 MHz, CSI: 4 MHz, HSI48: 48 MHz
///  - PLL1/2/3: DIVM/DIVN/DIVP/DIVQ/DIVR (fractional N supported)
///  - AHB max: 240 MHz (HPRE prescaler ≥ 2 when SYSCLK > 200 MHz)
///  - APB1/2/3/4 max: 120 MHz
///
/// @par Dual-core note (H745/H755/H747/H757)
/// Each core (M7 + M4) has its own SYSCLK domain.  The M4 domain is
/// configured separately via D2 clock registers.  This stub covers
/// the M7 (primary) domain only.

namespace hal::mcu::stm32h7 {

/// @brief Compile-time clock configuration for STM32H7 (M7 domain).
struct clock_cfg {
    std::uint32_t hse_hz     = 0;             ///< HSE frequency in Hz (0 = use HSI 64 MHz).
    std::uint32_t sysclk_hz  = 64'000'000;    ///< Target SYSCLK (max 480 MHz at VOS0).
    std::uint32_t ahb_div    = 2;             ///< AHB prescaler (≥2 when SYSCLK > 200 MHz).
    std::uint32_t apb1_div   = 2;             ///< APB1 max 120 MHz.
    std::uint32_t apb2_div   = 2;             ///< APB2 max 120 MHz.
    std::uint32_t apb3_div   = 2;             ///< APB3 max 120 MHz.
    std::uint32_t apb4_div   = 2;             ///< APB4 max 120 MHz.
};

/**
 * @brief CIB component that configures the STM32H7 PLL1 and bus clocks.
 * @tparam Cfg  Compile-time @ref clock_cfg.
 * @todo Implement groov writes to the H7 RCC register block.
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: PWR VOS0/1 selection, PLL1 DIVM/N/P, AHB/APB prescalers, SW=PLL1
        // Note: H7 requires disabling the D-cache and I-cache before
        //       reconfiguring clocks if already running at high speed
    }
};

} // namespace hal::mcu::stm32h7
