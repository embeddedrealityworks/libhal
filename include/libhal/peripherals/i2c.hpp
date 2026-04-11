#pragma once

#include <concepts>

namespace hal {

/**
 * @brief Concept for a groov-compatible I2C bus adaptor.
 *
 * A conforming type must provide static @c write<> and @c read<> template
 * methods that satisfy the groov bus contract (see groov's @c mmio_bus.hpp
 * for the canonical definition).  The full template signature is structurally
 * verified at @c groov::group<> instantiation time.
 *
 * @par Conforming types
 *  - @c hal::i2c_bus<periph, EvTrigger, ErrTrigger>::device<addr>
 *
 * @par Example
 * @code{.cpp}
 * // Constrain a device driver to accept only valid bus adaptors:
 * template <hal::i2c_bus Bus>
 * using my_device_t = groov::group<"my_device", Bus, my_reg_>;
 * @endcode
 */
template <typename T>
concept i2c_bus = std::is_class_v<T>;

} // namespace hal
