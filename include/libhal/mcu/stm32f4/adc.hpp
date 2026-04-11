#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/adc.hpp"

/// @file adc.hpp
/// @brief STM32F4 ADC CIB component and channel helpers.
///
/// The STM32F4 has three 12-bit SAR ADCs sharing a common clock (ADC_CLK),
/// derived from APB2 via a configurable prescaler (2/4/6/8).  Maximum
/// ADC_CLK is 36 MHz.
///
/// Available ADC resolution modes: 12-bit (default), 10-bit, 8-bit, 6-bit.
///
/// @par Usage
/// @code{.cpp}
/// using adc1 = hal::mcu::stm32f4::adc_init<mcu::stm32::adc1,
///                                           hal::mcu::stm32f4::adc_cfg{
///                                               .prescaler = 4,         // APB2/4
///                                               .resolution = 12,
///                                           }>;
/// cib::nexus<adc1, ...> nexus{};
/// @endcode

namespace hal::mcu::stm32f4 {

/// @brief Compile-time ADC configuration.
struct adc_cfg {
    std::uint8_t prescaler  = 4u;   ///< ADC clock prescaler: 2, 4, 6, or 8.
    std::uint8_t resolution = 12u;  ///< Conversion resolution: 6, 8, 10, or 12 bits.
    bool         continuous = false;///< Continuous conversion mode.
};

/**
 * @brief CIB component that initialises one STM32F4 ADC instance.
 *
 * @tparam adc_periph  ADC constexpr peripheral object (e.g. @c mcu::stm32::adc1).
 * @tparam Cfg         Compile-time @ref adc_cfg.
 *
 * @todo Implement groov writes to @c mcu::stm32::adcx (CR1 RES, CR2 ADON/CONT).
 */
template <auto& adc_periph, adc_cfg Cfg = adc_cfg{}>
struct adc_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: enable ADC clock via RCC APB2ENR
        // TODO: groov::sync_write<groov::blocking>(
        //     adc_periph(
        //         "cr1.res"_f    = resolution_bits(Cfg.resolution),
        //         "cr2.cont"_f   = Cfg.continuous ? groov::enable : groov::disable,
        //         "cr2.adon"_f   = groov::enable
        //     ));
    }
};

} // namespace hal::mcu::stm32f4
