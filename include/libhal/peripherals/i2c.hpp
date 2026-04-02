#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

/**
 * @brief CRTP base for I2C bus controllers.
 *
 * The 7-bit device address is passed per-call so one bus instance can be
 * shared by multiple devices without wrapping.
 *
 * @tparam Derived Concrete I2C driver (e.g. @c hal::stm32f4::i2c<i2c1>).
 *
 * @par Usage
 * @code{.cpp}
 * class my_i2c : public hal::i2c<my_i2c> {
 *   friend class hal::i2c<my_i2c>;
 *   void write_impl(u8 addr, std::span<const std::byte> data);
 *   void read_impl (u8 addr, std::span<std::byte> data);
 * };
 * @endcode
 */
template<typename Derived>
class i2c {
public:
  /// @brief Send bytes to a device then issue a STOP condition.
  /// @param addr 7-bit device address.
  /// @param data Bytes to transmit.
  void write(this auto& self, u8 addr, std::span<const std::byte> data) {
    self.write_impl(addr, data);
  }

  /// @brief Read bytes from a device then issue a STOP condition.
  /// @param addr 7-bit device address.
  /// @param data Buffer to fill; reads exactly @c data.size() bytes.
  void read(this auto& self, u8 addr, std::span<std::byte> data) {
    self.read_impl(addr, data);
  }

  /// @brief Combined write → repeated-START → read in a single transaction.
  ///
  /// Issues: START → write @p tx → REPEATED START → read @p rx → STOP.
  /// This is the standard way to write a register address and read back data.
  ///
  /// @param addr 7-bit device address.
  /// @param tx   Bytes to transmit (e.g. register address).
  /// @param rx   Buffer to fill with the response.
  void write_read(this auto&                  self,
                  u8                          addr,
                  std::span<const std::byte> tx,
                  std::span<std::byte>        rx)
  {
    self.write_read_impl(addr, tx, rx);
  }

protected:
  i2c()  = default;
  ~i2c() = default;
};

} // namespace hal
