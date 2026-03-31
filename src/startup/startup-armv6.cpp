#include <algorithm>
#include <cstdint>

// Linker-provided symbols
extern "C" std::uint32_t _estack;
extern "C" std::uint32_t __data_start_flash;
extern "C" std::uint32_t _sdata;
extern "C" std::uint32_t _edata;
extern "C" std::uint32_t _sbss;
extern "C" std::uint32_t _ebss;

extern int main();

extern "C" void _reset();
extern "C" void _default();

extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

// ARMv6-M (Cortex-M0/M0+) core exception handlers.
// MemManage (4), BusFault (5), UsageFault (6), DebugMon (12) do not
// exist on ARMv6-M; those vector table slots must be 0.
extern "C" void NMI_Handler()
    __attribute__((weak, alias("_default")));
extern "C" void HardFault_Handler()
    __attribute__((weak, alias("_default")));
extern "C" void SVC_Handler()
    __attribute__((weak, alias("_default")));
extern "C" void PendSV_Handler()
    __attribute__((weak, alias("_default")));
extern "C" void SysTick_Handler()
    __attribute__((weak, alias("_default")));

// ARMv6-M core vector table (positions 0-15).
// Device IRQs (positions 16+) contributed via .isr_vector.device.
extern "C" const volatile std::uintptr_t vector_table[]
    __attribute__((section(".isr_vector"), used)) = {
        reinterpret_cast<std::uintptr_t>(&_estack),          // 0: initial SP
        reinterpret_cast<std::uintptr_t>(&_reset),           // 1: Reset
        reinterpret_cast<std::uintptr_t>(&NMI_Handler),      // 2: NMI
        reinterpret_cast<std::uintptr_t>(&HardFault_Handler),// 3: HardFault
        0,                                                    // 4: reserved
        0,                                                    // 5: reserved
        0,                                                    // 6: reserved
        0,                                                    // 7: reserved
        0,                                                    // 8: reserved
        0,                                                    // 9: reserved
        0,                                                    // 10: reserved
        reinterpret_cast<std::uintptr_t>(&SVC_Handler),      // 11: SVCall
        0,                                                    // 12: reserved
        0,                                                    // 13: reserved
        reinterpret_cast<std::uintptr_t>(&PendSV_Handler),   // 14: PendSV
        reinterpret_cast<std::uintptr_t>(&SysTick_Handler),  // 15: SysTick
};

extern "C" void _reset() {
  std::copy(&__data_start_flash,
            &__data_start_flash + (&_edata - &_sdata),
            &_sdata);
  std::fill(&_sbss, &_ebss, 0u);
  std::for_each(__init_array_start, __init_array_end, [](auto fn) { fn(); });

  main();

  while (true) {}
}

extern "C" void _default() {
  while (true) {}
}
