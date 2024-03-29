/**
 * @file tss.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#pragma once
#include <Arch/i686/Arch.i686.hpp>
#include <Arch/i686/Assembly/Flush.h>
#include <stdint.h>

/**
 * @brief The Task State Segment (TSS) is a special data structure for x86 processors
 * which holds information about a task. The TSS is primarily suited for hardware
 * multitasking, where each individual process has its own TSS. In Software multitasking,
 * one or two TSS's are also generally used, as they allow for entering
 * Ring 0 code after an interrupt. (OSDev Wiki)
 *
 * Thanks to OSDev Wiki for this section of code.
 *
 */
struct [[gnu::packed]] tss_entry {
    uint32_t    prev;    // The previous TSS - if we used hardware task switching this would form a linked list.
    uint32_t    esp0;    // The stack pointer to load when we change to kernel mode.
    uint32_t    ss0;     // The stack segment to load when we change to kernel mode.
    uint32_t    esp1;    // everything below here is unusued now but required by Intel...
    uint32_t    ss1;
    uint32_t    esp2;
    uint32_t    ss2;
    uint32_t    cr3;
    uint32_t    eip;
    uint32_t    eflags;
    uint32_t    eax;
    uint32_t    ecx;
    uint32_t    edx;
    uint32_t    ebx;
    uint32_t    esp;
    uint32_t    ebp;
    uint32_t    esi;
    uint32_t    edi;
    uint32_t    es;
    uint32_t    cs;
    uint32_t    ss;
    uint32_t    ds;
    uint32_t    fs;
    uint32_t    gs;
    uint32_t    ldt;
    uint16_t    trap;
    uint16_t    iomap_base;
} tss_entry_t;
