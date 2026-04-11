#pragma once

#include <cstdint>

/// @file usart.hpp
/// @brief STM32F4 USART baud-rate helpers and bus-manager type aliases.
///
/// @par Usage
/// @code{.cpp}
/// #include <libhal/buses/usart_bus.hpp>
/// #include <libhal/mcu/stm32f4/usart.hpp>
///
/// using uart1 = hal::usart_bus<mcu::stm32::usart1, "uart1_tx", "uart1_rx">;
/// @endcode

namespace hal::mcu::stm32f4 {

/// @brief Compute USART BRR register value for a given baud rate.
///
/// Formula: BRR = f_ck / (8 × (2 − OVER8) × baud)
/// For OVER8=0 (16× oversampling, default): BRR = f_ck / (16 × baud)
///
/// @param fck_hz   USART source clock frequency in Hz (APB1 or APB2).
/// @param baud     Target baud rate in bps (e.g. 115200).
/// @param over8    @c true for 8× oversampling; @c false (default) for 16×.
/// @return BRR register value.
constexpr std::uint32_t usart_brr(std::uint32_t fck_hz,
                                  std::uint32_t baud,
                                  bool          over8 = false) noexcept {
    return fck_hz / ((over8 ? 8u : 16u) * baud);
}

/// @name Common pre-computed BRR values (APB1 = 42 MHz, 16× oversampling)
/// @{
inline constexpr std::uint32_t usart_brr_9600    = usart_brr(42'000'000u,   9'600u);
inline constexpr std::uint32_t usart_brr_115200  = usart_brr(42'000'000u, 115'200u);
inline constexpr std::uint32_t usart_brr_230400  = usart_brr(42'000'000u, 230'400u);
inline constexpr std::uint32_t usart_brr_921600  = usart_brr(42'000'000u, 921'600u);
/// @}

/// @name Bus instance type aliases
/// Uncomment after including buses/usart_bus.hpp and the stm32 subproject.
/// USART1/6 are on APB2 (84 MHz); USART2/3 and UART4/5 are on APB1 (42 MHz).
/// @{

// using usart1_bus = hal::usart_bus<mcu::stm32::usart1, "usart1_tx", "usart1_rx">;
// using usart2_bus = hal::usart_bus<mcu::stm32::usart2, "usart2_tx", "usart2_rx">;
// using usart3_bus = hal::usart_bus<mcu::stm32::usart3, "usart3_tx", "usart3_rx">;
// using uart4_bus  = hal::usart_bus<mcu::stm32::uart4,  "uart4_tx",  "uart4_rx">;
// using uart5_bus  = hal::usart_bus<mcu::stm32::uart5,  "uart5_tx",  "uart5_rx">;
// using usart6_bus = hal::usart_bus<mcu::stm32::usart6, "usart6_tx", "usart6_rx">;

/// @}

} // namespace hal::mcu::stm32f4
