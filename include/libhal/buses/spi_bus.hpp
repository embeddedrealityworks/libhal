#pragma once

#include <cstdint>
#include <cstddef>

#include <async/schedulers/trigger_scheduler.hpp>
#include <async/sequence.hpp>
#include <async/sync_wait.hpp>
#include <async/then.hpp>
#include <async/then_error.hpp>
#include <async/when_all.hpp>
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
// Software SPI command queue entry
// ---------------------------------------------------------------------------

/**
 * @brief A single queued SPI operation.
 */
struct spi_cmd {
    /// @brief Operation type.
    enum class kind : std::uint8_t { write, read, transfer };

    kind         op;        ///< Operation type.
    std::uint8_t reg_addr;  ///< Register/command byte sent first.
    std::uint8_t data;      ///< Write payload; filled with result for reads.
};

// ---------------------------------------------------------------------------
// SPI bus manager
// ---------------------------------------------------------------------------

/**
 * @brief STM32 SPI bus manager with CIB init, ISR hooks, and a command queue.
 *
 * Wraps a STM32 SPI peripheral (a @c constexpr groov::group object such as
 * @c mcu::stm32::spi1) and provides:
 *
 *  - A CIB @ref init component that registers hardware setup at boot.
 *  - Static @ref ev_isr() / @ref err_isr() stubs for interrupt vectors.
 *  - A software command queue backed by @c hal::ring_buffer.
 *  - A nested @ref device template — the groov-compatible bus adaptor
 *    parameterised by a chip-select pin reference.
 *
 * Chip-select management is the caller's responsibility.  Pass any GPIO
 * type that satisfies @ref hal::gpio_output as the @c cs_pin parameter.
 *
 * @par Register field names
 * Field names match the STM32 register definitions in the @c mcu::stm32
 * subproject (e.g. @c "cr1.spe", @c "sr.txe", @c "dr.data").
 *
 * @tparam spi_periph  STM32 SPI constexpr peripheral object
 *                     (e.g. @c mcu::stm32::spi1).
 * @tparam EvTrigger   Compile-time name of the SPI event trigger.
 * @tparam ErrTrigger  Compile-time name of the SPI error trigger.
 * @tparam QueueDepth  Capacity of the software command queue.
 *
 * @par Usage
 * @code{.cpp}
 * using bus1 = hal::spi_bus<mcu::stm32::spi1, "spi1_ev", "spi1_err">;
 *
 * cib::nexus<bus1::init, ...> nexus{};
 * extern "C" void SPI1_IRQHandler() { bus1::ev_isr(); }
 *
 * using my_dac = hal::devices::mcp4821_t<bus1::device<mcu::stm32::gpio_a, 4>>;
 * @endcode
 */
template <auto& spi_periph,
          stdx::ct_string EvTrigger,
          stdx::ct_string ErrTrigger,
          std::size_t QueueDepth = 16>
struct spi_bus {
private:
    using sr_read_t = decltype(groov::sync_read(spi_periph / "sr"_r));

    // Wait until SR.TXE or SR.RXNE is set.
    template <stdx::ct_string BitName>
    static auto wait_for_flag() {
        constexpr auto field = groov::path<"sr"_cts, BitName>{};
        return async::trigger_scheduler<EvTrigger, sr_read_t>{}.schedule()
               | async::repeat_until(
                     [field](auto r) { return r[field] == true; });
    }

    static auto wait_for_error() {
        return async::trigger_scheduler<ErrTrigger, sr_read_t>{}.schedule()
               | async::then_error([](auto e) { return e; });
    }

    static auto txe_or_error()  {
        return async::when_any(wait_for_flag<"txe">(),  wait_for_error()); }
    static auto rxne_or_error() {
        return async::when_any(wait_for_flag<"rxne">(), wait_for_error()); }

    static inline hal::ring_buffer<spi_cmd, QueueDepth> queue_{};

public:
    // -----------------------------------------------------------------------
    // CIB component
    // -----------------------------------------------------------------------

    /**
     * @brief CIB component that registers SPI hardware initialization.
     *
     * @note GPIO alternate-function and clock configuration must be done
     *       before this init runs via additional @c hal::startup extensions.
     */
    struct init {
        constexpr static auto config =
            cib::config(cib::extend<hal::startup>([]() { setup(); }));
    };

    // -----------------------------------------------------------------------
    // Hardware initialisation
    // -----------------------------------------------------------------------

