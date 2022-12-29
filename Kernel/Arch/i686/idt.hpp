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
#include <Arch/i686/Arch.hpp>
#include <stdint.h>

namespace IDT {

enum GateType : uint8_t
{
    TASK_GATE               = 0x5,
    INTERRUPT_GATE_16_BIT   = 0x6,
    TRAP_GATE_16_BIT        = 0x7,
    INTERRUPT_GATE_32_BIT   = 0xE,
    TRAP_GATE_32_BIT        = 0xF,
};

struct [[gnu::packed]] Segment {
    uint8_t privilege   : 2;        // Ring privilege level
    uint8_t table       : 1;        // 1 = LDT entry, 0 = GDT entry
    uint16_t index      : 13;       // Index into table (LDT or GDT)
};
static_assert(sizeof(struct Segment) == 2);

union Offset {
    struct [[gnu::packed]] OffsetSections
    {
        uint16_t low    : 16;
        uint16_t high   : 16;
    } section;
    uint32_t value;
};

struct [[gnu::packed]] Gate {
    uint16_t offset_low     : 16;   // Lower 16 bits of handler function address
    struct Segment selector;        // Kernel segment selector
    uint8_t reserved        : 8;    // Should always be 0
    struct
    {
        enum GateType type  : 4;    // Interrupt or Trap, 16 or 32 bit
        uint8_t offset      : 1;    // Should always be 0
        uint8_t privilege   : 2;    // Rings allowed to access via `INT` instruction (Ignored by hardware interrupts)
        uint8_t present     : 1;    // Gate is present
    } flags;
    uint16_t offset_high    : 16;   // Higher 16 bits of handler function address
};
static_assert(sizeof(struct Gate) == 8);

/**
 * @brief Sets the handler function (via address) for a specific IDT.
 *
 * @param n IDT index
 * @param handler Handler address
 */
void setGate(int n, uint32_t handler);

/**
 * @brief Calls the lidt instruction and installs the IDT onto the CPU.
 *
 */
void init();

} // !namespace IDT
