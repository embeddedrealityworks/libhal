#pragma once

#include <cstdint>

#include <async/repeat_until.hpp>
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
// Software I2C command queue entry
// ---------------------------------------------------------------------------

/**
 * @brief A single queued I2C operation.
 *
 * Used by @c i2c_bus::enqueue_write() / @c enqueue_read() and consumed
 * by @c i2c_bus::process().
 */
struct i2c_cmd {
    /// @brief Operation type.
    enum class kind : std::uint8_t { write, read };

    kind         op;           ///< Whether this is a read or write.
    std::uint8_t device_addr;  ///< 7-bit I2C device address.
    std::uint8_t reg_addr;     ///< Device register command byte.
    std::uint8_t data;         ///< Write payload; filled with result for reads.
};

// ---------------------------------------------------------------------------
// I2C bus manager
// ---------------------------------------------------------------------------

/**
 * @brief STM32 I2C bus manager with CIB init, ISR hooks, and a command queue.
 *
 * Wraps a STM32 I2C peripheral (a @c constexpr groov::group object such as
 * @c mcu::stm32::i2c1) and provides:
 *
 *  - A CIB @ref init component that registers hardware setup to run at boot.
 *  - Static @ref ev_isr() / @ref err_isr() stubs for the interrupt vectors.
 *  - A software command queue backed by @c hal::ring_buffer.
 *  - A nested @ref device template — the groov-compatible bus adaptor used
 *    by device drivers like @c hal::devices::pca9555_t.
 *
 * @par Async helpers
 * The internal @c write_byte / @c write_last_byte helpers follow the pattern
 * from the [stm32-gizmo](https://github.com/caisselabs/stm32-gizmo) reference
 * implementation.  Operations are sequenced via @c async::when_all and
 * @c async::seq, driven by @c async::trigger_scheduler events fired from the
 * I2C interrupt service routines.
 *
 * @par Register field names
 * Field names (@c "cr1.pe", @c "cr2.sadd7", etc.) are lowercase and match the
 * STM32 register definitions in the @c mcu::stm32 subproject.
 *
 * @tparam i2c_periph  The STM32 I2C constexpr peripheral object
 *                     (e.g. @c mcu::stm32::i2c1).
 * @tparam EvTrigger   Compile-time name of the I2C event trigger used with
 *                     @c async::trigger_scheduler (e.g. @c "i2c1_ev").
 *                     Must match the name passed to @c async::run_triggers<>
 *                     inside @ref ev_isr().
 * @tparam ErrTrigger  Compile-time name of the I2C error trigger
 *                     (e.g. @c "i2c1_err").
 * @tparam QueueDepth  Capacity of the software command queue.
 *
 * @par Usage
 * @code{.cpp}
 * // 1. Declare the bus type for I2C1
 * using bus1 = hal::i2c_bus<mcu::stm32::i2c1, "i2c1_ev", "i2c1_err">;
 *
 * // 2. Build a nexus that includes bus1::init
 * cib::nexus<bus1::init, ...> nexus{};
 *
 * // 3. Wire up interrupt vectors
 * extern "C" void I2C1_EV_IRQHandler()  { bus1::ev_isr();  }
 * extern "C" void I2C1_ERR_IRQHandler() { bus1::err_isr(); }
 *
 * // 4. Run init
 * nexus.init();
 * cib::service<hal::startup>();
 *
 * // 5. Attach a device driver
 * using my_pca = hal::devices::pca9555_t<bus1::device<0x24>>;
 * constexpr auto ioexpander = my_pca{};
 * @endcode
 */
template <auto& i2c_periph,
          stdx::ct_string EvTrigger,
          stdx::ct_string ErrTrigger,
          std::size_t QueueDepth = 16>
struct i2c_bus {
private:
    // Cached type of a sync ISR-register read (used by trigger_scheduler).
    using isr_read_t = decltype(groov::sync_read(i2c_periph / "isr"_r));

    // -----------------------------------------------------------------------
    // Async helpers (stm32-gizmo pattern)
    // -----------------------------------------------------------------------

    // Wait until the named ISR bit is true.  Driven by the event trigger ISR.
    template <stdx::ct_string BitName>
    static auto wait_for_flag() {
        constexpr auto field = groov::path<"isr"_cts, BitName>{};
        return async::trigger_scheduler<EvTrigger, isr_read_t>{}.schedule()
               | async::repeat_until(
                     [field](auto r) { return r[field] == true; });
    }

