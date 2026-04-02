#pragma once

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// Direction tag types
// ---------------------------------------------------------------------------

struct input_t  {};  ///< Tag type selecting input pin direction.
struct output_t {};  ///< Tag type selecting output pin direction.

inline constexpr input_t  input{};
inline constexpr output_t output{};

// ---------------------------------------------------------------------------
// Pull configuration tag types
// ---------------------------------------------------------------------------

struct pull_none_t {};  ///< Tag type disabling the internal pull resistor.
struct pull_up_t   {};  ///< Tag type enabling the internal pull-up resistor.
struct pull_down_t {};  ///< Tag type enabling the internal pull-down resistor.

inline constexpr pull_none_t pull_none{};
inline constexpr pull_up_t   pull_up{};
inline constexpr pull_down_t pull_down{};

// ---------------------------------------------------------------------------
// Output drive mode tag types
// ---------------------------------------------------------------------------

struct push_pull_t  {};  ///< Tag type selecting push-pull output drive.
struct open_drain_t {};  ///< Tag type selecting open-drain output drive.

inline constexpr push_pull_t  push_pull{};
inline constexpr open_drain_t open_drain{};

/**
 * @brief CRTP base for GPIO pins.
 *
 * @tparam Derived Concrete GPIO driver (MCU pin or expander channel).
 * @tparam Dir     Pin direction: @ref input_t or @ref output_t; enforced at
 *                 compile time via @c requires clauses.
 *
 * @par Usage (output)
 * @code{.cpp}
 * class my_pin : public hal::gpio<my_pin, hal::output_t> {
 *   friend class hal::gpio<my_pin, hal::output_t>;
 *   void set_impl()    { ... } // drive high
 *   void clear_impl()  { ... } // drive low
 *   void toggle_impl() { ... } // invert
 *   bool read_impl()   { ... } // read IDR
 * };
 * @endcode
 *
 * @par Usage (input)
 * @code{.cpp}
 * class my_pin : public hal::gpio<my_pin, hal::input_t> {
 *   friend class hal::gpio<my_pin, hal::input_t>;
 *   bool read_impl() { ... } // read IDR
 * };
 * @endcode
 */
template<typename Derived, typename Dir>
class gpio {
public:
  /// @brief Drive the pin high.
  /// @note Only available for @ref output_t pins.
  void set(this auto& self)
    requires std::is_same_v<Dir, output_t>
  {
    self.set_impl();
  }

  /// @brief Drive the pin low.
  /// @note Only available for @ref output_t pins.
  void clear(this auto& self)
    requires std::is_same_v<Dir, output_t>
  {
    self.clear_impl();
  }

  /// @brief Toggle the pin level.
  /// @note Only available for @ref output_t pins.
  void toggle(this auto& self)
    requires std::is_same_v<Dir, output_t>
  {
    self.toggle_impl();
  }

  /// @brief Read the logical level of the pin (available for both directions).
  /// @return @c true if the pin is high, @c false if low.
  [[nodiscard]] bool read(this auto& self) {
    return self.read_impl();
  }

protected:
  gpio()  = default;
  ~gpio() = default;
};

} // namespace hal
