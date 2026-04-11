#pragma once

#include <nexus/callback.hpp>
#include <nexus/service.hpp>

namespace hal {

/**
 * @brief CIB callback service invoked once at system startup.
 *
 * Any component that needs hardware initialisation (clocks, GPIOs,
 * peripherals) extends this service via @c cib::extend<hal::startup>.
 * The application calls the service exactly once, typically from
 * @c SystemInit() or the top of @c main():
 *
 * @code{.cpp}
 * cib::nexus<clock::init, bus1::init, ...> nexus{};
 * nexus.init();
 * cib::service<hal::startup>();
 * @endcode
 *
 * Extensions registered with @c cib::extend<hal::startup> are called in
 * the order they were registered (determined by nexus component order).
 */
struct startup : callback::service<> {};

} // namespace hal
