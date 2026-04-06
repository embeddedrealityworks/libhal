#pragma once

#include <cstdint>

#include <groov/groov.hpp>

#include <stdx/ct_string.hpp>
#include <stdx/literals.hpp>

namespace hal::i2c {

template <std::uint8_t i2c_addr, auto i2ci> struct bus {

  template <stdx::ct_string RegName, auto Mask, auto IdMask, auto IdValue>
  static auto write(auto addr, decltype(Mask) data) -> async::sender auto {}

  template <stdx::ct_string RegName, auto Mask>
  static auto read(auto) -> async::sender auto {}
};

} // namespace hal::i2c