    // Wait for any error bit; propagates the ISR value as an error.
    static auto wait_for_error() {
        return async::trigger_scheduler<ErrTrigger, isr_read_t>{}.schedule()
               | async::then_error([](auto e) { return e; });
    }

    // Race the named flag against an I2C error.
    static auto txis_or_error()  {
        return async::when_any(wait_for_flag<"txis">(),  wait_for_error()); }
    static auto stopf_or_error() {
        return async::when_any(wait_for_flag<"stopf">(), wait_for_error()); }
    static auto rxne_or_error()  {
        return async::when_any(wait_for_flag<"rxne">(),  wait_for_error()); }
    static auto tc_or_error()    {
        return async::when_any(wait_for_flag<"tc">(),    wait_for_error()); }

    // Write a non-final byte: start TXIS wait and TXDR write concurrently.
    static auto write_byte(std::uint8_t b) {
        return async::when_all(
            txis_or_error(),
            groov::write(i2c_periph("txdr.txdata"_f = b)));
    }

    // Write the final byte: wait for STOPF (AUTOEND), then clear the flag.
    static auto write_last_byte(std::uint8_t b) {
        return async::when_all(
                   stopf_or_error(),
                   groov::write(i2c_periph("txdr.txdata"_f = b)))
               | async::seq(
                   groov::write(i2c_periph("icr.stopcf"_f = groov::set)));
    }

    // Internal command queue.
    static inline hal::ring_buffer<i2c_cmd, QueueDepth> queue_{};

public:
    // -----------------------------------------------------------------------
    // CIB component
    // -----------------------------------------------------------------------

    /**
     * @brief CIB component that registers I2C hardware initialization.
     *
     * Include in your @c cib::nexus component list so that @ref setup() is
     * called automatically when @c cib::service<hal::startup>() is invoked.
     *
     * @note Clock source selection, TIMINGR, and GPIO alternate-function
     *       configuration are board-specific.  Perform those steps before
     *       this init runs, or call them via additional @c hal::startup
     *       extensions registered earlier in the nexus.
     */
    struct init {
        constexpr static auto config =
            cib::config(cib::extend<hal::startup>([]() { setup(); }));
    };

    // -----------------------------------------------------------------------
    // Hardware initialisation
    // -----------------------------------------------------------------------

    /**
     * @brief Initialise the I2C peripheral.
     *
     * Executes the STM32 reset sequence (PE=0 → poll PE=0 → PE=1), then
     * enables the TXIE, STOPIE, and ERRIE interrupt sources.
     *
     * Called automatically by the @ref init CIB component.  May also be
     * invoked directly when CIB is not used.
     *
     * @pre  Clock source and TIMINGR must already be configured.
     * @pre  GPIO pins must already be routed to the I2C alternate function.
     */
    static void setup() {
        // Per STM32 RM: disable PE, poll until clear, re-enable.
        groov::write(i2c_periph("cr1.pe"_f = groov::disable))
            | async::seq(groov::read(i2c_periph / "cr1.pe"_f))
            | async::seq(groov::write(i2c_periph("cr1.pe"_f = groov::enable)))
            | async::seq(groov::read(i2c_periph / "cr1.pe"_f))
            | async::sync_wait();

        groov::sync_write<groov::blocking>(
            i2c_periph(
                "cr1.txie"_f   = groov::enable,
                "cr1.stopie"_f = groov::enable,
                "cr1.errie"_f  = groov::enable,
                "cr1.pe"_f     = groov::enable
            ));
    }

    // -----------------------------------------------------------------------
    // ISR stubs
    // -----------------------------------------------------------------------

    /**
     * @brief I2C event interrupt handler body.
     *
     * Reads ISR and fires the @p EvTrigger so that any suspended
     * @c wait_for_flag<> sender can observe the new flag state.
     *
     * @par Typical wiring
     * @code{.cpp}
     * extern "C" void I2C1_EV_IRQHandler() { bus1::ev_isr(); }
     * @endcode
     */
    static void ev_isr() {
        auto v = groov::sync_read(i2c_periph / "isr"_r);
        async::run_triggers<EvTrigger>(v);
    }

