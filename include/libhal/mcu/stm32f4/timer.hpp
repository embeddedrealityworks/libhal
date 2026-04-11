#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../utils/units.hpp"

/// @file timer.hpp
/// @brief STM32F4 timer CIB component and prescaler helpers.
///
/// The STM32F4 has up to 14 timers:
///  - Advanced: TIM1, TIM8 (APB2)
///  - General-purpose 32-bit: TIM2, TIM5 (APB1)
///  - General-purpose 16-bit: TIM3, TIM4, TIM9–TIM14 (APB1/APB2)
///  - Basic: TIM6, TIM7 (APB1)
///
/// @par Usage
/// @code{.cpp}
/// using tim2 = hal::mcu::stm32f4::timer_init<mcu::stm32::tim2,
///                                             hal::mcu::stm32f4::timer_cfg{
///                                                 .apb_hz    = 84'000'000,
///                                                 .period_hz = 1'000,   // 1 kHz
///                                             }>;
/// cib::nexus<tim2, ...> nexus{};
/// @endcode

namespace hal::mcu::stm32f4 {
using namespace hal::units;

/// @brief Compile-time timer configuration.
struct timer_cfg {
    std::uint32_t apb_hz    = 84'000'000u;  ///< APB timer clock (2× APB1 or APB2).
    std::uint32_t period_hz = 1'000u;       ///< Overflow / update frequency in Hz.
    bool          one_shot  = false;        ///< If true, set OPM bit (one-pulse mode).
};

/// @brief Compute PSC value: largest prescaler that gives ARR ≤ 65535.
constexpr std::uint32_t timer_psc(std::uint32_t apb_hz,
                                  std::uint32_t period_hz) noexcept {
    std::uint32_t ticks = apb_hz / period_hz;
    return (ticks > 65535u) ? (ticks / 65536u) : 0u;
}

/// @brief Compute ARR value from PSC.
constexpr std::uint32_t timer_arr(std::uint32_t apb_hz,
                                  std::uint32_t period_hz,
                                  std::uint32_t psc) noexcept {
    return (apb_hz / ((psc + 1u) * period_hz)) - 1u;
}

/**
 * @brief CIB component that initialises one STM32F4 timer.
 *
 * @tparam tim_periph  Timer constexpr peripheral object (e.g. @c mcu::stm32::tim2).
 * @tparam Cfg         Compile-time @ref timer_cfg.
 *
 * @todo Implement groov writes to @c mcu::stm32::timx (PSC, ARR, CR1).
 */
template <auto& tim_periph, timer_cfg Cfg>
struct timer_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: groov::sync_write<groov::blocking>(
        //     tim_periph(
        //         "psc.psc"_f = timer_psc(Cfg.apb_hz, Cfg.period_hz),
        //         "arr.arr"_f = timer_arr(Cfg.apb_hz, Cfg.period_hz,
        //                                 timer_psc(Cfg.apb_hz, Cfg.period_hz)),
        //         "cr1.opm"_f = Cfg.one_shot ? groov::enable : groov::disable,
        //         "cr1.cen"_f = groov::enable
        //     ));
    }
};

} // namespace hal::mcu::stm32f4
