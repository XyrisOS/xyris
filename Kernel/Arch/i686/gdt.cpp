/**
 * @file gdt.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The Global Descriptor Table (GDT) is specific to the IA32 architecture.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include <Arch/i686/gdt.hpp>
#include <Arch/i686/Assembly/Flush.h>
#include <Library/string.hpp>

#define ARCH_GDT_MAX_ENTRIES 5

namespace GDT {

struct Entry gdt[ARCH_GDT_MAX_ENTRIES];
struct Registers::GDTR gdtr;

static void setGate(uint8_t num, uint64_t base, uint64_t limit, uint16_t flags)
{
    uint64_t descriptor = 0;

    // Create the high 32 bit segment
    descriptor = base & 0xFF000000;          // base direct map
    descriptor |= (base >> 16) & 0x000000FF; // base 23-16 : 7-0
    descriptor |= (flags << 8) & 0x00F0FF00; // flags 16-11 : 24-19 7-0 : 15-8
    descriptor |= limit & 0x000F0000;        // limit direct map
    // Shift by 32 to move to the lower half of the section
    descriptor <<= 32;
    // Create the low 32 bit segment
    descriptor |= (base << 16) & 0xFFFF0000; // base 15-0 : 31-16
    descriptor |= limit & 0x0000FFFF;        // limit direct map
    // Copy the descriptor value into our GDT entries array
    memcpy(&gdt[num], &descriptor, sizeof(uint64_t));
}

void init()
{
    gdtr.size = sizeof(gdt) - 1;
    gdtr.base = (uint32_t)&gdt;

    setGate(0, 0, 0, 0);                     // Null segment
    setGate(1, 0, 0x000FFFFF, GDT_CODE_PL0); // Kernel code segment
    setGate(2, 0, 0x000FFFFF, GDT_DATA_PL0); // Kernel data segment
    setGate(3, 0, 0x000FFFFF, GDT_CODE_PL3); // User mode code segment
    setGate(4, 0, 0x000FFFFF, GDT_DATA_PL3); // User mode data segment

    gdt_flush((uint32_t)&gdtr);
}

} // !namespace GDT
