/**
 * @file ctype.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-07-15
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */
#pragma once

#include <stdint.h>

/**
 * @brief Determines whether or not a character
 * is the ASCII representation of a digit.
 * 
 */
// TODO: This implementation is faulty because
// if the input is a function (return value)
// then the function runs twice and can return
// different results.
#define isdigit(x) ((x) >= '0' && (x) <= '9')
/**
 * @brief Determines whether an ASCII character
 * is a letter of the alphabet.
 * 
 */
#define isalpha(x) (('A' <= (x) && (x) <= 'Z') || ('a' <= (x) && (x) <= 'z'))
/**
 * @brief Converts every character to its uppercase equivalent.
 * 
 * @param str Input string
 * @return char* Resulting capitalized string
 */
// TODO: Set this to abide by the ASNI C standard function
char* toupper(char* str);
