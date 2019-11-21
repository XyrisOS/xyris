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
extern bool px_gdt_install();
/**
 * @brief 
 * 
 * @param num 
 * @param base 
 * @param limit 
 * @param access 
 * @param gran 
 */
extern void px_gdt_set_gate(uint8_t num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran);
/**
 * @brief Set the kernel stack pointer
 * 
 * @param stack 
 */
extern void px_set_kernel_stack(uintptr_t stack);

#endif /* PANIX_GLOBAL_DESCRIPTOR_TABLE_HPP */