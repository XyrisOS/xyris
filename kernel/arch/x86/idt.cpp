/**
 * @file idt.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-15
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#include <arch/x86/idt.hpp>

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void px_idt_set_gate(int n, uint32_t handler_addr) {
    idt[n].low_offset = (uint16_t)((handler_addr) & 0xFFFF);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; 
    idt[n].high_offset = (uint16_t)(((handler_addr) >> 16) & 0xFFFF);
}

void px_load_idt() {
    px_print_debug("Loading the IDT...", Info);
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    /* Don't make the mistake of loading &idt -- always load &idt_reg */
    asm volatile("lidtl (%0)" : : "r" (&idt_reg));
    px_print_debug("Loaded the IDT.", Success);
}