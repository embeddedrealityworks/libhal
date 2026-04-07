#pragma once

#include <groov/groov.hpp>

/// @file access.hpp
/// @brief Register access-mode tags used when defining groov registers.
///
/// Pass one of these aliases as the @c WriteFn template parameter of
/// @c groov::reg to enforce hardware-appropriate access semantics:
///
/// | Alias        | Read | Write | groov write function                     |
/// |--------------|------|-------|------------------------------------------|
/// | @c access::ro | yes  | no    | @c groov::read_only (ignores writes)      |
/// | @c access::rw | yes  | yes   | @c groov::w::replace (read-modify-write)  |
/// | @c access::wo | no   | yes   | @c groov::write_only (skips readback)     |

namespace hal::access {

/// @brief Read-only access mode.
///
/// Compile-time guard: groov will reject any attempt to write to a register
/// declared with this access mode.  Reads return the current hardware value.
using ro = groov::read_only<groov::w::ignore>;

/// @brief Read-write access mode.
///
/// Default register access.  groov performs a read-modify-write when only
/// a subset of bits is written, preserving the untouched bits.
using rw = groov::w::replace;

/// @brief Write-only access mode.
///
/// Compile-time guard: groov will reject any attempt to read from a register
/// declared with this access mode.  Writes go directly without a prior read.
using wo = groov::write_only<groov::w::replace>;

} // namespace hal::access
