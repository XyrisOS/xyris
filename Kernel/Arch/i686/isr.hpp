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
#include <Arch/i686/Arch.hpp>
#include <stdint.h>

#define ARCH_EXCEPTION_NUM 32           // Hardware exception count
#define ARCH_INTERRUPT_NUM 16           // Hardware interrupt count
#define ARCH_INTERRUPT_HANDLER_MAX 256  // Max number of registered interrupt handlers

namespace Interrupts {

/**
 * @brief Enumerator for mapping hardware exceptions to their purpose
 *
 */
enum Exception {
    EXCEPTION_DIVIDE_BY_ZERO    = 0x00,
    EXCEPTION_DEBUG             = 0x01,
    EXCEPTION_NON_MASK_INT      = 0x02,
    EXCEPTION_BREAKPOINT        = 0x03,
    EXCEPTION_OVERFLOW          = 0x04,
    EXCEPTION_BOUND_RANGE       = 0x05,
    EXCEPTION_INVALID_OPCODE    = 0x06,
    EXCEPTION_DEVICE_UNAVAIL    = 0x07,
    EXCEPTION_DOUBLE_FAULT      = 0x08,
    EXCEPTION_COPROCESSOR_SEG   = 0x09,
    EXCEPTION_INVALID_TSS       = 0x0A,
    EXCEPTION_SEGMENT_MISSING   = 0x0B,
    EXCEPTION_STACK_SEG_FAULT   = 0x0C,
    EXCEPTION_PROTECT_FAULT     = 0x0D,
    EXCEPTION_PAGE_FAULT        = 0x0E,
    // Exception 0xF is reserved
    EXCEPTION_FPU_EXCEPTION     = 0x10,
    EXCEPTION_ALIGNMENT_CHECK   = 0x11,
    EXCEPTION_MACHINE_CHECK     = 0x12,
    EXCEPTION_SIMD_FLOATPOINT   = 0x13,
    EXCEPTION_VIRTUALIZATION    = 0x14,
    // Exceptions 0x15-0x1D are reserved
    EXCEPTION_SECURITY          = 0x1E,
    // Exception 0x1F is reserved
};

/**
 * @brief Enumerator for mapping hardware interrupt values to zero-indexed
 * interrupts for programming ease. Unlike the exceptions, which are defined
 * by the CPU manufacturer, interrupts can have multiple purposes, so it would
 * be counter-productive to name them here.
 *
 */
enum Interrupt {
    INTERRUPT_0     = 0x20,
    INTERRUPT_1     = 0x21,
    INTERRUPT_2     = 0x22,
    INTERRUPT_3     = 0x23,
    INTERRUPT_4     = 0x24,
    INTERRUPT_5     = 0x35,
    INTERRUPT_6     = 0x26,
    INTERRUPT_7     = 0x27,
    INTERRUPT_8     = 0x28,
    INTERRUPT_9     = 0x29,
    INTERRUPT_10    = 0x2A,
    INTERRUPT_11    = 0x2B,
    INTERRUPT_12    = 0x2C,
    INTERRUPT_13    = 0x2D,
    INTERRUPT_14    = 0x2E,
    INTERRUPT_15    = 0x2F,
};

/* Interrupt Service Routines */
typedef void (*InterruptHandler_t)(struct registers*);

/**
 * @brief
 *
 * @param interrupt
 * @param handler
 */
void registerHandler(uint8_t interrupt, InterruptHandler_t handler);

/**
 * @brief
 *
 */
void init();

} // !namespace Interrupts
