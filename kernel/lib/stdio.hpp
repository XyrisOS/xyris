/**
 * @file stdio.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-07-09
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */
#pragma once

#include <stdarg.h>

#ifndef EOF
#define EOF (-1)
#endif

#define DBG_INFO "[ \033[37mINFO \033[0m] "
#define DBG_WARN "[ \033[93mWARN \033[0m] "
#define DBG_FAIL "[ \033[91mFAIL \033[0m] "
#define DBG_OKAY "[ \033[92m OK  \033[0m] "

typedef int (*printf_cb_fnptr_t)(unsigned c, void** helper);

/**
 * @brief Perform all printf operations on the format string using the provided
 * argument list and uses the callback function to perform the character printing
 * operation. This allows for adding printf capabilities to a wide range of
 * text output applications, such as an RS232 debug driver, or a framebuffer
 * console.
 *
 * @param fmt Format string
 * @param args Arguments list
 * @param fn Character printing callback function pointer
 * @param ptr User-provided data / handle pointer
 * @return int Returns number of characters written
 */
int printf_helper(const char* fmt, va_list args, printf_cb_fnptr_t fn, void* ptr);

/**
 * @brief Sends formatted output to a string using an argument list.
 *
 * @param buf Pointer to a buffer where the result is stored
 * @param fmt C string that contains a format string
 * @param args A value identifying a variable arguments list
 * @return int The total number of characters written.
 * The number of characters not written if negative.
 */
int kvsprintf(char* buf, const char* fmt, va_list args);

/**
 * @brief Sends formatted output to a string.
 *
 * @param buf Pointer to a buffer where the result is stored
 * @param fmt C string that contains a format string
 * @param ... Sequence of additional arguments
 * @return int The total number of characters written.
 * The number of characters not written if negative.
 */
int ksprintf(char* buf, const char* fmt, ...);

/**
 * @brief Prints a statement to serial debugger if the kernel
 * is built with the debug flag defined. Max message size is
 * 1024 (including null terminator).
 * @param fmt Formatted C string
 * @param ... Sequence of additional arguments
 */
#ifdef DEBUG
#include <dev/serial/rs232.hpp>
#define debugf(fmt, ...) RS232::printf(fmt, ##__VA_ARGS__)
#else
#define debugf(fmt, ...)
#endif
