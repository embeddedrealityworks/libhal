#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/dac.hpp"

/// @file dac.hpp
/// @brief STM32F4 DAC CIB component.
///
/// The STM32F4 DAC has two 12-bit channels (DAC_OUT1 on PA4, DAC_OUT2 on PA5).
/// Each channel can operate in 8-bit or 12-bit mode, with optional output
/// buffer and hardware trigger support.
///
/// @par Usage
/// @code{.cpp}
/// using dac1 = hal::mcu::stm32f4::dac_init<mcu::stm32::dac,
///                                           hal::mcu::stm32f4::dac_cfg{
///                                               .channel  = 1,
///                                               .buffered = true,
///                                           }>;
/// cib::nexus<dac1, ...> nexus{};
/// @endcode

namespace hal::mcu::stm32f4 {

/// @brief Compile-time DAC channel configuration.
struct dac_cfg {
    std::uint8_t channel  = 1u;    ///< DAC channel: 1 (PA4) or 2 (PA5).
    bool         buffered = true;  ///< Enable output buffer (reduces drive impedance).
    bool         trigger  = false; ///< Enable hardware trigger (TRGx) for synchronized output.
};

/**
 * @brief CIB component that initialises one STM32F4 DAC channel.
 *
 * @tparam dac_periph  DAC constexpr peripheral object (@c mcu::stm32::dac).
 * @tparam Cfg         Compile-time @ref dac_cfg.
 *
 * @todo Implement groov writes to @c mcu::stm32::dac (CR, DHR12Rx).
 */
template <auto& dac_periph, dac_cfg Cfg = dac_cfg{}>
struct dac_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: enable DAC clock via RCC APB1ENR
        // TODO: groov::sync_write<groov::blocking>(
        //     dac_periph(
        //         channel_boff(Cfg.channel) = !Cfg.buffered ? groov::enable : groov::disable,
        //         channel_ten(Cfg.channel)  = Cfg.trigger   ? groov::enable : groov::disable,
        //         channel_en(Cfg.channel)   = groov::enable
        //     ));
    }
};

} // namespace hal::mcu::stm32f4
