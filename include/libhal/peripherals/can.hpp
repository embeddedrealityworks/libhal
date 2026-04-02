#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

/// @brief Maximum data length for a CAN classic frame (bytes).
inline constexpr unsigned can_max_dlc = 8u;

/// @brief A single CAN 2.0A/B frame.
struct can_frame {
  u32  id       : 29;  ///< 11-bit (standard) or 29-bit (extended) identifier.
  u8   dlc      : 4;   ///< Data length code (0–8 bytes).
  bool extended : 1;   ///< @c true for 29-bit extended ID.
  bool rtr      : 1;   ///< @c true for remote transmission request.
  u8   data[can_max_dlc];  ///< Payload bytes.
};

/**
 * @brief CRTP base for CAN bus controllers.
 *
 * @tparam Derived Concrete CAN driver (e.g. @c hal::stm32f4::can<can1>).
 *
 * @par Usage
 * @code{.cpp}
 * class my_can : public hal::can<my_can> {
 *   friend class hal::can<my_can>;
 *   void     set_baudrate_impl(freq baud);
 *   bool     send_impl(const can_frame& frame);
 *   bool     recv_impl(can_frame& frame);
 *   unsigned pending_impl();
 * };
 * @endcode
 */
template<typename Derived>
class can {
public:
  /// @brief Set the bus baud rate.
  /// @param baud Target bus frequency.
  /// @note Must be called while the controller is stopped.
  void set_baudrate(this auto& self, freq baud) {
    self.set_baudrate_impl(baud);
  }

  /// @brief Transmit a frame.
  /// @param frame Frame to send.
  /// @return @c false if the TX mailboxes are full.
  [[nodiscard]] bool send(this auto& self, const can_frame& frame) {
    return self.send_impl(frame);
  }

  /// @brief Receive the oldest frame from the RX FIFO.
  /// @param[out] frame Destination for the received frame.
  /// @return @c false if no frame is waiting.
  [[nodiscard]] bool recv(this auto& self, can_frame& frame) {
    return self.recv_impl(frame);
  }

  /// @brief Query the number of frames waiting in the RX FIFO.
  /// @return Frame count (0 if the FIFO is empty).
  [[nodiscard]] unsigned pending(this auto& self) {
    return self.pending_impl();
  }

protected:
  can()  = default;
  ~can() = default;
};

} // namespace hal
