/**
 * @file stdlib.hpp
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Standard library
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright Copyright the Panix Contributors (c) 2021
 *
 */
#pragma once
#define MAX_RAND 32767
/**
 * @brief Return a random value 
 * 
 * @param 
 * @return int random value
 */
int rand(void);
/**
 * @brief Set a seed to rand
 * 
 * @param seed a number to set seed
 */
void srand(unsigned int seed);
