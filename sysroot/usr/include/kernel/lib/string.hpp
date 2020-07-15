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

/**
 * @brief 
 * 
 * @param s 
 * @return int 
 */
int strlen(const char* s);
/**
 * @brief 
 * 
 * @param source 
 * @param destination 
 */
void strcpy(const char* source, char* destination);
/**
 * @brief 
 * 
 * @param s1 
 * @param s2 
 * @return char* 
 */
char* strcat(const char *s1, const char *s2);
/**
 * @brief 
 * 
 * @param string 
 * @return char* 
 */
char* to_upper(char* string);
/**
 * @brief 
 * 
 * @param s 
 */
void reverse(char* s);
/**
 * @brief 
 * 
 * @param n 
 * @param str 
 */
void itoa(int n, char str[]);
/**
 * @brief 
 * 
 * @param bufptr 
 * @param value 
 * @param size 
 * @return void* 
 */
void* memset(void* bufptr, int value, size_t size);
/**
 * @brief 
 * 
 * @param aptr 
 * @param bptr 
 * @param size 
 * @return int 
 */
int memcmp(const void* aptr, const void* bptr, size_t size);
/**
 * @brief 
 * 
 * @param dstptr 
 * @param srcptr 
 * @param size 
 * @return void* 
 */
void* memmove(void* dstptr, const void* srcptr, size_t size);
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