/**
 * @file idt.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include <Arch/i686/idt.hpp>

struct idt_gate idt[IDT_ENTRIES];
struct idt_register idt_reg;

void idt_set_gate(int n, uint32_t handler_addr) {
    idt[n].low_offset = (uint16_t)((handler_addr) & 0xFFFF);
    idt[n].selector = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; //    1 -> present bit,
                         //   00 -> ring 0 privilege
                         //    0 -> interrupt/trap gate
                         // 1110 -> type: 32-bit interrupt gate
    idt[n].high_offset = (uint16_t)(((handler_addr) >> 16) & 0xFFFF);
}

void load_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(struct idt_gate) - 1;
    /* Don't make the mistake of loading &idt -- always load &idt_reg */
    asm volatile("lidt (%0)" : : "r" (&idt_reg) : "memory");
}
