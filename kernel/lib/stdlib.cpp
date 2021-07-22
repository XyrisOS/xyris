/**
 * @file stdlib.cpp
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Standard library, based code from the book "The Standard C Library" - ISBN-10: 0131315099  
 *
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright 
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


