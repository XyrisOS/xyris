/**
 * @file compiler.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Compiler meta directives
 * @version 0.1
 * @date 2021-06-17
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once

// Function attributes
#define NORET __attribute__((noreturn))
#define OPTIMIZE(x) __attribute__((optimize("O"#x)))
#define ALWAYS_INLINE __attribute__((always_inline))

// Constructors / Destructors
#define CONSTRUCTOR __attribute__ ((constructor))
#define DESTRUCTOR __attribute__ ((destructor))

// Branch prediction
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

// Data attributes
#define USED __attribute__ ((used))
#define PACKED __attribute__ ((__packed__))
#define ALIGN(x) __attribute__ ((aligned ((x))))
