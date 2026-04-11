#pragma once

#include <concepts>

namespace hal {

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

// ---------------------------------------------------------------------------
// Concept
// ---------------------------------------------------------------------------

/**
 * @brief Concept for a groov-compatible SPI bus adaptor.
 *
 * A conforming type must provide static @c write<> and @c read<> template
 * methods satisfying the groov bus contract.  The contract is structurally
 * validated at @c groov::group<> instantiation time.
 *
 * @par Conforming types
 *  - @c hal::spi_bus<periph, ...>::device (future)
 *
 * @par Mode and bit-order selection
 * Use the @ref spi_mode0_t … @ref spi_mode3_t and @ref msb_first_t /
 * @ref lsb_first_t tag types as additional template parameters of the
 * bus manager to set clock polarity and data order at compile time.
 */
template <typename T>
concept spi_bus = std::is_class_v<T>;

} // namespace hal
