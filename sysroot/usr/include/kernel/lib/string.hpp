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

#include <sys/types.hpp>

// String functions
int strlen(const char* s);
void strcpy(const char* source, char* destination);
char* strcat(const char *s1, const char *s2);
char* to_upper(char* string);
void reverse(char* s);
void itoa(int n, char str[]);
// Memory functions
void* memset(void* bufptr, int value, size_t size);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memcpy(void* dstptr, const void* srcptr, size_t size);

#endif /* PANIX_LIB_STRING_HPP */