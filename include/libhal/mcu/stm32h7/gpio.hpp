#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/gpio.hpp"

/// @file gpio.hpp
/// @brief CIB component for STM32H7 GPIO alternate-function configuration.
///
/// @note STM32H7 GPIO register layout (MODER/OTYPER/OSPEEDR/AFR) is
///       identical to STM32F4.  The H7 adds a SECCFGR security register
///       which can be ignored for non-TrustZone applications.
///       AF numbers differ — consult the H7 datasheet.

namespace hal::mcu::stm32h7 {

struct af_pin {
    char          port;
    std::uint8_t  pin;
    std::uint8_t  af;
    bool          open_drain  = false;
    std::uint8_t  speed       = 2u;  ///< OSPEEDR: 0=low, 1=medium, 2=high, 3=very-high.
};

/**
 * @brief CIB component that configures STM32H7 GPIO pins for alternate use.
 * @tparam Pins  One or more @ref af_pin descriptors.
 * @todo Implement (same register layout as STM32F4; note AHB4 clock for GPIO).
 */
template <af_pin... Pins>
struct gpio_af_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept { ((void)Pins, ...); }
};

} // namespace hal::mcu::stm32h7
