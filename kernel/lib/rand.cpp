/**
 * @file rand.cpp
 * @author Brian Kernighan, Dennis Ritchie (C Standard Authors)
 * Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Portable implementation of rand and srand as according to the
 * C standard implementation by K&R.
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright C Programming Language copyright Brian Kernighan, Dennis Ritchie
 * Implementation copyright the Panix Contributors (c) 2021.
 *
 * References:
 *     https://wiki.osdev.org/Random_Number_Generator#The_Standard.27s_Example
 *     https://pubs.opengroup.org/onlinepubs/9699919799/functions/rand.html
 *
 */

#include <lib/rand.hpp>

unsigned long _seedrand = 1;

int rand(void)
{
    _seedrand = _seedrand * 214013L + 2531011L;
    return (unsigned int)(_seedrand >> 16) & MAX_RAND;
}

void srand(unsigned int seed)
{
    _seedrand = seed;
}
