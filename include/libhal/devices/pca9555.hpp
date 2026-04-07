#pragma once

#include <cstdint>

#include <groov/groov.hpp>

#include "../utils/access.hpp"
#include "../utils/bittypes.hpp"
#include "i2c_bus.hpp"

namespace hal::devices {

/// @brief groov register map for the NXP PCA9555 16-bit I2C I/O expander.
///
/// Registers are split into two 8-bit ports (port 0 = pins 0–7, port 1 = pins
/// 8–15).  Each port has four register types:
///
/// | Register | Command | Access | Description                       |
/// |----------|---------|--------|-----------------------------------|
/// | ip0/ip1  | 0x00–01 | RO     | Input Port — physical pin state   |
/// | op0/op1  | 0x02–03 | RW     | Output Port — driven output latch |
/// | pi0/pi1  | 0x04–05 | RW     | Polarity Inversion                |
/// | cf0/cf1  | 0x06–07 | RW     | Configuration (1=input, 0=output) |
namespace pca9555::regs {

/// @brief Input Port register — port 0 (pins 0–7).  Read-only; reflects the
///        logic level on each pin regardless of the direction setting.
using ip0_ = groov::reg<"ip0", std::uint8_t, 0x00, hal::access::ro,
                        groov::field<"i0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"i1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"i2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"i3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"i4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"i5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"i6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"i7", hal::bittypes::bit_enable, 7, 7>>;

/// @brief Input Port register — port 1 (pins 8–15).  Read-only; reflects the
///        logic level on each pin regardless of the direction setting.
using ip1_ = groov::reg<"ip1", std::uint8_t, 0x01, hal::access::ro,
                        groov::field<"i0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"i1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"i2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"i3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"i4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"i5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"i6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"i7", hal::bittypes::bit_enable, 7, 7>>;

/// @brief Output Port register — port 0 (pins 0–7).
///        Holds the value driven on each output pin; ignored for input pins.
using op0_ = groov::reg<"op0", std::uint8_t, 0x02, hal::access::rw,
                        groov::field<"o0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"o1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"o2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"o3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"o4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"o5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"o6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"o7", hal::bittypes::bit_enable, 7, 7>>;

/// @brief Output Port register — port 1 (pins 8–15).
///        Holds the value driven on each output pin; ignored for input pins.
using op1_ = groov::reg<"op1", std::uint8_t, 0x03, hal::access::rw,
                        groov::field<"o0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"o1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"o2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"o3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"o4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"o5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"o6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"o7", hal::bittypes::bit_enable, 7, 7>>;

/// @brief Polarity Inversion register — port 0 (pins 0–7).
///        When a bit is set the corresponding Input Port bit is inverted
///        before being reported.
using pi0_ = groov::reg<"pi0", std::uint8_t, 0x04, hal::access::rw,
                        groov::field<"pi0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"pi1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"pi2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"pi3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"pi4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"pi5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"pi6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"pi7", hal::bittypes::bit_enable, 7, 7>>;

/// @brief Polarity Inversion register — port 1 (pins 8–15).
///        When a bit is set the corresponding Input Port bit is inverted
///        before being reported.
using pi1_ = groov::reg<"pi1", std::uint8_t, 0x05, hal::access::rw,
                        groov::field<"pi0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"pi1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"pi2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"pi3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"pi4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"pi5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"pi6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"pi7", hal::bittypes::bit_enable, 7, 7>>;

/// @brief Configuration register — port 0 (pins 0–7).
///        @c ENABLE (1) = pin is an input; @c DISABLE (0) = pin is an output.
///        Power-on default is all inputs (0xFF).
using cf0_ = groov::reg<"cf0", std::uint8_t, 0x06, hal::access::rw,
                        groov::field<"cf0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"cf1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"cf2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"cf3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"cf4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"cf5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"cf6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"cf7", hal::bittypes::bit_enable, 7, 7>>;

/// @brief Configuration register — port 1 (pins 8–15).
///        @c ENABLE (1) = pin is an input; @c DISABLE (0) = pin is an output.
///        Power-on default is all inputs (0xFF).
using cf1_ = groov::reg<"cf1", std::uint8_t, 0x07, hal::access::rw,
                        groov::field<"cf0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"cf1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"cf2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"cf3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"cf4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"cf5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"cf6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"cf7", hal::bittypes::bit_enable, 7, 7>>;

} // namespace pca9555::regs

/**
 * @brief groov device group for the PCA9555 16-bit I2C I/O expander.
 *
 * Bundles all eight register definitions with the @c hal::i2cbus adaptor so
 * that groov read/write operations translate directly to I2C transactions.
 *
 * @tparam i2cp    Reference to the @c hal::i2c<Derived> bus driver.
 * @tparam address 7-bit I2C device address set by the A0–A2 pins
 *                 (0x20–0x27).
 *
 * @par Usage
 * @code{.cpp}
 * extern hal::stm32f4::i2c<i2c1> my_i2c;
 * constexpr auto ioexpander = hal::devices::pca9555_t<my_i2c, 0x24>{};
 *
 * // Configure pin 0 as output, set it high:
 * groov::sync_write(ioexpander / ("cf0.cf0"_f = hal::bittypes::bit_enable::DISABLE));
 * groov::sync_write(ioexpander / ("op0.o0"_f  = hal::bittypes::bit_enable::ENABLE));
 * @endcode
 */
template <auto& i2cp, std::uint8_t address>
using pca9555_t = groov::group<"pca9555", hal::i2cbus<i2cp, address>,
                               pca9555::regs::ip0_, pca9555::regs::ip1_,
                               pca9555::regs::op0_, pca9555::regs::op1_,
                               pca9555::regs::pi0_, pca9555::regs::pi1_,
                               pca9555::regs::cf0_, pca9555::regs::cf1_>;

} // namespace hal::devices
