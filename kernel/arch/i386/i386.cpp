#include <arch/i386/i386.hpp>

void px_arch_main()
{
    px_interrupts_disable();
    px_gdt_install();           // Initialize the Global Descriptor Table
    px_isr_install();           // Initialize Interrupt Service Requests
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    // Enable interrupts now that we're out of a critical area
    px_interrupts_enable();
}