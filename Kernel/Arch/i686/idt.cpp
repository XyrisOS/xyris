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
#include <Arch/i686/regs.hpp>

#define ARCH_IDT_MAX_ENTRIES 256

namespace IDT {

struct Gate idt[ARCH_IDT_MAX_ENTRIES];
struct Registers::IDTR idtr;

void setGate(int n, uint32_t handler_addr)
{
    idt[n].low_offset = (uint16_t)((handler_addr) & 0xFFFF);
    idt[n].selector = {
        .privilege = 0,
        .table = 0,
        .index = 1,
    };
    idt[n].reserved = 0;
    idt[n].flags = {
        .type = INTERRUPT_GATE_32_BIT,
        .offset = 0,
        .privilege = 0,
        .present = 1,
    };
    idt[n].high_offset = (uint16_t)(((handler_addr) >> 16) & 0xFFFF);
}

void init()
{
    // Update the IDT table
    idtr.size = sizeof(idt) - 1;
    idtr.base = (uint32_t)&idt;
    // Load the IDT table into the IDT register
    asm volatile(
        "lidt (%0)"
        :
        : "r"(&idtr)
        : "memory");
}

} // !namespace IDT
