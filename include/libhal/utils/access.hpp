#pragma once

#include <groov/groov.hpp>

namespace hal::access {
using ro = groov::read_only<groov::w::ignore>;
using rw = groov::w::replace;
using wo = groov::write_only<groov::w::replace>;

} // namespace hal::access
