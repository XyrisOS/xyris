/**
 * @file isr.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Interrupt Service Routine header.
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#pragma once
#include <arch/i386/arch-i386.hpp>
#include <stdint.h>

/**
 * All of the following values are Interrupt Request (IRQ) identifiers
 * These values start at 32 because there are 32 prior values reserved
 * for processor level exceptions. Look at interrupt.s for the ASM on
 * how the IRQs call functions (and how they pass their value to said
 * function). Each Interrupt Request pushes the IRQ value along with
 * their corresponding hardware interrupt value (starting at 32).
 */
enum isr {
    ISR_DIVIDE_BY_ZERO = 0,
    ISR_DEBUG = 1,
    ISR_NON_MASK_INT = 2,
    ISR_BREAKPOINT = 3,
    ISR_OVERFLOW = 4,
    ISR_BOUND_RANGE = 5,
    ISR_INVALID_OPCODE = 6,
    ISR_DEVICE_UNAVAIL = 7,
    ISR_DOUBLE_FAULT = 8,
    ISR_COPROCESSOR_SEG = 9,
    ISR_INVALID_TSS = 10,
    ISR_SEGMENT_MISSING = 11,
    ISR_STACK_SEG_FAULT = 12,
    ISR_PROTECT_FAULT = 13,
    ISR_PAGE_FAULT = 14,
    // Exception 0xF is reserved
    ISR_FPU_EXCEPTION = 15,
    ISR_ALIGNMENT_CHECK = 16,
    ISR_MACHINE_CHECK = 17,
    ISR_SIMD_FLOATPOINT = 18,
    ISR_VIRTUALIZATION = 19,
    // Exceptions 0x15-0x1D are reserved
    ISR_SECURITY = 30,
    // Exception 0x1F is reserved
};

enum irq {
    IRQ0 = 32,
    IRQ1 = 33,
    IRQ2 = 34,
    IRQ3 = 35,
    IRQ4 = 36,
    IRQ5 = 37,
    IRQ6 = 38,
    IRQ7 = 39,
    IRQ8 = 40,
    IRQ9 = 41,
    IRQ10 = 42,
    IRQ11 = 43,
    IRQ12 = 44,
    IRQ13 = 45,
    IRQ14 = 46,
    IRQ15 = 47,
};

/* Interrupt Service Routines */
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
/* Interrupt Requests */
extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

typedef void (*isr_cb)(struct registers*);

/**
 * @brief
 *
 */
void isr_install();

/**
 * @brief Handler for the Interrupt Service Request
 *
 * @param r Register information struct
 */
extern "C" void isr_handler(struct registers* t);

/**
 * @brief
 *
 * @param n
 * @param handler
 */
extern "C" void register_interrupt_handler(uint8_t n, isr_cb handler);

/**
 * @brief
 *
 */
extern "C" void irq_handler(struct registers* regs);
