#pragma once

#include <cstdint>

#include <groov/groov.hpp>

#include "../utils/access.hpp"
#include "../utils/bittypes.hpp"
#include "../peripherals/spi.hpp"

namespace hal::devices {

/// @brief groov register map for the Microchip MCP4821 12-bit SPI DAC.
///
/// The MCP4821 accepts a single write-only 16-bit SPI command word:
///
/// | Bit  | Field  | Description                                          |
/// |------|--------|------------------------------------------------------|
/// | 15   | —      | Always 0 (channel select on the dual MCP4922)        |
/// | 14   | buf    | Input buffer: ENABLE = buffered, DISABLE = unbuffered |
/// | 13   | /ga    | Gain (active-low): ENABLE (0) = 2×, DISABLE (1) = 1× |
/// | 12   | /shdn  | Shutdown (active-low): ENABLE (0) = off, DISABLE = on |
/// | 11:0 | value  | 12-bit DAC output value (0–4095)                     |
///
/// @note @c /ga and @c /shdn use @c bit_nenable (active-low):
///       @c DISABLE keeps the device active / at 1× gain.
namespace mcp4821::regs {

/// @brief Write-only 16-bit SPI command register.
using cmd_ = groov::reg<"cmd", std::uint16_t, 0x00, hal::access::wo,
    groov::field<"value", std::uint16_t,               0, 11>,
    groov::field<"shdn",  hal::bittypes::bit_nenable, 12, 12>,
    groov::field<"ga",    hal::bittypes::bit_nenable, 13, 13>,
    groov::field<"buf",   hal::bittypes::bit_enable,  14, 14>>;

} // namespace mcp4821::regs

/**
 * @brief groov device group for the MCP4821 12-bit SPI DAC.
 *
 * @tparam Bus  groov-compatible SPI bus adaptor satisfying @ref hal::spi_bus.
 *              Use @c hal::spi_bus<periph, ...>::device<> (future).
 *
 * @par Usage
 * @code{.cpp}
 * using dac = hal::devices::mcp4821_t<bus1::device<>>;
 * constexpr auto ch = dac{};
 *
 * // Output 2048 (half scale), 1× gain, active, unbuffered:
 * groov::sync_write(ch / (
 *     "cmd.value"_f = std::uint16_t{2048},
 *     "cmd.shdn"_f  = hal::bittypes::bit_nenable::DISABLE,   // active
 *     "cmd.ga"_f    = hal::bittypes::bit_nenable::DISABLE,   // 1× gain
 *     "cmd.buf"_f   = hal::bittypes::bit_enable::DISABLE     // unbuffered
 * ));
 * @endcode
 */
template <hal::spi_bus Bus>
using mcp4821_t = groov::group<"mcp4821", Bus, mcp4821::regs::cmd_>;

} // namespace hal::devices
