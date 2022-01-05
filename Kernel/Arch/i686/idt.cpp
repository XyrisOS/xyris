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
    struct Gate* gate = &idt[n];
    union Offset offset = { .value = handler_addr };
    gate->offset_low = offset.section.low;
    gate->selector = {
        .privilege = 0,
        .table = 0,
        .index = 1,
    };
    gate->reserved = 0;
    gate->flags = {
        .type = INTERRUPT_GATE_32_BIT,
        .offset = 0,
        .privilege = 0,
        .present = 1,
    };
    gate->offset_high = offset.section.high;
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
