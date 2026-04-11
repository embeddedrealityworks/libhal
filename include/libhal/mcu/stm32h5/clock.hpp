#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32H5 RCC clock initialisation.
///
/// @par STM32H5 clock limits
///  - SYSCLK max: 250 MHz
///  - HSI: 64 MHz, HSE: 4–50 MHz, CSI: 4 MHz
///  - PLL1/2/3: DIVM/DIVN/DIVP/DIVQ/DIVR
///  - AHB max: 250 MHz
///  - APB1/2/3 max: 250 MHz (configurable prescalers)
///
/// @note H5 has a substantially redesigned RCC compared to F4/F7.
///       PLL configuration uses DIVM/DIVN/DIVP rather than M/N/P.

namespace hal::mcu::stm32h5 {

/// @brief Compile-time clock configuration for STM32H5.
struct clock_cfg {
    std::uint32_t hse_hz    = 0;            ///< HSE frequency in Hz (0 = use HSI 64 MHz).
    std::uint32_t sysclk_hz = 64'000'000;   ///< Target SYSCLK (max 250 MHz).
    std::uint32_t ahb_div   = 1;
    std::uint32_t apb1_div  = 1;
    std::uint32_t apb2_div  = 1;
    std::uint32_t apb3_div  = 1;
};

/**
 * @brief CIB component that configures the STM32H5 PLL and bus clocks.
 * @tparam Cfg  Compile-time @ref clock_cfg.
 * @todo Implement groov writes to the H5 RCC register block.
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: H5-specific RCC sequence (PWR VOS, PLL1 DIVM/N/P, SW=PLL1)
    }
};

} // namespace hal::mcu::stm32h5
