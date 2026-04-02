#pragma once

#include <cstddef>
#include <span>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

/// @brief Minimum Ethernet frame size in bytes (header + payload, no FCS).
inline constexpr unsigned eth_min_frame = 60u;
/// @brief Maximum Ethernet frame size in bytes (header + payload, no FCS).
inline constexpr unsigned eth_max_frame = 1514u;

/**
 * @brief CRTP base for Ethernet MAC controllers.
 *
 * Targets simple DMA-less (polling) usage.  DMA-backed transfers are handled
 * in the concrete MCU driver; the base only describes the logical frame
 * send/receive contract.
 *
 * @tparam Derived Concrete Ethernet MAC driver.
 *
 * @par Usage
 * @code{.cpp}
 * class my_eth : public hal::eth<my_eth> {
 *   friend class hal::eth<my_eth>;
 *   bool     send_impl(std::span<const std::byte> frame);
 *   unsigned recv_impl(std::span<std::byte> buf);
 *   bool     link_up_impl();
 * };
 * @endcode
 */
template<typename Derived>
class eth {
public:
  /// @brief Transmit a raw Ethernet frame (header + payload, no FCS).
  /// @param frame Byte span of length [eth_min_frame, eth_max_frame].
  /// @return @c false if the TX descriptor ring is full.
  [[nodiscard]] bool send(this auto& self, std::span<const std::byte> frame) {
    return self.send_impl(frame);
  }

  /// @brief Copy the oldest received frame into a buffer.
  /// @param buf Destination buffer; should be at least @ref eth_max_frame bytes
  ///            to avoid truncation.
  /// @return Number of bytes written, or 0 if no frame is waiting.
  [[nodiscard]] unsigned recv(this auto& self, std::span<std::byte> buf) {
    return self.recv_impl(buf);
  }

  /// @brief Query the physical link state.
  /// @return @c true if the link is up.
  [[nodiscard]] bool link_up(this auto& self) {
    return self.link_up_impl();
  }

protected:
  eth()  = default;
  ~eth() = default;
};

} // namespace hal
