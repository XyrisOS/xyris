/**
 * @file rand.cpp
 * @author Brian Kernighan, Dennis Ritchie (C Standard Authors)
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Portable implementation of rand and srand as according to the C standard implementation by K&R.
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright C Programming Language copyright Brian Kernighan, Dennis Ritchie
 * @copyright Implementation Copyright the Xyris Contributors (c) 2021.
 *
 * References:
 *     https://wiki.osdev.org/Random_Number_Generator#The_Standard.27s_Example
 *     https://pubs.opengroup.org/onlinepubs/9699919799/functions/rand.html
 *
 */
#pragma once
#define MAX_RAND 32767

/**
 * @brief Return a random value
 *
 * @return int Value from random number generator
 */
int rand(void);

/**
 * @brief Set a seed to rand
 *
 * @param seed Random number generator seed
 */
void srand(unsigned int seed);
