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

// groov-compatible bus adaptor for an I2C peripheral.
//
// Maps groov register read/write operations to the I2C protocol used
// by single-byte-addressed devices (e.g. PCA9555):
//
//   Write:  START [i2c_addr W] [cmd] [data...] STOP
//   Read:   START [i2c_addr W] [cmd] RESTART [i2c_addr R] [data...] STOP
//
// Template parameters:
//   i2c_addr  7-bit device address on the bus (e.g. 0x24).
//   i2ci      Reference to a concrete hal::i2c<Derived> driver.
template <std::uint8_t i2c_addr, auto& i2ci>
struct i2cbus {

    // Write a register value over I2C.
    // If Mask does not cover the full register width, performs a
    // read-modify-write to preserve bits outside the mask.
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

    // Read a register value over I2C.
    // Sends the command byte then reads sizeof(T) data bytes back.
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
