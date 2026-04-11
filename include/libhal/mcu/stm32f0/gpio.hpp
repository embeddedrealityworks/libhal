#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/gpio.hpp"

/// @file gpio.hpp
/// @brief CIB component for STM32F0 GPIO alternate-function configuration.
///
/// @note STM32F0 GPIO is register-compatible with STM32F4 (MODER / OTYPER /
///       OSPEEDR / AFR layout is identical).  AF numbers differ — consult the
///       device-specific datasheet alternate-function table.

namespace hal::mcu::stm32f0 {

/// @brief Descriptor for a single GPIO pin alternate-function assignment.
struct af_pin {
    char          port;  ///< GPIO port letter ('A'–'F').
    std::uint8_t  pin;   ///< Pin number (0–15).
    std::uint8_t  af;    ///< Alternate-function number (0–7 on F0).
};

/**
 * @brief CIB component that configures STM32F0 GPIO pins for alternate use.
 * @tparam Pins  One or more @ref af_pin descriptors.
 * @todo Implement groov writes to @c mcu::stm32::gpio_x.
 */
template <af_pin... Pins>
struct gpio_af_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: enable port clocks, set MODER/OTYPER/OSPEEDR/AFR
        ((void)Pins, ...);
    }
};

} // namespace hal::mcu::stm32f0
