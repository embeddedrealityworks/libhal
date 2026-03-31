// Bare-metal startup for ARM Cortex-M targets.
// Provides the vector table (.isr_vector) and _reset entry point.
//
// Copies .data from flash, zeroes .bss, runs C++ static constructors,
// then calls main(). On return calls _exit(0).

#include <algorithm>
#include <cstdint>

extern "C" {

// Linker-script symbols
extern std::uint32_t _estack;
extern std::uint32_t __data_start_flash; // LMA of .data in flash
extern std::uint32_t _sdata;             // VMA start of .data in RAM
extern std::uint32_t _edata;             // VMA end of .data in RAM
extern std::uint32_t _sbss;
extern std::uint32_t _ebss;

// C++ static constructor / destructor arrays
extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

// Provided by semihosting.cpp
[[noreturn]] void _exit(int status);

int main();

// --------------------------------------------------------------------------
// Default / weak fault handlers
// --------------------------------------------------------------------------

void Default_Handler() {
  while (true) {
  }
}

// Core ARM Cortex-M exceptions
void NMI_Handler() __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler() __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler() __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler() __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler() __attribute__((weak, alias("Default_Handler")));
void SVC_Handler() __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler() __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, alias("Default_Handler")));

// --------------------------------------------------------------------------
// Vector table — placed in .isr_vector by linker script
// --------------------------------------------------------------------------

using HandlerFn = void (*)();

__attribute__((section(".isr_vector"), used))
const std::uintptr_t vector_table[] = {
    // Initial stack pointer (top of RAM)
    reinterpret_cast<std::uintptr_t>(&_estack),
    // Core exception handlers
    reinterpret_cast<std::uintptr_t>(_reset),
    reinterpret_cast<std::uintptr_t>(NMI_Handler),
    reinterpret_cast<std::uintptr_t>(HardFault_Handler),
    reinterpret_cast<std::uintptr_t>(MemManage_Handler),
    reinterpret_cast<std::uintptr_t>(BusFault_Handler),
    reinterpret_cast<std::uintptr_t>(UsageFault_Handler),
    0,
    0,
    0,
    0, // Reserved
    reinterpret_cast<std::uintptr_t>(SVC_Handler),
    reinterpret_cast<std::uintptr_t>(DebugMon_Handler),
    0, // Reserved
    reinterpret_cast<std::uintptr_t>(PendSV_Handler),
    reinterpret_cast<std::uintptr_t>(SysTick_Handler),
    // Device-specific IRQ slots — linker-script .isr_vector.device section
    // fills these in from the BSP; remainder defaults to 0 (triggers HardFault
    // if an unexpected interrupt fires during simulation).
};

// --------------------------------------------------------------------------
// Reset handler — entry point (ENTRY(_reset) in linker script)
// --------------------------------------------------------------------------

[[noreturn]] void _reset() {
  std::copy(&__data_start_flash, &__data_start_flash + (&_edata - &_sdata), &_sdata);
  std::fill(&_sbss, &_ebss, 0u);
  std::for_each(__preinit_array_start, __preinit_array_end, [](auto fn) { fn(); });
  std::for_each(__init_array_start,    __init_array_end,    [](auto fn) { fn(); });
  _exit(main());
}

} // extern "C"
