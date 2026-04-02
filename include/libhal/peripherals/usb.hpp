#pragma once

#include <cstddef>
#include <span>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// USB speed tag types
// ---------------------------------------------------------------------------

struct usb_full_speed_t {};  ///< USB 1.1 full-speed  (12 Mbit/s).
struct usb_high_speed_t {};  ///< USB 2.0 high-speed  (480 Mbit/s).

inline constexpr usb_full_speed_t usb_full_speed{};
inline constexpr usb_high_speed_t usb_high_speed{};

// ---------------------------------------------------------------------------
// Endpoint direction tag types
// ---------------------------------------------------------------------------

struct usb_in_t  {};  ///< IN endpoint: data flows Device → Host.
struct usb_out_t {};  ///< OUT endpoint: data flows Host → Device.

inline constexpr usb_in_t  usb_in{};
inline constexpr usb_out_t usb_out{};

// ---------------------------------------------------------------------------
// Transfer type tag types
// ---------------------------------------------------------------------------

struct usb_control_t      {};  ///< Control transfer type.
struct usb_bulk_t         {};  ///< Bulk transfer type.
struct usb_interrupt_ep_t {};  ///< Interrupt transfer type.
struct usb_isochronous_t  {};  ///< Isochronous transfer type.

inline constexpr usb_control_t      usb_control{};
inline constexpr usb_bulk_t         usb_bulk{};
inline constexpr usb_interrupt_ep_t usb_interrupt_ep{};
inline constexpr usb_isochronous_t  usb_isochronous{};

/**
 * @brief CRTP base for USB device-mode endpoint access.
 *
 * Each endpoint is a separate CRTP type parameterized by endpoint number,
 * direction, and transfer type.  The USB core and enumeration stack sit above
 * this layer and are out of scope for this peripheral interface.
 *
 * @tparam Derived   Concrete endpoint driver.
 * @tparam N         Endpoint number (0 = control endpoint).
 * @tparam Dir       Direction: @ref usb_in_t or @ref usb_out_t.
 * @tparam XferType  Transfer type: @ref usb_control_t, @ref usb_bulk_t, etc.
 * @tparam Speed     Bus speed: @ref usb_full_speed_t or @ref usb_high_speed_t.
 *
 * @par Usage
 * @code{.cpp}
 * class my_ep1 : public hal::usb_endpoint<my_ep1, 1,
 *                                          hal::usb_in_t,
 *                                          hal::usb_bulk_t> {
 *   friend class hal::usb_endpoint<my_ep1, 1,
 *                                  hal::usb_in_t,
 *                                  hal::usb_bulk_t>;
 *   unsigned write_impl(std::span<const std::byte> data);
 *   unsigned read_impl (std::span<std::byte>       buf);
 *   bool     ready_impl();
 * };
 * @endcode
 */
template<typename Derived,
         unsigned N,
         typename Dir,
         typename XferType = usb_bulk_t,
         typename Speed    = usb_full_speed_t>
class usb_endpoint {
public:
  static constexpr unsigned ep_number = N;  ///< Endpoint number.
  using direction  = Dir;       ///< Endpoint direction tag type.
  using xfer_type  = XferType;  ///< Transfer type tag type.
  using speed      = Speed;     ///< Bus speed tag type.

  /// @brief Queue data into the IN endpoint buffer (device → host).
  /// @param data Bytes to send.
  /// @return Number of bytes queued.
  /// @note Only available for @ref usb_in_t endpoints.
  [[nodiscard]] unsigned write(this auto& self,
                               std::span<const std::byte> data)
    requires std::is_same_v<Dir, usb_in_t>
  {
    return self.write_impl(data);
  }

  /// @brief Read data from the OUT endpoint buffer (host → device).
  /// @param buf Destination buffer.
  /// @return Number of bytes read, or 0 if no data is available.
  /// @note Only available for @ref usb_out_t endpoints.
  [[nodiscard]] unsigned read(this auto& self, std::span<std::byte> buf)
    requires std::is_same_v<Dir, usb_out_t>
  {
    return self.read_impl(buf);
  }

  /// @brief Check whether the endpoint is ready.
  ///
  ///  - IN endpoint: @c true when the TX buffer is empty (data was sent).
  ///  - OUT endpoint: @c true when data is waiting in the RX buffer.
  ///
  /// @return @c true if the endpoint is ready.
  [[nodiscard]] bool ready(this auto& self) {
    return self.ready_impl();
  }

protected:
  usb_endpoint()  = default;
  ~usb_endpoint() = default;
};

} // namespace hal
