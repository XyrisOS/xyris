/**
 * @file stdlib.cpp
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Standard library
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright Copyright the Panix Contributors (c) 2021
 *
 */

#include <lib/stdlib.hpp>

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


