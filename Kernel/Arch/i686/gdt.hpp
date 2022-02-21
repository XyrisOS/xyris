/**
 * @file gdt.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The Global Descriptor Table (GDT) is specific to the IA32 architecture.
 * It contains entries telling the CPU about memory segments.
 * @version 0.3
 * @date 2019-09-26
 *
 * @copyright Copyright Keeton Feavel (c) 2019
 *
 */
#pragma once
#include <stdint.h>
#include <Arch/i686/Arch.hpp>

namespace GDT {

union Limit {
    struct [[gnu::packed]] LimitSections
    {
        uint16_t low : 16;
        uint8_t high : 4;
    } section;
    uint32_t value;
};

union Base {
    struct [[gnu::packed]] BaseSections
    {
        uint32_t low : 24;
        uint8_t high : 8;
    } section;
    uint32_t value;
};

/**
 * @brief GDT Code & Data Segment Selector Struct
 * See https://wiki.osdev.org/Descriptors#Code.2FData_Segment_Descriptors for details
 *
 */
struct [[gnu::packed]] Entry {
    // Limit
    uint16_t limit_low      : 16;
    // Base
    uint32_t base_low       : 24;
    // Access byte
    uint8_t accessed        : 1;    // Accessed indicator (default to 0)
    uint8_t rw              : 1;    // Readable (code segment) / writeable bit (data segment)
    uint8_t dc              : 1;    // Conforming (code segment) / direction (data segment)
    uint8_t executable      : 1;    // Code (1) or data (0)
    uint8_t system          : 1;    // Task segment (0) or code/data segment (1)
    uint8_t privilege       : 2;    // Privilege level (rings 0-3)
    uint8_t present         : 1;    // Indicates entry is available (default to 1)
    // Limit
    uint8_t limit_high      : 4;
    // Flags
    uint8_t reserved        : 1;    // Reserved (default to 0)
    uint8_t longMode        : 1;    // Indicates a long mode (64-bit) code segment if set
    uint8_t size            : 1;    // Indicates a 32-bit (if set) or 16-bit (if unset) protected mode segment
    uint8_t granulatity     : 1;    // Indicates page granularity if set (otherwise byte granularity)
    // Base
    uint8_t base_high       : 8;
};
static_assert(sizeof(struct Entry) == 8);

/**
 * @brief Setup and install the GDT onto the system.
 *
 */
void init();

} // !namespace GDT
