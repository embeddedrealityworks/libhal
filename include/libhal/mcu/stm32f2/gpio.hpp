#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/gpio.hpp"

/// @file gpio.hpp
/// @brief CIB component for STM32F2 GPIO alternate-function configuration.
///
/// @note STM32F2 GPIO register layout (MODER/OTYPER/OSPEEDR/AFR) is
///       identical to STM32F4.  AF numbers differ — consult the F2 datasheet.

namespace hal::mcu::stm32f2 {

struct af_pin {
    char          port;
    std::uint8_t  pin;
    std::uint8_t  af;
    bool          open_drain = false;
};

/**
 * @brief CIB component that configures STM32F2 GPIO pins for alternate use.
 * @tparam Pins  One or more @ref af_pin descriptors.
 * @todo Implement (same register layout as STM32F4).
 */
template <af_pin... Pins>
struct gpio_af_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept { ((void)Pins, ...); }
};

} // namespace hal::mcu::stm32f2