    /**
     * @brief I2C error interrupt handler body.
     *
     * Reads ISR and fires the @p ErrTrigger, unblocking any
     * @c wait_for_error() sender in the current async chain.
     *
     * @par Typical wiring
     * @code{.cpp}
     * extern "C" void I2C1_ERR_IRQHandler() { bus1::err_isr(); }
     * @endcode
     */
    static void err_isr() {
        auto v = groov::sync_read(i2c_periph / "isr"_r);
        async::run_triggers<ErrTrigger>(v);
    }

    // -----------------------------------------------------------------------
    // Software command queue
    // -----------------------------------------------------------------------

    /**
     * @brief Enqueue a single-byte register write.
     *
     * @param device_addr  7-bit I2C device address.
     * @param reg_addr     Register command byte.
     * @param data         Byte to write.
     * @return @c true if queued; @c false if the queue is full.
     */
    static bool enqueue_write(std::uint8_t device_addr,
                              std::uint8_t reg_addr,
                              std::uint8_t data) noexcept {
        return queue_.push(
            {i2c_cmd::kind::write, device_addr, reg_addr, data});
    }

    /**
     * @brief Enqueue a single-byte register read.
     *
     * @param device_addr  7-bit I2C device address.
     * @param reg_addr     Register command byte.
     * @return @c true if queued; @c false if the queue is full.
     */
    static bool enqueue_read(std::uint8_t device_addr,
                             std::uint8_t reg_addr) noexcept {
        return queue_.push(
            {i2c_cmd::kind::read, device_addr, reg_addr, 0u});
    }

    /**
     * @brief Execute the next pending command from the software queue.
     *
     * Call from the main loop to drain queued transactions one at a time.
     * Each call blocks (via @c async::sync_wait) until the transfer completes
     * or an error occurs.  Does nothing if the queue is empty.
     */
    static void process() {
        auto cmd = queue_.pop();
        if (!cmd) return;

        if (cmd->op == i2c_cmd::kind::write) {
            auto ctrl = i2c_periph(
                "cr2.add10"_f   = groov::disable,
                "cr2.sadd7"_f   = cmd->device_addr,
                "cr2.rd_wrn"_f  = false,
                "cr2.nbytes"_f  = std::uint8_t{2},
                "cr2.reload"_f  = groov::disable,
                "cr2.autoend"_f = groov::enable,
                "cr2.start"_f   = true
            );
            async::when_all(txis_or_error(), groov::write(ctrl))
                | async::seq(write_byte(cmd->reg_addr))
                | async::seq(write_last_byte(cmd->data))
                | async::sync_wait();
        } else {
            auto write_ctrl = i2c_periph(
                "cr2.add10"_f   = groov::disable,
                "cr2.sadd7"_f   = cmd->device_addr,
                "cr2.rd_wrn"_f  = false,
                "cr2.nbytes"_f  = std::uint8_t{1},
                "cr2.reload"_f  = groov::disable,
                "cr2.autoend"_f = groov::disable,
                "cr2.start"_f   = true
            );
            auto read_ctrl = i2c_periph(
                "cr2.sadd7"_f   = cmd->device_addr,
                "cr2.rd_wrn"_f  = true,
                "cr2.nbytes"_f  = std::uint8_t{1},
                "cr2.reload"_f  = groov::disable,
                "cr2.autoend"_f = groov::enable,
                "cr2.start"_f   = true
            );
            auto& result = cmd->data;
            async::when_all(txis_or_error(), groov::write(write_ctrl))
                | async::seq(async::when_all(
                      tc_or_error(),
                      groov::write(i2c_periph("txdr.txdata"_f = cmd->reg_addr))
                  ))
                | async::seq(groov::write(read_ctrl))
                | async::seq(
                      rxne_or_error()
                      | async::then([&result](auto) {
                            result = static_cast<std::uint8_t>(
                                groov::sync_read<groov::blocking>(
                                    i2c_periph / "rxdr.rxdata"_f));
                        })
                  )
                | async::sync_wait();
        }
    }

    // -----------------------------------------------------------------------
    // groov-compatible bus for a specific device address
    // -----------------------------------------------------------------------

    /**
     * @brief groov bus adaptor for one I2C device on this bus.
     *
     * Pass this as the @c Bus template argument of a @c groov::group (e.g.
     * @c hal::devices::pca9555_t).  The @c write() and @c read() static
     * methods implement the groov bus contract and produce @c async::sender
     * chains driven by the I2C event and error trigger schedulers.
     *
     * @tparam device_addr  7-bit I2C address of the target device.
     *
     * @par Usage
     * @code{.cpp}
     * using bus1   = hal::i2c_bus<mcu::stm32::i2c1, "i2c1_ev", "i2c1_err">;
     * using my_pca = hal::devices::pca9555_t<bus1::device<0x24>>;
     * @endcode
     */
    template <std::uint8_t device_addr>
    struct device {

