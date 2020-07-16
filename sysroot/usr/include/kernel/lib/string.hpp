/**
 * @file string.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard string and memory utility library
 * @version 0.3
 * @date 2020-06-17
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */

#ifndef PANIX_LIB_STRING_HPP
#define PANIX_LIB_STRING_HPP

#include <lib/stdint.hpp>
#include <lib/stddef.hpp>

/**
 * @brief Returns the length of a string.
 * 
 * @param s Input string
 * @return int Length of string
 */
int strlen(const char* s);
/**
 * @brief Copys a string from the source to the destination.
 * 
 * @param source String to be copied
 * @param destination Location where string will be copied
 */
void strcpy(const char* source, char* destination);
/**
 * @brief Concatanates string one onto string two.
 * 
 * @param s1 String one
 * @param s2 String two
 * @return char* Resulting concatenated string 
 */
char* strcat(const char *s1, const char *s2);
/**
 * @brief Reverses the inputted string.
 * 
 * @param s String to be reversed
 */
void reverse(char* s);
/**
 * @brief Converts an integer into its ASCII representation.
 * 
 * @param n Number to be converted to ASCII
 * @param str Buffer to hold result
 */
// TODO: Set this to abide by the ASNI C standard function
void itoa(int n, char str[]);
/**
 * @brief Sets the number of bytes in memory at ptr to the value.
 * 
 * @param ptr Pointer to location in memory
 * @param value Value to be written in memory
 * @param num Number of bytes
 * @return void* Pointer to location in memory
 */
void* memset(void* bufptr, int value, size_t num);
/**
 * @brief Compares a given number of bytes in memory at pointer A to pointer B.
 * 
 * @param ptr1 Source pointer
 * @param ptr2 Destination pointer
 * @param size Number of bytes
 * @return int Returns 0 if identical. If negative, pointer A is less than
 * than pointer B and vice versa for positive.
 */
// TODO: Fix the return type to be ANSI compliant
int memcmp(const void* ptr1, const void* ptr2, size_t size);
/**
 * @brief Moves a given number of bytes from the source to the destination.
 * 
 * @param destination Destination pointer
 * @param source Source pointer
 * @param size Number of bytes
 * @return void* Pointer to the destination
 */
void* memmove(void* destination, const void* source, size_t size);
/**
 * @brief 
 * 
 * @param dstptr 
 * @param srcptr 
 * @param size 
 * @return void* 
 */
void* memcpy(void* dstptr, const void* srcptr, size_t size);

#endif /* PANIX_LIB_STRING_HPP */