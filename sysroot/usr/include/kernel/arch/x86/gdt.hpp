/**
 * @file GlobalDescriptorTable.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The Global Descriptor Table (GDT) is specific to the IA32 architecture.
 * It contains entries telling the CPU about memory segments.
 * @version 0.1
 * @date 2019-09-26
 * 
 * @copyright Copyright Keeton Feavel (c) 2019
 * 
 */
#ifndef PANIX_GLOBAL_DESCRIPTOR_TABLE_HPP
#define PANIX_GLOBAL_DESCRIPTOR_TABLE_HPP

#include <sys/sys.hpp>
#include <arch/x86/tss.hpp>

// TODO: Do we want to use this for our access? It would be a lot more code...
struct gdt_access {
    uint8_t accessed    : 1; // The CPU sets this to 1 when the segment is accessed
    uint8_t rw          : 1; // Readable for code selectors, writable for data selectors
    uint8_t dir_conf    : 1; // Direction/Conforming bit. See OSDev for details
    uint8_t executable  : 1; // If 1 code in this segment can be executed, otherwise data
    uint8_t desc_type   : 1; // Set for code or data segments and cleared for system segments
    uint8_t privilege   : 2; // Ring level (0 - kernel, 3 - usermode)
    uint8_t present     : 1; // This must be 1 for all valid selectors
} __attribute__((packed));

/**
 * @brief GDT Code & Data Segment Selector Struct
 * 
 */
struct gdt_segment {
    // These are descriptors in the GDT that have S=1. Bit 3 of "type" indicates whether it's (0) Data or (1) Code.
    // See https://wiki.osdev.org/Descriptors#Code.2FData_Segment_Descriptors for details on code & data segments.
    uint16_t limit_low; // Limit
    uint16_t base_low;  // Base
    uint8_t  base_high; // Base
    uint8_t  type;      // Type
    uint8_t  limit;     // Bits 0..3: limit, Bits 4..7: additional data/code attributes
    uint8_t  base_vhi;  // Base
} __attribute__((packed));
/**
 * @brief GDT Pointer Struct
 * 
 */
struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));
/**
 * @brief GDT Struct
 * 
 */
static struct {
    gdt_segment entries[6];
    gdt_ptr pointer;
    tss_entry tss;
} gdt __attribute__((used));

/**
 * @brief Setup and install the GDT onto the system.
 * 
 */
extern void px_gdt_install();
// TODO: Create access byte struct to be more verbose with how the GDT works.
// Reference: https://wiki.osdev.org/Global_Descriptor_Table
/**
 * @brief 
 * 
 * @param num Specifies the index for the GDT entry
 * @param base The linear address where the segment begins
 * @param limit Maximum addressable unit 
 * @param access Access byte. Contains various flags.
 * @param gran Page granularity
 */
extern void px_gdt_set_gate(uint8_t num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran);
/**
 * @brief Set the kernel stack pointer
 * 
 * @param stack Stack pointer address
 */
extern void px_set_kernel_stack(uintptr_t stack);

#endif /* PANIX_GLOBAL_DESCRIPTOR_TABLE_HPP */