/**
 * @file isr.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-15
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#include <arch/arch.hpp>

// Private array of interrupt handlers
isr_t interrupt_handlers[256];
void (* isr_func_ptr[])(void) = { isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
                                  isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15,
                                  isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23, 
                                  isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31 };
void (* irq_func_ptr[])(void) = { irq0, irq1, irq2, irq3,   irq4,  irq5,  irq6,  irq7,
                                  irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15 };

void px_interrupts_disable() {
    px_print_debug("Disabling interrupts", Warning);
    asm volatile("cli");
}
void px_interrupts_enable() {
    px_print_debug("Enabling interrupts", Warning);
    asm volatile("sti");
}

/* Can't do this with a loop because we need the address
 * of the function names */
void px_isr_install() {
    px_print_debug("Initializing the IDT...", Info);
    // Set all of the gate addresses
    px_print_debug("Setting the ISRs...", Info);
    for (int isr = 0; isr < 32; ++isr) {
        px_idt_set_gate(isr, (uint32_t)isr_func_ptr[isr]);
    }
    px_print_debug("Remapping the PIC...", Info);
    // Remap the PIC
    px_write_byte(0x20, 0x11);
    px_write_byte(0xA0, 0x11);
    px_write_byte(0x21, 0x20);
    px_write_byte(0xA1, 0x28);
    px_write_byte(0x21, 0x04);
    px_write_byte(0xA1, 0x02);
    px_write_byte(0x21, 0x01);
    px_write_byte(0xA1, 0x01);
    px_write_byte(0x21, 0x0);
    px_write_byte(0xA1, 0x0);
    // Install the IRQs
    px_print_debug("Setting the IRQs...", Info);
    for (int irq = 0; irq < 16; ++irq) {
        px_idt_set_gate(32 + irq, (uint32_t)irq_func_ptr[irq]);
    }
    // Load the IDT now that we've registered all of our IDT, IRQ, and ISR addresses
    px_load_idt();
    px_print_debug("Loaded the IDT.", Success);
}

extern "C" void px_register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

extern "C" void px_isr_handler(registers_t *r) {
    PANIC(r);
}

extern "C" void px_irq_handler(registers_t *regs) {
    px_set_indicator(Red);
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (regs->int_num >= 40) 
        px_write_byte(0xA0, 0x20);                      /* slave  */
    px_write_byte(0x20, 0x20);                          /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[regs->int_num] != 0) {
        px_set_indicator(Yellow);
        isr_t handler = interrupt_handlers[regs->int_num];
        handler(regs);
    }
    px_set_indicator(Green);
}