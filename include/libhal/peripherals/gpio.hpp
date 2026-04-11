#pragma once

#include <concepts>

namespace hal {

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

// ---------------------------------------------------------------------------
// Concepts
// ---------------------------------------------------------------------------

/**
 * @brief Concept for a readable digital input pin.
 *
 * Satisfied by any type that exposes a @c read() member returning a value
 * convertible to @c bool.  This includes MCU GPIO inputs, expander pins
 * (@c hal::devices::iopin), and software-simulated pins.
 */
template <typename T>
concept gpio_input = requires(T t) {
    { t.read() } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for a driveable digital output pin.
 *
 * Satisfied by any type that exposes @c set(), @c clear(), @c toggle(), and
 * @c read() members.  @c read() reflects the current driven value (IDR or
 * the output latch, depending on the implementation).
 */
template <typename T>
concept gpio_output = requires(T t) {
    t.set();
    t.clear();
    t.toggle();
    { t.read() } -> std::convertible_to<bool>;
};

} // namespace hal
