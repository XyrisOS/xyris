/**
 * @file ctype.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief These functions are used to test characters
 * for membership in a particular class of characters,
 * such as alphabetic characters, control characters, etc.
 * -- Wikipedia
 * @version 0.3
 * @date 2020-07-15
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#include <lib/ctype.hpp>

char* toupper(char* str) {
    for(char* p=str; *p != '\0'; p++) {
        if(*p >= 'a' && *p <= 'z') {
            *p -= 32;
        }
    }
    return str;
}