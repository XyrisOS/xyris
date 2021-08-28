/**
 * @file panic.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-07-14
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once
#include <arch/i386/arch-i386.hpp>
#include <meta/compiler.hpp>
#include <stdint.h>

/**
 * @brief Halts kernel execution and prints provided info.
 *
 * @param msg Explaination of what happened
 * @param file File causing the issue
 * @param line Line with the error
 * @param func Function containing error
 */
NORET void panic(const char* msg, const char* file, uint32_t line, const char* func);

/**
 * @brief Halts kernel execution and prints register info.
 *
 * @param regs Registers struct
 * @param file File causing the issue
 * @param line Line with the error
 * @param func Function containing error
 */
NORET void panic(struct registers* regs, const char* file, uint32_t line, const char* func);
