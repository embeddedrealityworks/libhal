#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

#include <async/just_result_of.hpp>

#include <groov/groov.hpp>

#include <stdx/ct_string.hpp>

#include "../peripherals/i2c.hpp"

namespace hal {

/**
 * @brief groov-compatible bus adaptor for an I2C peripheral.
 *
 * Translates groov register read/write operations into the I2C wire protocol
 * used by single-byte-command-addressed devices such as the PCA9555:
 *
 * @par Write transaction
 * @code
 *   START  [i2c_addr | W]  [cmd = reg address]  [data byte(s)]  STOP
 * @endcode
 *
 * @par Read transaction
 * @code
 *   START  [i2c_addr | W]  [cmd = reg address]
 *   RESTART  [i2c_addr | R]  [data byte(s)]  STOP
 * @endcode
 *
 * When a write covers only a subset of a register's bits (Mask ≠ 0xFF),
 * a read-modify-write cycle is performed automatically to preserve the
 * untouched bits.
 *
 * @tparam i2ci      Reference to a concrete @c hal::i2c<Derived> driver.
 * @tparam i2c_addr  7-bit device address on the bus (e.g. @c 0x24).
 *
 * @par Usage
 * @code{.cpp}
 * extern hal::stm32f4::i2c<i2c1> my_i2c;
 * using my_bus = hal::i2cbus<my_i2c, 0x24>;
 * using my_device = groov::group<"dev", my_bus, reg0, reg1>;
 * @endcode
 */
template <auto &i2ci, std::uint8_t i2c_addr>
struct i2cbus {

    /**
     * @brief Write a register value over I2C.
     *
     * If @p Mask covers the full register width the value is sent in a single
     * 2-byte transaction (@c [cmd, data]).  Otherwise the register is first
     * read back and the relevant bits are merged before writing.
     *
     * @tparam Mask     Bitmask of bits being written.
     * @tparam IdMask   Identity (preserved) bit mask supplied by groov.
     * @tparam IdValue  Identity bit values supplied by groov.
     * @param addr  Register command byte (groov register address).
     * @param value Value to write; only bits within @p Mask are significant.
     * @return An @c async::sender that completes after the transaction.
     */
    template <stdx::ct_string, auto Mask, auto IdMask, auto IdValue>
    static auto write(auto addr, decltype(Mask) value) -> async::sender auto {
        using T = decltype(Mask);
        return async::just_result_of([=]() -> void {
            auto const cmd            = static_cast<std::byte>(addr);
            constexpr T combined_mask = Mask | IdMask;
            constexpr T full_mask     = std::numeric_limits<T>::max();

            if constexpr (combined_mask == full_mask) {
                // Full-register write — send [cmd, data] directly.
                std::array<std::byte, 2> tx{cmd,
                    static_cast<std::byte>(value | IdValue)};
                i2ci.write(i2c_addr, std::span<const std::byte>{tx});
            } else {
                // Partial write — read current, merge new bits, write back.
                std::byte raw{};
                std::array<std::byte, 1> cmd_buf{cmd};
                i2ci.write_read(i2c_addr,
                                std::span<const std::byte>{cmd_buf},
                                std::span<std::byte>{&raw, 1});

                auto const old_val = static_cast<T>(raw);
                auto const new_val = static_cast<std::byte>(
                    (old_val & static_cast<T>(~combined_mask)) |
                    (value   & Mask) |
                    IdValue);

                std::array<std::byte, 2> tx{cmd, new_val};
                i2ci.write(i2c_addr, std::span<const std::byte>{tx});
            }
        });
    }

    /**
     * @brief Read a register value over I2C.
     *
     * Sends the command byte then reads @c sizeof(T) data bytes using a
     * repeated-START (write_read) transaction.
     *
     * @tparam Mask  Bitmask indicating which bits will be inspected; its type
     *               determines the register width @c T.
     * @param addr  Register command byte (groov register address).
     * @return An @c async::sender that produces the raw register value of
     *         type @c T.
     */
    template <stdx::ct_string, auto Mask>
    static auto read(auto addr) -> async::sender auto {
        using T = decltype(Mask);
        return async::just_result_of([=]() -> T {
            std::array<std::byte, 1>         cmd{static_cast<std::byte>(addr)};
            std::array<std::byte, sizeof(T)> rx{};
            i2ci.write_read(i2c_addr,
                            std::span<const std::byte>{cmd},
                            std::span<std::byte>{rx});
            return std::bit_cast<T>(rx);
        });
    }
};

} // namespace hal
