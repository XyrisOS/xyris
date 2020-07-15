/**
 * @file assert.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-07-14
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#ifndef PANIX_ASSERT_HPP
#define PANIX_ASSERT_HPP

#define assert(x) (!(x) ? (PANIC("Assert failed at ")) : (void)0)

#endif /* PANIX_ASSERT_HPP */