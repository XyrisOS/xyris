/**
 * @file tss.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_TASK_STATE_SEGMENT
#define PANIX_TASK_STATE_SEGMENT

#include <types.hpp>

extern void tss_flush();

/**
 * @brief The Task State Segment (TSS) is a special data structure for x86 processors 
 * which holds information about a task. The TSS is primarily suited for hardware 
 * multitasking, where each individual process has its own TSS. In Software multitasking, 
 * one or two TSS's are also generally used, as they allow for entering 
 * Ring 0 code after an interrupt. (OSDev Wiki)
 * 
 * Thanks to toaruOS for this section of code.
 * 
 */

typedef struct tss_entry {
	uint32_t	prev;
	uint32_t	esp0;
	uint32_t	ss0;
	uint32_t	esp1;
	uint32_t	ss1;
	uint32_t	esp2;
	uint32_t	ss2;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax;
	uint32_t	ecx;
	uint32_t	edx;
	uint32_t	ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint32_t	es;
	uint32_t	cs;
	uint32_t	ss;
	uint32_t	ds;
	uint32_t	fs;
	uint32_t	gs;
	uint32_t	ldt;
	uint16_t	trap;
	uint16_t	iomap_base;
} __attribute__ ((packed)) tss_entry_t;

#endif /* PANIX_TASK_STATE_SEGMENT */