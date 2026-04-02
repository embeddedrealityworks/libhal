#pragma once

#include <libhal/utils/units.hpp>

namespace hal {
using namespace hal::units;

/**
 * @brief CRTP base for Reset and Clock Control (RCC) peripherals.
 *
 * RCC is highly MCU-specific in its register layout, but this base captures
 * the generic operations that all MCU clock controllers share.
 *
 * The @p P template parameter on enable/disable/reset is a compile-time
 * peripheral tag (e.g. the GPIO peripheral instance type).  Concrete MCU
 * drivers specialize or constrain it via @c requires clauses.
 *
 * @tparam Derived Concrete RCC driver (e.g. @c hal::stm32f4::rcc).
 *
 * @par Usage
 * @code{.cpp}
 * class my_rcc : public hal::rcc<my_rcc> {
 *   friend class hal::rcc<my_rcc>;
 *
 *   template<typename P> void enable_clock_impl();
 *   template<typename P> void disable_clock_impl();
 *   template<typename P> void reset_impl();
 *
 *   freq sysclk_impl()  const;
 *   freq hclk_impl()    const;
 *   freq pclk1_impl()   const;
 *   freq pclk2_impl()   const;
 * };
 * @endcode
 */
template<typename Derived>
class rcc {
public:
  /// @brief Enable the bus clock for peripheral @p P.
  /// @tparam P Compile-time peripheral tag type.
  template<typename P>
  void enable_clock(this auto& self) {
    self.template enable_clock_impl<P>();
  }

  /// @brief Disable the bus clock for peripheral @p P.
  /// @tparam P Compile-time peripheral tag type.
  template<typename P>
  void disable_clock(this auto& self) {
    self.template disable_clock_impl<P>();
  }

  /// @brief Assert then release the peripheral reset for @p P.
  /// @tparam P Compile-time peripheral tag type.
  template<typename P>
  void reset(this auto& self) {
    self.template reset_impl<P>();
  }

  /// @brief Query the system core clock frequency (SYSCLK).
  /// @return SYSCLK in Hz.
  [[nodiscard]] freq sysclk(this auto& self) {
    return self.sysclk_impl();
  }

  /// @brief Query the AHB clock frequency (HCLK).
  /// @return HCLK in Hz.
  [[nodiscard]] freq hclk(this auto& self) {
    return self.hclk_impl();
  }

  /// @brief Query the APB1 peripheral clock frequency.
  /// @return PCLK1 in Hz.
  [[nodiscard]] freq pclk1(this auto& self) {
    return self.pclk1_impl();
  }

  /// @brief Query the APB2 peripheral clock frequency.
  /// @return PCLK2 in Hz.
  [[nodiscard]] freq pclk2(this auto& self) {
    return self.pclk2_impl();
  }

protected:
  rcc()  = default;
  ~rcc() = default;
};

} // namespace hal
