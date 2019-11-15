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

#include <types.hpp>
#include <sys/sys.hpp>
#include <sys/tss.hpp>

extern void gdt_install();
extern void gdt_set_gate(uint8_t num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran);
extern void set_kernel_stack(uintptr_t stack);

#endif /* PANIX_GLOBAL_DESCRIPTOR_TABLE_HPP */