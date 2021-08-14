/**
 * @file puts.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-08
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */

#include <lib/stdio.hpp>

int puts(const char *str) {
    int i = 0;
    // Loops until a null character
    while(str[i]) {
        if(putchar(str[i]) == EOF) {
            return EOF;
        }
        i++;
    }
    if(putchar('\n') == EOF) {
       return EOF;
    }
    // Follow POSIX spec
    return 1;
}
