/**
 * @file Arch.i386.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i386 architecture implementation of Arch.hpp
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <arch/i386/regs.hpp>
#include <arch/i386/gdt.hpp>
#include <arch/i386/idt.hpp>
#include <arch/i386/isr.hpp>
#include <arch/i386/panic.hpp>
#include <arch/i386/ports.hpp>
#include <arch/i386/timer.hpp>
#include <cpuid.h>
#include <stddef.h>
#include <stdint.h>

#define ARCH_PAGE_SIZE  0x1000

namespace Arch {

struct stackframe {
    struct stackframe* ebp;
    size_t eip;
};

}
