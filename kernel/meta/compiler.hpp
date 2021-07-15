/**
 * @file compiler.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Compiler meta directives
 * @version 0.1
 * @date 2021-06-17
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 */
#pragma once

// Function attributes
#define NORET __attribute__((noreturn))
#define ALWAYS_INLINE __attribute__((always_inline))

// Branch prediction
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