    /**
     * @brief Initialise the SPI peripheral.
     *
     * Configures master mode, baud rate divider, CPOL/CPHA, data width,
     * and enables TXEIE / ERRIE interrupts before setting SPE=1.
     *
     * @pre  APBx clock must already be enabled for this SPI instance.
     * @pre  GPIO pins must already be in alternate-function mode.
     *
     * @todo Implement using groov writes to @c mcu::stm32::spix registers.
     */
    static void setup() noexcept {
        // TODO: groov::sync_write<groov::blocking>(
        //     spi_periph(
        //         "cr1.mstr"_f   = groov::enable,
        //         "cr1.br"_f     = 0b010u,   // f_pclk / 8
        //         "cr1.cpol"_f   = groov::disable,
        //         "cr1.cpha"_f   = groov::disable,
        //         "cr1.ssm"_f    = groov::enable,
        //         "cr1.ssi"_f    = groov::enable,
        //         "cr2.txeie"_f  = groov::enable,
        //         "cr2.errie"_f  = groov::enable,
        //         "cr1.spe"_f    = groov::enable
        //     ));
    }

    // -----------------------------------------------------------------------
    // ISR stubs
    // -----------------------------------------------------------------------

    /**
     * @brief SPI event interrupt handler body.
     * @par Typical wiring
     * @code{.cpp}
     * extern "C" void SPI1_IRQHandler() { bus1::ev_isr(); }
     * @endcode
     */
    static void ev_isr() {
        auto v = groov::sync_read(spi_periph / "sr"_r);
        async::run_triggers<EvTrigger>(v);
    }

    /**
     * @brief SPI error interrupt handler body.
     * @par Typical wiring
     * @code{.cpp}
     * extern "C" void SPI1_IRQHandler() { bus1::err_isr(); }  // shared vector on F4
     * @endcode
     */
    static void err_isr() {
        auto v = groov::sync_read(spi_periph / "sr"_r);
        async::run_triggers<ErrTrigger>(v);
    }

    // -----------------------------------------------------------------------
    // Software command queue
    // -----------------------------------------------------------------------

    /**
     * @brief Enqueue a single-byte register write.
     * @return @c true if queued; @c false if the queue is full.
     */
    static bool enqueue_write(std::uint8_t reg_addr,
                              std::uint8_t data) noexcept {
        return queue_.push({spi_cmd::kind::write, reg_addr, data});
    }

    /**
     * @brief Enqueue a single-byte register read.
     * @return @c true if queued; @c false if the queue is full.
     */
    static bool enqueue_read(std::uint8_t reg_addr) noexcept {
        return queue_.push({spi_cmd::kind::read, reg_addr, 0u});
    }

    /**
     * @brief Execute the next pending command.
     * Blocks via @c async::sync_wait until the transfer completes.
     */
    static void process() noexcept {
        auto cmd = queue_.pop();
        if (!cmd) return;
        // TODO: implement SPI transaction using txe_or_error() / rxne_or_error()
    }

    // -----------------------------------------------------------------------
    // groov-compatible bus for a specific chip-select
    // -----------------------------------------------------------------------

    /**
     * @brief groov bus adaptor for one SPI device identified by its CS pin.
     *
     * The @c cs_pin is asserted (low) before the transaction and deasserted
     * after.  The CS pin must satisfy @ref hal::gpio_output.
     *
     * @tparam cs_port  GPIO port constexpr object (e.g. @c mcu::stm32::gpio_a).
     * @tparam cs_pin_n Pin number within the port (0–15).
     *
     * @par Usage
     * @code{.cpp}
     * using my_dac = hal::devices::mcp4821_t<bus1::device<mcu::stm32::gpio_a, 4>>;
     * @endcode
     *
     * @todo Implement groov bus write/read with CS assert/deassert and
     *       TXE/RXNE wait chains.
     */
    template <auto& cs_port, std::uint8_t cs_pin_n>
    struct device {

        /**
         * @brief Write a groov register field value over SPI.
         *
         * Transaction: @c CS↓ → @c [reg_addr] → @c [data byte(s)] → @c CS↑
         *
         * @todo Implement.
         */
        template <stdx::ct_string, auto Mask, auto IdMask, auto IdValue>
        static auto write(auto addr, decltype(Mask) data) -> async::sender auto;

        /**
         * @brief Read a groov register value over SPI.
         *
         * Transaction: @c CS↓ → @c [reg_addr | 0x80] → @c [sizeof(T) bytes] → @c CS↑
         *
         * @todo Implement.
         */
        template <stdx::ct_string, auto Mask>
        static auto read(auto addr) -> async::sender auto;
    };
};

} // namespace hal
