#pragma once

#include <cstdint>

#include <groov/groov.hpp>
#include <cib/cib.hpp>

#include "../../startup.hpp"
#include "../../peripherals/can.hpp"

/// @file can.hpp
/// @brief STM32F4 bxCAN CIB component and bit-timing helpers.
///
/// The STM32F4 bxCAN peripheral supports CAN 2.0A/B at up to 1 Mbit/s.
/// Bit timing is configured via BTR: BRP (baud rate prescaler), TS1, TS2
/// (time segment quanta), and SJW (re-synchronisation jump width).
///
/// @par Bit-timing formula
/// @code
/// t_q          = t_pclk × (BRP + 1)
/// bit_time     = t_q × (1 + TS1 + 1 + TS2 + 1)
/// baud_rate_Hz = 1 / bit_time
/// @endcode
///
/// @par Usage
/// @code{.cpp}
/// using can1 = hal::mcu::stm32f4::can_init<mcu::stm32::can1,
///                                           hal::mcu::stm32f4::can_cfg{
///                                               .apb_hz  = 42'000'000,
///                                               .baud_hz = 500'000,
///                                           }>;
/// cib::nexus<can1, ...> nexus{};
/// extern "C" void CAN1_RX0_IRQHandler() { can1::rx_isr(); }
/// extern "C" void CAN1_TX_IRQHandler()  { can1::tx_isr(); }
/// @endcode

namespace hal::mcu::stm32f4 {

/// @brief Compile-time CAN bus configuration.
struct can_cfg {
    std::uint32_t apb_hz  = 42'000'000u;  ///< APB1 clock in Hz.
    std::uint32_t baud_hz = 500'000u;     ///< Target CAN baud rate in bps.
    std::uint8_t  ts1     = 13u;          ///< Time segment 1 quanta (1–16).
    std::uint8_t  ts2     =  2u;          ///< Time segment 2 quanta (1–8).
    std::uint8_t  sjw     =  1u;          ///< Sync jump width quanta (1–4).
    bool          loopback = false;       ///< Enable loopback mode for testing.
    bool          silent   = false;       ///< Enable silent (listen-only) mode.
};

/// @brief Compute BTR.BRP from @p cfg.
/// BRP = apb_hz / (baud_hz × (1 + ts1 + ts2)) − 1
constexpr std::uint32_t can_brp(const can_cfg& cfg) noexcept {
    return cfg.apb_hz / (cfg.baud_hz * (1u + cfg.ts1 + cfg.ts2)) - 1u;
}

/**
 * @brief CIB component that initialises one STM32F4 bxCAN instance.
 *
 * @tparam can_periph  CAN constexpr peripheral object (e.g. @c mcu::stm32::can1).
 * @tparam Cfg         Compile-time @ref can_cfg.
 *
 * @todo Implement groov writes to @c mcu::stm32::canx (MCR, BTR, IER).
 */
template <auto& can_periph, can_cfg Cfg = can_cfg{}>
struct can_init {
    constexpr static auto config =
        cib::config(cib::extend<hal::startup>([]() { setup(); }));

    static void setup() noexcept {
        // TODO: enable CAN clock via RCC APB1ENR
        // TODO: request initialisation mode (MCR.INRQ = 1, wait MSR.INAK)
        // TODO: groov::sync_write<groov::blocking>(
        //     can_periph(
        //         "btr.brp"_f  = can_brp(Cfg),
        //         "btr.ts1"_f  = Cfg.ts1 - 1u,
        //         "btr.ts2"_f  = Cfg.ts2 - 1u,
        //         "btr.sjw"_f  = Cfg.sjw - 1u,
        //         "btr.lbkm"_f = Cfg.loopback ? groov::enable : groov::disable,
        //         "btr.silm"_f = Cfg.silent   ? groov::enable : groov::disable
        //     ));
        // TODO: leave initialisation mode (MCR.INRQ = 0)
    }

    /// @brief CAN RX FIFO 0 interrupt handler body.
    static void rx_isr() noexcept {
        // TODO: pop frame from RF0R, notify application
    }

    /// @brief CAN TX mailbox empty interrupt handler body.
    static void tx_isr() noexcept {
        // TODO: clear RQCP bits, notify application
    }
};

} // namespace hal::mcu::stm32f4
