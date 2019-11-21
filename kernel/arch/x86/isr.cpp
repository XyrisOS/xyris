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

#include <arch/x86/isr.hpp>
// Private array of interrupt handlers
isr_t interrupt_handlers[256];

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
    px_print_debug("Installing the ISR.", Info);
    // Set all of the gate addresses
    // TODO: Add all of the ISRs to a list and make a for loop
    px_idt_set_gate(0, (uint32_t)isr0);
    px_idt_set_gate(1, (uint32_t)isr1);
    px_idt_set_gate(2, (uint32_t)isr2);
    px_idt_set_gate(3, (uint32_t)isr3);
    px_idt_set_gate(4, (uint32_t)isr4);
    px_idt_set_gate(5, (uint32_t)isr5);
    px_idt_set_gate(6, (uint32_t)isr6);
    px_idt_set_gate(7, (uint32_t)isr7);
    px_idt_set_gate(8, (uint32_t)isr8);
    px_idt_set_gate(9, (uint32_t)isr9);
    px_idt_set_gate(10, (uint32_t)isr10);
    px_idt_set_gate(11, (uint32_t)isr11);
    px_idt_set_gate(12, (uint32_t)isr12);
    px_idt_set_gate(13, (uint32_t)isr13);
    px_idt_set_gate(14, (uint32_t)isr14);
    px_idt_set_gate(15, (uint32_t)isr15);
    px_idt_set_gate(16, (uint32_t)isr16);
    px_idt_set_gate(17, (uint32_t)isr17);
    px_idt_set_gate(18, (uint32_t)isr18);
    px_idt_set_gate(19, (uint32_t)isr19);
    px_idt_set_gate(20, (uint32_t)isr20);
    px_idt_set_gate(21, (uint32_t)isr21);
    px_idt_set_gate(22, (uint32_t)isr22);
    px_idt_set_gate(23, (uint32_t)isr23);
    px_idt_set_gate(24, (uint32_t)isr24);
    px_idt_set_gate(25, (uint32_t)isr25);
    px_idt_set_gate(26, (uint32_t)isr26);
    px_idt_set_gate(27, (uint32_t)isr27);
    px_idt_set_gate(28, (uint32_t)isr28);
    px_idt_set_gate(29, (uint32_t)isr29);
    px_idt_set_gate(30, (uint32_t)isr30);
    px_idt_set_gate(31, (uint32_t)isr31);

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
    // TODO: Make a list out of this too
    px_idt_set_gate(32, (uint32_t)irq0);
    px_idt_set_gate(33, (uint32_t)irq1);
    px_idt_set_gate(34, (uint32_t)irq2);
    px_idt_set_gate(35, (uint32_t)irq3);
    px_idt_set_gate(36, (uint32_t)irq4);
    px_idt_set_gate(37, (uint32_t)irq5);
    px_idt_set_gate(38, (uint32_t)irq6);
    px_idt_set_gate(39, (uint32_t)irq7);
    px_idt_set_gate(40, (uint32_t)irq8);
    px_idt_set_gate(41, (uint32_t)irq9);
    px_idt_set_gate(42, (uint32_t)irq10);
    px_idt_set_gate(43, (uint32_t)irq11);
    px_idt_set_gate(44, (uint32_t)irq12);
    px_idt_set_gate(45, (uint32_t)irq13);
    px_idt_set_gate(46, (uint32_t)irq14);
    px_idt_set_gate(47, (uint32_t)irq15);

    // Load the IDT now that we've registered all of our IDT, IRQ, and ISR addresses
    px_load_idt();
}

extern "C" void px_isr_handler(registers_t r) {
    /*
    kprint("Interrupt: ");
    char s[4];
    itoa(r.int_num, s);
    kprint(s);
    kprint("\n");
    kprint(px_exception_descriptions[r.int_num]);
    kprint("\n");
    */
    panic(r.int_num);
}

extern "C" void px_register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

extern "C" void px_irq_handler(registers_t r) {
    //px_print_debug("Don't fear, the IRQ handler is here!", Info);
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (r.int_num >= 40) px_write_byte(0xA0, 0x20);  /* slave */
    px_write_byte(0x20, 0x20);                       /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[r.int_num] != 0) {
        isr_t handler = interrupt_handlers[r.int_num];
        handler(r);
    }
}