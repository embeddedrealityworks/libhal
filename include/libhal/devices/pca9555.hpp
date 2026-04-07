#pragma once

#include <cstdint>

#include <groov/groov.hpp>

#include "../utils/access.hpp"
#include "../utils/bittypes.hpp"
#include "i2c_bus.hpp"

namespace hal::devices {

namespace pca9555::regs {
using ip0_ = groov::reg<"ip0", std::uint8_t, 0x00, hal::access::ro,
                        groov::field<"i0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"i1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"i2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"i3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"i4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"i5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"i6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"i7", hal::bittypes::bit_enable, 7, 7>>;
using ip1_ = groov::reg<"ip1", std::uint8_t, 0x01, hal::access::ro,
                        groov::field<"i0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"i1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"i2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"i3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"i4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"i5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"i6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"i7", hal::bittypes::bit_enable, 7, 7>>;
using op0_ = groov::reg<"op0", std::uint8_t, 0x02, hal::access::rw,
                        groov::field<"o0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"o1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"o2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"o3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"o4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"o5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"o6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"o7", hal::bittypes::bit_enable, 7, 7>>;
using op1_ = groov::reg<"op1", std::uint8_t, 0x03, hal::access::rw,
                        groov::field<"o0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"o1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"o2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"o3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"o4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"o5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"o6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"o7", hal::bittypes::bit_enable, 7, 7>>;
using pi0_ = groov::reg<"pi0", std::uint8_t, 0x04, hal::access::rw,
                        groov::field<"pi0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"pi1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"pi2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"pi3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"pi4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"pi5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"pi6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"pi7", hal::bittypes::bit_enable, 7, 7>>;
using pi1_ = groov::reg<"pi1", std::uint8_t, 0x05, hal::access::rw,
                        groov::field<"pi0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"pi1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"pi2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"pi3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"pi4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"pi5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"pi6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"pi7", hal::bittypes::bit_enable, 7, 7>>;
using cf0_ = groov::reg<"cf0", std::uint8_t, 0x06, hal::access::rw,
                        groov::field<"cf0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"cf1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"cf2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"cf3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"cf4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"cf5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"cf6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"cf7", hal::bittypes::bit_enable, 7, 7>>;
using cf1_ = groov::reg<"cf1", std::uint8_t, 0x07, hal::access::rw,
                        groov::field<"cf0", hal::bittypes::bit_enable, 0, 0>,
                        groov::field<"cf1", hal::bittypes::bit_enable, 1, 1>,
                        groov::field<"cf2", hal::bittypes::bit_enable, 2, 2>,
                        groov::field<"cf3", hal::bittypes::bit_enable, 3, 3>,
                        groov::field<"cf4", hal::bittypes::bit_enable, 4, 4>,
                        groov::field<"cf5", hal::bittypes::bit_enable, 5, 5>,
                        groov::field<"cf6", hal::bittypes::bit_enable, 6, 6>,
                        groov::field<"cf7", hal::bittypes::bit_enable, 7, 7>>;
} // namespace pca9555::regs

template <auto& i2cp, std::uint8_t address>
using pca9555_t = groov::group<"pca9555", hal::i2cbus<address, i2cp>,
                               ip0_, ip1_, op0_, op1_, pi0_, pi1_, cf0_, cf1_>;

// instantiate device with for example
// constexpr auto ioexpander = pca9555_t<i2c1,0x24>{};

} // namespace hal::devices
