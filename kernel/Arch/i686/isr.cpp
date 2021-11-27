/**
 * @file isr.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include <Arch/Arch.hpp>
#include <Arch/i686/idt.hpp>
#include <Arch/i686/isr.hpp>
#include <sys/Panic.hpp>

// Private array of interrupt handlers
isr_cb_t interrupt_handlers[256];
void (* isr_func_ptr[])(void) = { isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
                                  isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15,
                                  isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
                                  isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31 };
void (* irq_func_ptr[])(void) = { irq0, irq1, irq2, irq3,   irq4,  irq5,  irq6,  irq7,
                                  irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15 };

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_install() {
    // Set all of the gate addresses
    for (int isr = 0; isr < 32; ++isr) {
        idt_set_gate(isr, (uint32_t)isr_func_ptr[isr]);
    }
    // Remap the PIC
    writeByte(0x20, 0x11);
    writeByte(0xA0, 0x11);
    writeByte(0x21, 0x20);
    writeByte(0xA1, 0x28);
    writeByte(0x21, 0x04);
    writeByte(0xA1, 0x02);
    writeByte(0x21, 0x01);
    writeByte(0xA1, 0x01);
    writeByte(0x21, 0x0);
    writeByte(0xA1, 0x0);
    // Install the IRQs
    for (int irq = 0; irq < 16; ++irq) {
        idt_set_gate(32 + irq, (uint32_t)irq_func_ptr[irq]);
    }
    // Load the IDT now that we've registered all of our IDT, IRQ, and ISR addresses
    load_idt();
}

extern "C" void register_interrupt_handler(uint8_t n, isr_cb_t handler) {
    interrupt_handlers[n] = handler;
}

extern "C" void isr_handler(struct registers *r) {
    panic(r);
}

extern "C" void irq_handler(struct registers *regs) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (regs->int_num >= 40) {
        writeByte(0xA0, 0x20);                      /* slave  */
    }
    writeByte(0x20, 0x20);                          /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[regs->int_num] != 0) {
        isr_cb_t handler = interrupt_handlers[regs->int_num];
        handler(regs);
    }
}
