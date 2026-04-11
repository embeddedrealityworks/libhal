#pragma once

#include <concepts>
#include <cstdint>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// Timer mode tag types
// ---------------------------------------------------------------------------

struct timer_oneshot_t  {};  ///< One-shot mode: fires once then stops.
struct timer_periodic_t {};  ///< Periodic mode: auto-reloads on every overflow.
struct timer_pwm_t      {};  ///< PWM / input-capture / output-compare mode.

inline constexpr timer_oneshot_t  timer_oneshot{};
inline constexpr timer_periodic_t timer_periodic{};
inline constexpr timer_pwm_t      timer_pwm{};

// ---------------------------------------------------------------------------
// Concepts
// ---------------------------------------------------------------------------

/**
 * @brief Concept for a hardware timer or counter peripheral.
 *
 * A conforming type must expose @c start(), @c stop(), @c reset(),
 * @c set_period(freq), @c count() → @c uint32_t, and @c expired() → @c bool.
 *
 * The operating mode (one-shot / periodic / PWM) is selected at compile time
 * via the @ref timer_oneshot_t, @ref timer_periodic_t, or @ref timer_pwm_t
 * tag type passed to the concrete driver.
 *
 * @par Example
 * @code{.cpp}
 * template <hal::timer_peripheral Timer>
 * void wait(Timer& t, freq period) {
 *     t.set_period(period);
 *     t.start();
 *     while (!t.expired()) {}
 * }
 * @endcode
 */
template <typename T>
concept timer_peripheral = requires(T t, freq f) {
    t.start();
    t.stop();
    t.reset();
    t.set_period(f);
    { t.count() }   -> std::convertible_to<std::uint32_t>;
    { t.expired() } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for a PWM output channel.
 *
 * A conforming type must expose @c set_frequency(freq) and
 * @c set_duty_cycle(float) (duty in [0.0, 1.0]).
 *
 * @par Example
 * @code{.cpp}
 * template <hal::pwm_channel Pwm>
 * void set_50pct(Pwm& p, freq carrier) {
 *     p.set_frequency(carrier);
 *     p.set_duty_cycle(0.5f);
 * }
 * @endcode
 */
template <typename T>
concept pwm_channel = requires(T t, freq f, float d) {
    t.set_frequency(f);
    t.set_duty_cycle(d);
};

} // namespace hal
