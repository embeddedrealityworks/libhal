#pragma once

/// @file bittypes.hpp
/// @brief Strongly-typed boolean enumerations for single-bit hardware fields.
///
/// Each enum class maps a 1-bit hardware register field to a meaningful name.
/// Two variants exist for every concept:
///
/// - **Normal polarity** — the enum value @c true corresponds to the
///   "active" or "asserted" state (e.g. @c bit_enable::ENABLE == 1).
/// - **Negated polarity** (prefix @c n) — the enum value @c true
///   corresponds to the "inactive" state, matching hardware registers that
///   use active-low signals (e.g. @c bit_nenable::ENABLE == 0).
///
/// The helper functions @c is_enabled(), @c is_ready(), and @c is_locked()
/// provide polarity-agnostic tests that work with both variants.

namespace hal::bittypes {

/**
 * @brief Active-high enable/disable bit.
 *
 * Use for fields where bit = 1 means enabled / on.
 *
 * | Enumerator | Bit value |
 * |------------|-----------|
 * | ENABLE / ON / one  | 1 |
 * | DISABLE / OFF / zero | 0 |
 */
enum class bit_enable : bool {
  ENABLE  = true,
  DISABLE = false,
  ON      = true,
  OFF     = false,
  one     = true,
  zero    = false
};

/**
 * @brief Active-low enable/disable bit (negated polarity).
 *
 * Use for fields where bit = 0 means enabled / on (e.g. nEN, active-low
 * chip-selects).
 *
 * | Enumerator | Bit value |
 * |------------|-----------|
 * | ENABLE / ON / zero  | 0 |
 * | DISABLE / OFF / one | 1 |
 */
enum class bit_nenable : bool {
  ENABLE  = false,
  DISABLE = true,
  ON      = false,
  OFF     = true,
  zero    = false,
  one     = true
};

/**
 * @brief Returns @c true if the bit represents an enabled state.
 *
 * Works with both @c bit_enable and @c bit_nenable.
 *
 * @tparam T One of @c bit_enable or @c bit_nenable.
 * @param v  The field value read from a register.
 */
template <typename T> auto is_enabled(T v) -> bool { return v == T::ENABLED; }

/**
 * @brief Active-high ready/not-ready bit.
 *
 * Use for status flags where bit = 1 means the peripheral or operation is
 * ready.
 *
 * | Enumerator | Bit value |
 * |------------|-----------|
 * | READY / one   | 1 |
 * | NOT_READY / zero | 0 |
 */
enum class bit_ready : bool {
  READY     = true,
  NOT_READY = false,
  zero      = false,
  one       = true
};

/**
 * @brief Active-low ready/not-ready bit (negated polarity).
 *
 * Use for status flags where bit = 0 means the peripheral or operation is
 * ready.
 *
 * | Enumerator | Bit value |
 * |------------|-----------|
 * | READY / zero     | 0 |
 * | NOT_READY / one  | 1 |
 */
enum class bit_nready : bool {
  READY     = false,
  NOT_READY = true,
  zero      = false,
  one       = true
};

/**
 * @brief Returns @c true if the bit represents a ready state.
 *
 * Works with both @c bit_ready and @c bit_nready.
 *
 * @tparam T One of @c bit_ready or @c bit_nready.
 * @param v  The field value read from a register.
 */
template <typename T> auto is_ready(T v) -> bool { return v == T::READY; }

/**
 * @brief Active-high locked/unlocked bit.
 *
 * Use for lock or protection flags where bit = 1 means locked / protected.
 *
 * | Enumerator | Bit value |
 * |------------|-----------|
 * | LOCKED / one    | 1 |
 * | UNLOCKED / zero | 0 |
 */
enum class bit_locked : bool {
  LOCKED   = true,
  UNLOCKED = false,
  zero     = false,
  one      = true
};

/**
 * @brief Active-low locked/unlocked bit (negated polarity).
 *
 * Use for lock or protection flags where bit = 0 means locked / protected.
 *
 * | Enumerator | Bit value |
 * |------------|-----------|
 * | LOCKED / zero    | 0 |
 * | UNLOCKED / one   | 1 |
 */
enum class bit_nlocked : bool {
  LOCKED   = false,
  UNLOCKED = true,
  zero     = false,
  one      = true
};

/**
 * @brief Returns @c true if the bit represents a locked state.
 *
 * Works with both @c bit_locked and @c bit_nlocked.
 *
 * @tparam T One of @c bit_locked or @c bit_nlocked.
 * @param v  The field value read from a register.
 */
template <typename T> auto is_locked(T v) -> bool { return v == T::LOCKED; }

/**
 * @brief Write-action bit for self-clearing control fields.
 *
 * Use for fields where writing 1 triggers a one-shot action (e.g. a software
 * reset or a DMA trigger).  The @c do_nothing enumerator allows the field to
 * be written without triggering the action.
 *
 * | Enumerator | Bit value |
 * |------------|-----------|
 * | RESET / SET / one | 1 (triggers action) |
 * | do_nothing / zero | 0 (no effect)       |
 */
enum class bit_reset : bool {
  do_nothing = false,
  RESET      = true,
  SET        = true,
  zero       = false,
  one        = true
};

} // namespace hal::bittypes
