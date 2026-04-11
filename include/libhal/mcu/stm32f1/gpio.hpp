#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/gpio.hpp"

/// @file gpio.hpp
/// @brief CIB component for STM32F1 GPIO alternate-function configuration.
///
/// @note STM32F1 uses a different GPIO register layout from F4/F7/H7.
///       Pins are configured via CRL (pins 0–7) and CRH (pins 8–15).
///       There is no MODER or AFR register — alternate function is implied
///       by MODE/CNF bits and the AFIO remapping registers.

namespace hal::mcu::stm32f1 {

/// @brief Descriptor for a single GPIO pin alternate-function assignment.
struct af_pin {
    char          port;     ///< GPIO port letter ('A'–'G').
    std::uint8_t  pin;      ///< Pin number (0–15).
    std::uint8_t  af_remap; ///< AFIO remap value (0 = default, device-specific).
    bool          open_drain = false; ///< Use open-drain output mode.
};

/**
 * @brief CIB component that configures STM32F1 GPIO pins for alternate use.
 *
 * @note STM32F1 pin mode is set in CRx.CNF/MODE bits.  Remapping is done via
 *       AFIO MAPR / MAPR2 registers, not AFR like later STM32 families.
 *
 * @tparam Pins  One or more @ref af_pin descriptors.
 * @todo Implement groov writes to @c mcu::stm32::gpio_x and @c mcu::stm32::afio.
 */
template <af_pin... Pins>
struct gpio_af_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: enable port/AFIO clocks, set CRL/CRH MODE=0b11 CNF=0b10,
        //       apply AFIO MAPR remap bits
        ((void)Pins, ...);
    }
};

} // namespace hal::mcu::stm32f1
