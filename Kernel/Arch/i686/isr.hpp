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
#include <Arch/i686/Arch.i686.hpp>
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
typedef void (*isr_cb_t)(struct registers*);

extern "C"
{

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
/* Interrupt Requests */
void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();


/**
 * @brief Handler for the Interrupt Service Request
 *
 * @param r Register information struct
 */
void isr_handler(struct registers* r);

/**
 * @brief
 *
 * @param n
 * @param handler
 */
void register_interrupt_handler(uint8_t n, isr_cb_t handler);

/**
 * @brief
 *
 */
void irq_handler(struct registers* regs);

}

/**
 * @brief
 *
 */
void isr_install();
