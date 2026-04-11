#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32F0 RCC clock initialisation.
///
/// @par STM32F0 clock limits
///  - SYSCLK max: 48 MHz
///  - HSI: 8 MHz (default), HSE: 4–32 MHz (device-dependent)
///  - PLL source: HSI/2 or HSE; output range: 16–48 MHz
///  - AHB max: 48 MHz (no prescaler required for most applications)
///  - APB max: 48 MHz
///
/// @par Usage
/// @code{.cpp}
/// using clock = hal::mcu::stm32f0::clock_init<
///     hal::mcu::stm32f0::clock_cfg{ .sysclk_hz = 48'000'000 }>;
/// cib::nexus<clock, ...> nexus{};
/// @endcode

namespace hal::mcu::stm32f0 {

/// @brief Compile-time clock configuration for STM32F0.
struct clock_cfg {
    std::uint32_t hse_hz    = 0;           ///< HSE frequency in Hz (0 = use HSI 8 MHz).
    std::uint32_t sysclk_hz = 8'000'000;   ///< Target SYSCLK (max 48 MHz).
    std::uint32_t ahb_div   = 1;           ///< AHB prescaler (1, 2, 4, 8, …).
    std::uint32_t apb_div   = 1;           ///< APB prescaler (max 48 MHz).
};

/**
 * @brief CIB component that configures the STM32F0 PLL and bus clocks.
 * @tparam Cfg  Compile-time @ref clock_cfg.
 * @todo Implement groov writes to @c mcu::stm32::rcc.
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: HSI/HSE enable, FLASH latency, PLL-MUL/PREDIV, SW switch
    }
};

} // namespace hal::mcu::stm32f0
