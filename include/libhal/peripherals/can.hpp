#pragma once

#include <concepts>
#include <cstdint>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// CAN frame definition
// ---------------------------------------------------------------------------

/// @brief Maximum data length for a CAN classic frame (bytes).
inline constexpr unsigned can_max_dlc = 8u;

/**
 * @brief A single CAN 2.0A/B data frame.
 *
 * Supports both standard (11-bit) and extended (29-bit) identifiers.
 * Set @c extended = @c true for 29-bit IDs; the @c id field is interpreted
 * accordingly by the driver.
 */
struct can_frame {
    u32  id       : 29;          ///< 11-bit (standard) or 29-bit (extended) identifier.
    u8   dlc      :  4;          ///< Data length code — number of valid payload bytes (0–8).
    bool extended :  1;          ///< @c true for 29-bit extended ID; @c false for 11-bit.
    bool rtr      :  1;          ///< @c true for remote transmission request frame.
    u8   data[can_max_dlc];      ///< Payload bytes (only the first @c dlc bytes are valid).
};

// ---------------------------------------------------------------------------
// Concept
// ---------------------------------------------------------------------------

/**
 * @brief Concept for a CAN bus controller driver.
 *
 * A conforming type must expose:
 *  - @c set_baudrate(freq) — configure bus baud rate (call while stopped).
 *  - @c send(const can_frame&) → @c bool — transmit frame; @c false if TX
 *    mailboxes are full.
 *  - @c recv(can_frame&) → @c bool — read oldest frame from RX FIFO; @c false
 *    if no frame is waiting.
 *  - @c pending() → @c unsigned — number of frames in the RX FIFO.
 *
 * @par Example
 * @code{.cpp}
 * template <hal::can_controller Can>
 * void send_heartbeat(Can& can) {
 *     can_frame f{ .id = 0x100, .dlc = 1, .data = {0xAA} };
 *     can.send(f);
 * }
 * @endcode
 */
template <typename T>
concept can_controller = requires(T t, can_frame f, freq baud) {
    t.set_baudrate(baud);
    { t.send(f) }   -> std::convertible_to<bool>;
    { t.recv(f) }   -> std::convertible_to<bool>;
    { t.pending() } -> std::convertible_to<unsigned>;
};

} // namespace hal
