#pragma once

#include <cstddef>
#include <span>

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

// ---------------------------------------------------------------------------
// SPI clock polarity / phase tag types (mode 0–3)
// ---------------------------------------------------------------------------

struct spi_mode0_t {};  ///< CPOL=0, CPHA=0 — clock idle low,  sample on rising  edge.
struct spi_mode1_t {};  ///< CPOL=0, CPHA=1 — clock idle low,  sample on falling edge.
struct spi_mode2_t {};  ///< CPOL=1, CPHA=0 — clock idle high, sample on falling edge.
struct spi_mode3_t {};  ///< CPOL=1, CPHA=1 — clock idle high, sample on rising  edge.

inline constexpr spi_mode0_t spi_mode0{};
inline constexpr spi_mode1_t spi_mode1{};
inline constexpr spi_mode2_t spi_mode2{};
inline constexpr spi_mode3_t spi_mode3{};

// ---------------------------------------------------------------------------
// Bit order tag types
// ---------------------------------------------------------------------------

struct msb_first_t {};  ///< Transmit/receive most-significant bit first.
struct lsb_first_t {};  ///< Transmit/receive least-significant bit first.

inline constexpr msb_first_t msb_first{};
inline constexpr lsb_first_t lsb_first{};

/**
 * @brief CRTP base for SPI bus controllers.
 *
 * Chip-select management is intentionally left to the caller so that any
 * GPIO type (MCU pin, expander channel) can be used without coupling.
 *
 * @tparam Derived   Concrete SPI driver (e.g. @c hal::stm32f4::spi<spi1>).
 * @tparam Mode      Clock mode: one of @ref spi_mode0_t … @ref spi_mode3_t.
 * @tparam BitOrder  Bit order: @ref msb_first_t or @ref lsb_first_t.
 *
 * @par Usage
 * @code{.cpp}
 * class my_spi : public hal::spi<my_spi, hal::spi_mode0_t> {
 *   friend class hal::spi<my_spi, hal::spi_mode0_t>;
 *   void transfer_impl(std::span<const std::byte> tx,
 *                      std::span<std::byte>       rx);
 * };
 * @endcode
 */
template<typename Derived,
         typename Mode     = spi_mode0_t,
         typename BitOrder = msb_first_t>
class spi {
public:
  using mode      = Mode;
  using bit_order = BitOrder;

  /// @brief Full-duplex transfer: transmit @p tx while capturing into @p rx.
  ///
  /// Spans may have different lengths:
  ///  - If @p tx is shorter, @c 0xFF is clocked out for the remaining bytes.
  ///  - If @p rx is shorter, incoming bytes beyond its length are discarded.
  ///  - Pass an empty span for @p rx to do a write-only transfer, or an empty
  ///    @p tx to do a read-only transfer.
  ///
  /// @param tx Bytes to transmit.
  /// @param rx Buffer to fill with received bytes.
  void transfer(this auto&                  self,
                std::span<const std::byte> tx,
                std::span<std::byte>       rx)
  {
    self.transfer_impl(tx, rx);
  }

  /// @brief Write-only convenience overload (no capture).
  /// @param tx Bytes to transmit.
  void write(this auto& self, std::span<const std::byte> tx) {
    self.transfer_impl(tx, {});
  }

  /// @brief Read-only convenience overload (clocks out @c 0xFF).
  /// @param rx Buffer to fill with received bytes.
  void read(this auto& self, std::span<std::byte> rx) {
    self.transfer_impl({}, rx);
  }

protected:
  spi()  = default;
  ~spi() = default;
};

} // namespace hal
