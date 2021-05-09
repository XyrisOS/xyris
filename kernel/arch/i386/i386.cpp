#include <arch/i386/i386.hpp>

void px_arch_init()
{
    px_interrupts_disable();
    px_gdt_install();           // Initialize the Global Descriptor Table
    px_isr_install();           // Initialize Interrupt Service Requests
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    // Enable interrupts now that we're out of a critical area
    px_interrupts_enable();
}

namespace Memory {

inline void SetPageDirectory(size_t pageDir) {
    asm volatile("mov %0, %%cr3" :: "b"(pageDir));
}

inline void PagingEnable() {
    size_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    // 0x80000000 = 0b10000000000000000000000000000000
    // The most significant bit signifies whether to
    // enable or disable paging within control register 0.
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));
}

inline void PagingDisable() {
    size_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    // 0x80000000 = 0b10000000000000000000000000000000
    // The most significant bit signifies whether to
    // enable or disable paging within control register 0.
    // In this case we set the opposite (~) so the result
    // is 0b01111111111111111111111111111111
    cr0 &= ~(0x80000000U);
    asm volatile("mov %0, %%cr0":: "b"(cr0));
}

} // namespace Memory
