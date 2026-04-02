#pragma once

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

/**
 * @brief CRTP base for hardware timers and counters.
 *
 * @tparam Derived Concrete timer driver.
 * @tparam Mode    Timer mode: @ref timer_oneshot_t, @ref timer_periodic_t,
 *                 or @ref timer_pwm_t.
 *
 * @par Usage (periodic)
 * @code{.cpp}
 * class my_timer : public hal::timer<my_timer, hal::timer_periodic_t> {
 *   friend class hal::timer<my_timer, hal::timer_periodic_t>;
 *   void start_impl();
 *   void stop_impl();
 *   void reset_impl();
 *   void set_period_impl(freq period_hz);
 *   u32  count_impl();
 *   bool expired_impl();
 * };
 * @endcode
 */
template<typename Derived, typename Mode = timer_periodic_t>
class timer {
public:
  using mode = Mode;

  /// @brief Start the timer from its current counter value.
  void start(this auto& self) { self.start_impl(); }

  /// @brief Stop the timer (counter value is preserved).
  void stop(this auto& self)  { self.stop_impl();  }

  /// @brief Reset the counter to zero without stopping the timer.
  void reset(this auto& self) { self.reset_impl(); }

  /// @brief Set the period / overflow frequency.
  /// @param period_hz Overflow frequency in Hz.  For one-shot mode this sets
  ///                  the timeout duration.
  void set_period(this auto& self, freq period_hz) {
    self.set_period_impl(period_hz);
  }

  /// @brief Read the current raw counter value.
  /// @return Current counter register value.
  [[nodiscard]] u32 count(this auto& self) {
    return self.count_impl();
  }

  /// @brief Check whether the timer has expired or overflowed.
  ///
  /// Returns @c true if the timer has expired (one-shot) or overflowed
  /// (periodic) since the last call.  Clears the flag on read.
  ///
  /// @return @c true if an expiry/overflow event is pending.
  [[nodiscard]] bool expired(this auto& self) {
    return self.expired_impl();
  }

protected:
  timer()  = default;
  ~timer() = default;
};

/**
 * @brief CRTP base for PWM output channels.
 *
 * @tparam Derived Concrete PWM driver.
 *
 * @par Usage
 * @code{.cpp}
 * class my_pwm : public hal::pwm<my_pwm> {
 *   friend class hal::pwm<my_pwm>;
 *   void set_frequency_impl(freq f);
 *   void set_duty_cycle_impl(float duty); // 0.0 – 1.0
 * };
 * @endcode
 */
template<typename Derived>
class pwm {
public:
  /// @brief Set the PWM carrier frequency.
  /// @param f Carrier frequency in Hz.
  void set_frequency(this auto& self, freq f) {
    self.set_frequency_impl(f);
  }

  /// @brief Set the duty cycle.
  /// @param duty Duty cycle in the range [0.0, 1.0].
  void set_duty_cycle(this auto& self, float duty) {
    self.set_duty_cycle_impl(duty);
  }

protected:
  pwm()  = default;
  ~pwm() = default;
};

} // namespace hal
