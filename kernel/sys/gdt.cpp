/**
 * TODO: Refactor this file a ton or completely rewrite because it was basically taken from toaruOS.
 * @file gdt.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The Global Descriptor Table (GDT) is specific to the IA32 architecture.
 * It contains entries telling the CPU about memory segments.
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */
#include <sys/gdt.hpp>

extern "C" void gdt_flush(uintptr_t);
static void write_tss(int32_t num, uint16_t ss0, uint32_t esp0);

void tss_flush() {
    asm volatile ("mov $0x2B, %ax");
    asm volatile ("ltr %ax");
}

// Code/Data Segment Descriptor
struct gdt_segment {
    // These are descriptors in the GDT that have S=1. Bit 3 of "type" indicates whether it's (0) Data or (1) Code.
    // See https://wiki.osdev.org/Descriptors#Code.2FData_Segment_Descriptors for details on code & data segments.
    uint16_t limit_low;  // Limit
    uint16_t base_low;   // Base
    uint8_t base_high;    // Base
    uint8_t type;       // Type
    uint8_t limit;      // Bits 0..3: limit, Bits 4..7: additional data/code attributes
    uint8_t base_vhi;   // Base
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

static struct {
    gdt_segment entries[6];
    gdt_ptr pointer;
    tss_entry tss;
} gdt __attribute__((used));

void gdt_set_gate(uint8_t num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran) {
    // 32-bit address space
    // Now we have to squeeze the (32-bit) limit into 2.5 regiters (20-bit).
    // This is done by discarding the 12 least significant bits, but this
    // is only legal, if they are all ==1, so they are implicitly still there

    // so if the last bits aren't all 1, we have to set them to 1, but this
    // would increase the limit (cannot do that, because we might go beyond
    // the physical limit or get overlap with other segments) so we have to
    // compensate this by decreasing a higher bit (and might have up to
    // 4095 wasted bytes behind the used memory)
	kprint("Setting a GDT gate.\n");
	/* Base Address */
	gdt.entries[num].base_low = (base & 0xFFFF);
	gdt.entries[num].base_high = (base >> 16) & 0xFF;
	gdt.entries[num].base_vhi = (base >> 24) & 0xFF;
	/* Limits */
	gdt.entries[num].limit_low = (limit & 0xFFFF);
	gdt.entries[num].limit = (limit >> 16) & 0X0F;
	/* Granularity */
	gdt.entries[num].limit |= (gran & 0xF0);
	/* Access flags */
	gdt.entries[num].type = access;
}

void gdt_install() {
	//
	kprint("Installing the GDT onto the system...\n");
	//
	gdt_ptr *gdtp = &gdt.pointer;
	gdtp->limit = sizeof gdt.entries - 1;
	gdtp->base = (uintptr_t)&gdt.entries[0];

	gdt_set_gate(0, 0, 0, 0, 0);                /* NULL segment */
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Code segment */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* Data segment */
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User code */
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User data */

	// Write the TSS, then flush / reload the GDT and TSS
	write_tss(5, 0x10, 0x0);
	gdt_flush((uintptr_t)gdtp);
	kprint("Flushed the GDT.\n");
	tss_flush();
	kprint("Flushed the TSS.\n");
}

static void write_tss(int32_t num, uint16_t ss0, uint32_t esp0) {
	//
	kprint("Writing the TSS...\n");
	//
	tss_entry * tss = &gdt.tss;
	uintptr_t base = (uintptr_t)tss;
	uintptr_t limit = base + sizeof *tss;

	/* Add the TSS descriptor to the GDT */
	gdt_set_gate(num, base, limit, 0xE9, 0x00);

	memset(tss, 0x0, sizeof *tss);

	tss->ss0 = ss0;
	tss->esp0 = esp0;
	tss->cs = 0x0b;
	tss->ss = 0x13;
	tss->ds = 0x13;
	tss->es = 0x13;
	tss->fs = 0x13;
	tss->gs = 0x13;

	tss->iomap_base = sizeof *tss;
}

void set_kernel_stack(uintptr_t stack) {
	kprint("Setting the kernel stack...\n");
	/* Set the kernel stack */
	gdt.tss.esp0 = stack;
}