/**
 * @file Panic.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Kernel panic management
 * @version 0.1
 * @date 2021-11-05
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once

#include <Arch/Arch.hpp>

/**
 * @brief Halt the system and print the provided message on the panic screen.
 *
 * @param msg Panic message
 */
[[noreturn]] void panic(const char* msg);

/**
 * @brief Halt the system and print the provided message and arguments on the panic screen.
 *
 * @param fmt printf style format panic message
 * @param ... arguments
 */
[[gnu::format(printf, 1, 2)]]
[[noreturn]] void panicf(const char* fmt, ...);

/**
 * @brief Halt the system and print information about the provided register dump.
 *
 * @param registers Architecture register structure
 */
[[noreturn]] void panic(struct registers *registers);
