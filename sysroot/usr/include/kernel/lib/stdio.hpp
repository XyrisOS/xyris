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

#include <sys/types.hpp>
/**
 * @brief Sends formatted output to a string using an argument list.
 * 
 * @param buf 
 * @param fmt 
 * @param args 
 * @return int 
 */
int vsprintf(char* buf, const char* fmt, va_list args);
/**
 * @brief Sends formatted output to a string.
 * 
 * @param buf 
 * @param fmt 
 * @param ... 
 * @return int 
 */
int sprintf(char* buf, const char* fmt, ...);
/**
 * @brief Sends formatted output to stdout using an argument list.
 * 
 * @param fmt 
 * @param args 
 * @return int 
 */
int vprintf(const char* fmt, va_list args);
/**
 * @brief Sends formatted output to stdout.
 * 
 * @param fmt 
 * @param ... 
 * @return int 
 */
int printf(const char* fmt, ...);