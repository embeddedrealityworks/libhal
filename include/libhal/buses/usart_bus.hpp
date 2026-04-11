#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

#include <async/schedulers/trigger_scheduler.hpp>
#include <async/sequence.hpp>
#include <async/sync_wait.hpp>
#include <async/then.hpp>
#include <async/when_any.hpp>

#include <groov/groov.hpp>

#include <stdx/ct_string.hpp>

#include <cib/cib.hpp>

#include "../startup.hpp"
#include "../utils/ringbuffer.hpp"

namespace hal {

using namespace groov::literals;
using stdx::literals::operator""_cts;

// ---------------------------------------------------------------------------
// USART bus manager
// ---------------------------------------------------------------------------

/**
 * @brief STM32 USART bus manager with CIB init, ISR hooks, and TX/RX queues.
 *
 * Unlike I2C and SPI, USART is a byte-stream interface rather than a
 * register-mapped bus.  This manager does not implement the groov bus
 * contract; instead it provides a direct @c write() / @c read() / @c available()
 * API that satisfies the @ref hal::usart_peripheral concept.
 *
 * @par Architecture
 * - TX queue: bytes written via @ref write() are pushed into a @c ring_buffer
 *   and drained in the TXEIE ISR one byte at a time.
 * - RX queue: each byte arriving in the RXNE ISR is pushed into an RX
 *   @c ring_buffer for consumption by the main loop via @ref read().
 *
 * @tparam usart_periph  STM32 USART constexpr peripheral object
 *                       (e.g. @c mcu::stm32::usart1).
 * @tparam TxTrigger     Compile-time trigger name for the TX-empty event.
 * @tparam RxTrigger     Compile-time trigger name for the RX-not-empty event.
 * @tparam TxDepth       TX ring buffer capacity in bytes.
 * @tparam RxDepth       RX ring buffer capacity in bytes.
 *
 * @par Usage
 * @code{.cpp}
 * using uart1 = hal::usart_bus<mcu::stm32::usart1, "uart1_tx", "uart1_rx">;
 *
 * cib::nexus<uart1::init, ...> nexus{};
 * extern "C" void USART1_IRQHandler() { uart1::irq(); }
 *
 * uart1::write(std::as_bytes(std::span{"hello\n"}));
 * @endcode
 */
template <auto& usart_periph,
          stdx::ct_string TxTrigger,
          stdx::ct_string RxTrigger,
          std::size_t TxDepth = 64,
          std::size_t RxDepth = 64>
struct usart_bus {
private:
    static inline hal::ring_buffer<std::uint8_t, TxDepth> tx_queue_{};
    static inline hal::ring_buffer<std::uint8_t, RxDepth> rx_queue_{};

public:
    // -----------------------------------------------------------------------
    // CIB component
    // -----------------------------------------------------------------------

    /**
     * @brief CIB component that registers USART hardware initialization.
     */
    struct init {
        constexpr static auto config =
            cib::config(cib::extend<hal::startup>([]() { setup(); }));
    };

    // -----------------------------------------------------------------------
    // Hardware initialisation
    // -----------------------------------------------------------------------

    /**
     * @brief Initialise the USART peripheral.
     *
     * Configures baud rate, data bits, stop bits, parity, and enables
     * TXEIE and RXNEIE interrupts before setting UE=1.
     *
     * @pre  APBx clock must already be enabled for this USART instance.
     * @pre  GPIO TX/RX pins must already be in alternate-function mode.
     *
     * @todo Implement using groov writes to @c mcu::stm32::usartx registers.
     */
    static void setup() noexcept {
        // TODO: groov::sync_write<groov::blocking>(
        //     usart_periph(
        //         "brr.div"_f      = compute_brr(apb_hz, baud),
        //         "cr1.m"_f        = groov::disable,   // 8-bit
        //         "cr1.pce"_f      = groov::disable,   // no parity
        //         "cr2.stop"_f     = 0b00u,            // 1 stop bit
        //         "cr1.txeie"_f    = groov::enable,
        //         "cr1.rxneie"_f   = groov::enable,
        //         "cr1.te"_f       = groov::enable,
        //         "cr1.re"_f       = groov::enable,
        //         "cr1.ue"_f       = groov::enable
        //     ));
    }

    // -----------------------------------------------------------------------
    // ISR stub (TX and RX share one vector on most STM32 devices)
    // -----------------------------------------------------------------------

    /**
     * @brief USART interrupt handler body.
     *
     * Handles both TXEIE (drain TX queue) and RXNE (fill RX queue).
     *
     * @par Typical wiring
     * @code{.cpp}
     * extern "C" void USART1_IRQHandler() { uart1::irq(); }
     * @endcode
     *
     * @todo Implement TXEIE / RXNE flag handling via groov reads.
     */
    static void irq() noexcept {
        // TODO: auto sr = groov::sync_read(usart_periph / "sr"_r);
        // if (sr["sr.rxne"_f]) rx_queue_.push(groov::sync_read(usart_periph / "dr.data"_f));
        // if (sr["sr.txe"_f])  { auto b = tx_queue_.pop(); if (b) write_dr(*b); else disable_txeie(); }
    }

    // -----------------------------------------------------------------------
    // Public stream API (satisfies hal::usart_peripheral concept)
    // -----------------------------------------------------------------------

    /**
     * @brief Queue bytes for transmission (non-blocking).
     *
     * Bytes are pushed into the TX ring buffer and sent in the TXEIE ISR.
     * Returns @c false if any byte could not be queued (TX buffer full).
     *
     * @todo Re-enable TXEIE after pushing.
     */
    static bool write(std::span<const std::byte> data) noexcept {
        for (auto b : data) {
            if (!tx_queue_.push(static_cast<std::uint8_t>(b)))
                return false;
        }
        // TODO: groov::sync_write(usart_periph("cr1.txeie"_f = groov::enable));
        return true;
    }

    /**
     * @brief Read up to @p buf.size() bytes from the RX queue.
     * @return Number of bytes actually read.
     */
    static unsigned read(std::span<std::byte> buf) noexcept {
        unsigned n = 0;
        for (auto& dest : buf) {
            auto b = rx_queue_.pop();
            if (!b) break;
            dest = static_cast<std::byte>(*b);
            ++n;
        }
        return n;
    }

    /**
     * @brief Number of bytes available in the RX queue.
     */
    static unsigned available() noexcept {
        return static_cast<unsigned>(rx_queue_.size());
    }
};

} // namespace hal
