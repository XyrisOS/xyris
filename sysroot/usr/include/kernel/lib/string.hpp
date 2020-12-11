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

#include <stdint.h>
#include <stddef.h>

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
void strcpy(char* destination, const char* source);
/**
 * @brief Concatanates source onto destination.
 * 
 * @param dest Destination
 * @param src Source
 * @return char* Pointer to destination
 */
char* strcat(char *dest, const char *src);
/**
 * @brief Reverses the inputted string.
 * 
 * @param s String to be reversed
 */
void reverse(char* s);
/**
 * @brief Converts an integer into its ASCII representation.
 * (This does not have a standard, ANSI implementation.)
 * 
 * @param n Number to be converted to ASCII
 * @param str Buffer to hold result
 */
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
 * @param num Number of bytes
 * @return int Returns 0 if identical. If negative, pointer A is less than
 * than pointer B and vice versa for positive.
 */
int memcmp(const void* ptr1, const void* ptr2, size_t num);
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