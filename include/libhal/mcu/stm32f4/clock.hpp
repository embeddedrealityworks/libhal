#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"

/// @file clock.hpp
/// @brief CIB component for STM32F4 RCC / PLL clock initialisation.
///
/// @par Intended usage
/// @code{.cpp}
/// // Board header — configure clocks before any peripheral init:
/// using clock = hal::mcu::stm32f4::clock_init<
///     hal::mcu::stm32f4::clock_cfg{
///         .hse_hz     = 8'000'000,
///         .sysclk_hz  = 168'000'000,
///         .ahb_div    = 1,
///         .apb1_div   = 4,
///         .apb2_div   = 2,
///     }>;
///
/// cib::nexus<clock, bus1::init, ...> nexus{};
/// nexus.init();
/// cib::service<hal::startup>();
/// @endcode

namespace hal::mcu::stm32f4 {

/// @brief Compile-time clock configuration parameters.
struct clock_cfg {
    std::uint32_t hse_hz     = 0;            ///< External oscillator frequency (0 = use HSI).
    std::uint32_t sysclk_hz  = 16'000'000;   ///< Target SYSCLK frequency in Hz.
    std::uint32_t ahb_div    = 1;            ///< AHB prescaler (1, 2, 4, 8, 16, 64, 128, 256, 512).
    std::uint32_t apb1_div   = 1;            ///< APB1 prescaler (max 42 MHz on F4).
    std::uint32_t apb2_div   = 1;            ///< APB2 prescaler (max 84 MHz on F4).
};

/**
 * @brief CIB component that configures the STM32F4 PLL and bus clocks.
 *
 * Include in your @c cib::nexus before any peripheral bus managers so that
 * clocks are ready before @c i2c_bus::setup() or @c spi_bus::setup() run.
 *
 * @tparam Cfg  Compile-time @ref clock_cfg specifying oscillator and
 *              frequency targets.
 *
 * @todo Implement PLL-M/N/P calculation from @p Cfg and write to
 *       @c mcu::stm32::rcc registers via groov.
 */
template <clock_cfg Cfg>
struct clock_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    /// @brief Apply the clock configuration.
    ///
    /// Sequence:
    ///  1. Enable HSE (or confirm HSI is ready).
    ///  2. Configure FLASH latency for target SYSCLK.
    ///  3. Program PLL-M, PLL-N, PLL-P from @p Cfg.
    ///  4. Enable PLL and wait for PLLRDY.
    ///  5. Switch SYSCLK source to PLL.
    ///  6. Set AHB / APB1 / APB2 prescalers.
    ///
    /// @todo Implement using groov writes to @c mcu::stm32::rcc.
    static void setup() noexcept {
        // TODO: groov::sync_write<groov::blocking>(
        //     mcu::stm32::rcc("cr.hseon"_f = groov::enable, ...));
    }
};

} // namespace hal::mcu::stm32f4
