#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/gpio.hpp"

/// @file gpio.hpp
/// @brief CIB component and helpers for STM32F4 GPIO alternate-function
///        configuration.
///
/// Each GPIO pin used by a peripheral (I2C, SPI, USART, …) must be
/// switched to its alternate-function mode before the peripheral is
/// enabled.  The @ref hal::mcu::stm32f4::gpio_af_init component handles
/// this within the @ref hal::startup callback chain.
///
/// @par Intended usage
/// @code{.cpp}
/// // I2C1 pins: PB6 (SCL, AF4) and PB7 (SDA, AF4)
/// using i2c1_pins = hal::mcu::stm32f4::gpio_af_init<
///     hal::mcu::stm32f4::af_pin{ .port = 'B', .pin = 6, .af = 4 },
///     hal::mcu::stm32f4::af_pin{ .port = 'B', .pin = 7, .af = 4 }>;
///
/// cib::nexus<i2c1_pins, bus1::init, ...> nexus{};
/// @endcode

namespace hal::mcu::stm32f4 {

/// @brief Descriptor for a single GPIO pin alternate-function assignment.
struct af_pin {
    char          port;  ///< GPIO port letter ('A'–'K').
    std::uint8_t  pin;   ///< Pin number within the port (0–15).
    std::uint8_t  af;    ///< Alternate-function number (0–15, see DS table).
    hal::output_t dir  = hal::output{};  ///< Pin direction (output by default for AF).
    hal::open_drain_t drive = hal::open_drain{};  ///< Drive mode (open-drain for I2C).
};

/**
 * @brief CIB component that configures one or more GPIO pins for alternate
 *        function use.
 *
 * Extend @ref hal::startup so that GPIO configuration runs before peripheral
 * bus managers initialise.  List pins as non-type template parameters.
 *
 * @tparam Pins  One or more @ref af_pin descriptors.
 *
 * @todo Implement using groov writes to the appropriate
 *       @c mcu::stm32::gpio_x registers (MODER, OTYPER, OSPEEDR, AFR).
 */
template <af_pin... Pins>
struct gpio_af_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    /// @brief Apply the alternate-function configuration for all @p Pins.
    ///
    /// Sequence per pin:
    ///  1. Enable the GPIO port clock via RCC AHB1ENR.
    ///  2. Set MODER to alternate-function (0b10).
    ///  3. Set OTYPER to push-pull (0) or open-drain (1).
    ///  4. Set OSPEEDR to high speed.
    ///  5. Write the AF number to AFR[L|H].
    ///
    /// @todo Implement using groov writes to @c mcu::stm32::gpio_x.
    static void setup() noexcept {
        // TODO: for each pin in Pins:
        //   groov::sync_write<groov::blocking>(
        //       mcu::stm32::gpio_b("moder.moder6"_f = 0b10u, ...));
        ((void)Pins, ...);  // suppress unused-parameter warnings until impl
    }
};

} // namespace hal::mcu::stm32f4