        /**
         * @brief Write a groov register field value over I2C.
         *
         * Transaction: @c START → @c [device_addr|W] → @c [reg_addr]
         *              → @c [data byte(s)] → @c STOP
         *
         * @return An @c async::sender completing on STOPF or propagating an
         *         error on any I2C error flag.
         */
        template <stdx::ct_string, auto Mask, auto IdMask, auto IdValue>
        static auto write(auto addr, decltype(Mask) data) -> async::sender auto {
            using T = decltype(Mask);
            data |= IdValue;

            constexpr auto nbytes = std::uint8_t(sizeof(T) + 1u);
            auto ctrl = i2c_periph(
                "cr2.add10"_f   = groov::disable,
                "cr2.sadd7"_f   = device_addr,
                "cr2.rd_wrn"_f  = false,
                "cr2.nbytes"_f  = nbytes,
                "cr2.reload"_f  = groov::disable,
                "cr2.autoend"_f = groov::enable,
                "cr2.start"_f   = true
            );

            auto setup     = async::when_all(txis_or_error(), groov::write(ctrl));
            auto addr_byte = static_cast<std::uint8_t>(addr);

            if constexpr (sizeof(T) == 1) {
                return setup
                       | async::seq(write_byte(addr_byte))
                       | async::seq(write_last_byte(static_cast<std::uint8_t>(data)));

            } else if constexpr (sizeof(T) == 2) {
                auto bytes = stdx::bit_unpack<std::uint8_t>(data);
                return setup
                       | async::seq(write_byte(addr_byte))
                       | async::seq(write_byte(std::get<0>(bytes)))
                       | async::seq(write_last_byte(std::get<1>(bytes)));

            } else if constexpr (sizeof(T) == 4) {
                auto bytes = stdx::bit_unpack<std::uint8_t>(data);
                return setup
                       | async::seq(write_byte(addr_byte))
                       | async::seq(write_byte(std::get<0>(bytes)))
                       | async::seq(write_byte(std::get<1>(bytes)))
                       | async::seq(write_byte(std::get<2>(bytes)))
                       | async::seq(write_last_byte(std::get<3>(bytes)));
            }
        }

        /**
         * @brief Read a groov register value over I2C.
         *
         * Two-phase transaction:
         *  1. @c START → @c [device_addr|W] → @c [reg_addr] → @c TC
         *  2. @c RESTART → @c [device_addr|R] → @c [sizeof(T) bytes] → @c STOP
         *
         * @return An @c async::sender producing the register value of type @c T.
         */
        template <stdx::ct_string, auto Mask>
        static auto read(auto addr) -> async::sender auto {
            using T = decltype(Mask);

            auto write_ctrl = i2c_periph(
                "cr2.add10"_f   = groov::disable,
                "cr2.sadd7"_f   = device_addr,
                "cr2.rd_wrn"_f  = false,
                "cr2.nbytes"_f  = std::uint8_t{1},
                "cr2.reload"_f  = groov::disable,
                "cr2.autoend"_f = groov::disable,
                "cr2.start"_f   = true
            );
            auto read_ctrl = i2c_periph(
                "cr2.sadd7"_f   = device_addr,
                "cr2.rd_wrn"_f  = true,
                "cr2.nbytes"_f  = std::uint8_t(sizeof(T)),
                "cr2.reload"_f  = groov::disable,
                "cr2.autoend"_f = groov::enable,
                "cr2.start"_f   = true
            );

            return
                async::when_all(txis_or_error(), groov::write(write_ctrl))
                | async::seq(async::when_all(
                      tc_or_error(),
                      groov::write(i2c_periph(
                          "txdr.txdata"_f = static_cast<std::uint8_t>(addr)))
                  ))
                | async::seq(groov::write(read_ctrl))
                | async::seq(
                      rxne_or_error()
                      | async::then([](auto) -> T {
                            return static_cast<T>(
                                groov::sync_read<groov::blocking>(
                                    i2c_periph / "rxdr.rxdata"_f));
                        })
                  );
        }
    };
};

} // namespace hal
