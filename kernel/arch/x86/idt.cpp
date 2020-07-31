/**
 * @file idt.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2019-11-15
 * 
 * @copyright Copyright the Panix Contributors (c) 2019
 * 
 */

#include <arch/arch.hpp>
#include <lib/stdio.hpp>
#include <dev/tty/tty.hpp>

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void px_idt_set_gate(int n, uint32_t handler_addr) {
    idt[n].low_offset = (uint16_t)((handler_addr) & 0xFFFF);
    idt[n].selector = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; //    1 -> present bit,
                         //   00 -> ring 0 privilege
                         //    0 -> interrupt/trap gate
                         // 1110 -> type: 32-bit interrupt gate
    idt[n].high_offset = (uint16_t)(((handler_addr) >> 16) & 0xFFFF);
}

void px_load_idt() {
    px_kprintf(DBG_INFO "Loading the IDT...\n");
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    /* Don't make the mistake of loading &idt -- always load &idt_reg */
    asm volatile("lidtl (%0)" : : "r" (&idt_reg));
    px_kprintf(DBG_OKAY "Loaded the IDT.\n");
}