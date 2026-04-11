#pragma once

#include <groov/groov.hpp>

#include "../utils/bittypes.hpp"

namespace hal::devices {

/**
 * @brief Single-bit I/O pin backed by a field in a groov device group.
 *
 * Wraps one @c hal::bittypes::bit_enable field within a groov group (e.g. a
 * single pin of a @c pca9555_t expander).  All operations are synchronous
 * and use @c groov::blocking, making them safe to call from the main loop.
 *
 * @tparam Device    @c groov::group type that owns the field
 *                   (e.g. @c pca9555_t<bus1::device<0x24>>).
 * @tparam FieldPath Compile-time groov field path literal
 *                   (e.g. @c "op0.o3"_f).
 *
 * @par Satisfies
 *  - @c hal::gpio_input  (via @c read())
 *  - @c hal::gpio_output (via @c set(), @c clear(), @c toggle(), @c read())
 *
 * @par Usage
 * @code{.cpp}
 * using bus1   = hal::i2c_bus<mcu::stm32::i2c1, "i2c1_ev", "i2c1_err">;
 * using expander = hal::devices::pca9555_t<bus1::device<0x24>>;
 *
 * // Pin 3 of port 0, used as output:
 * using led_pin = hal::devices::iopin<expander, "op0.o3"_f>;
 *
 * led_pin::set();     // drive high
 * led_pin::clear();   // drive low
 * led_pin::toggle();  // invert
 * bool state = led_pin::read();
 * @endcode
 */
template <typename Device, auto FieldPath>
struct iopin {
    using bit_t = hal::bittypes::bit_enable;

    /// @brief Drive the pin high (@c bit_enable::ENABLE).
    static void set() noexcept {
        groov::sync_write<groov::blocking>(
            Device{} / (FieldPath = bit_t::ENABLE));
    }

    /// @brief Drive the pin low (@c bit_enable::DISABLE).
    static void clear() noexcept {
        groov::sync_write<groov::blocking>(
            Device{} / (FieldPath = bit_t::DISABLE));
    }

    /// @brief Toggle the current output state.
    static void toggle() noexcept {
        auto cur = groov::sync_read<groov::blocking>(Device{} / FieldPath);
        groov::sync_write<groov::blocking>(
            Device{} / (FieldPath = (cur == bit_t::ENABLE)
                                        ? bit_t::DISABLE
                                        : bit_t::ENABLE));
    }

    /// @brief Read the current logical level of the pin.
    /// @return @c true if @c bit_enable::ENABLE; @c false otherwise.
    [[nodiscard]] static bool read() noexcept {
        return groov::sync_read<groov::blocking>(Device{} / FieldPath)
               == bit_t::ENABLE;
    }
};

} // namespace hal::devices
