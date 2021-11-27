/**
 * @file idt.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Interrupt Descriptor Table header.
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */

#pragma once
#include <Arch/i686/Arch.i686.hpp>
#include <stdint.h>

/* Segment selectors */
#define KERNEL_CS 0x08
#define IDT_ENTRIES 256

/* How every interrupt gate (handler) is defined */
/* Reference: See mmu.h in XV6 for an alternative to this system where
 * bitfields are used for the uint8_t flag parameter instead of a magic
 * number like we use. */
struct idt_gate {
    uint16_t low_offset;    /* Lower 16 bits of handler function address */
    uint16_t selector;      /* Kernel segment selector */
    uint8_t always0;
    /* First byte
     * Bit 7: "Interrupt is present"
     * Bits 6-5: Privilege level of caller (0=kernel..3=user)
     * Bit 4: Set to 0 for interrupt gates
     * Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
    /** Diagram for flags from OSDev Wiki. See IDT page for details.
     *     7                           0
     *   +---+---+---+---+---+---+---+---+
     *   | P |  DPL  | S |    GateType   |
     *   +---+---+---+---+---+---+---+---+
     */
    uint8_t flags;
    uint16_t high_offset; /* Higher 16 bits of handler function address */
} __attribute__((packed));

/* A pointer to the array of interrupt handlers.
 * Assembly instruction 'lidt' will read it */
struct idt_register {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern struct idt_gate idt[IDT_ENTRIES];
extern struct idt_register idt_reg;

/**
 * @brief Sets the handler function (via address) for a specific IDT.
 *
 * @param n IDT index
 * @param handler Handler address
 */
void idt_set_gate(int n, uint32_t handler);
/**
 * @brief Calls the lidt instruction and installs the IDT onto the CPU.
 *
 */
void load_idt();
