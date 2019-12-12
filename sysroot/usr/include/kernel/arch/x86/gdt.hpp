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