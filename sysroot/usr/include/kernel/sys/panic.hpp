/**
 * @file panic.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-07-14
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#ifndef PANIX_PANIC_HPP
#define PANIX_PANIC_HPP

#include <lib/stdint.hpp>    // Data type definitions
#include <arch/arch.hpp>    // Architecture specific features

// Macros for panic and assert
#define PANIC(x) panic((x), __FILE__, __LINE__, __FUNCTION__)

// List of all exceptions and their associated english descriptions
extern const char* px_exception_descriptions[];

/**
 * @brief Halts kernel execution and prints info about an exception.
 * 
 * @param exception Exception number
 */
void panic(int exception);
/**
 * @brief Halts kernel execution and prints provided info.
 * 
 * @param msg Explaination of what happened
 * @param file File causing the issue
 * @param line Line with the error
 * @param func Function containing error
 */
void panic(char* msg, const char *file, uint32_t line, const char *func);
/**
 * @brief Halts kernel execution and prints register info.
 * 
 * @param regs Registers struct
 * @param file File causing the issue
 * @param line Line with the error
 * @param func Function containing error
 */
void panic(registers_t *regs, const char *file, uint32_t line, const char *func);

#endif /* PANIX_PANIC_HPP */
