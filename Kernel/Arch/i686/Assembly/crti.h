/**
 * @file crti.h
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief C/C++ runtime initialization & teardown. See crti.s and crtn.s for details
 * https://wiki.osdev.org/Calling_Global_Constructors
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once

extern "C" {

/**
 * @brief Global constructor
 *
 */
void _init();

/**
 * @brief Global destructor
 *
 */
void _fini();

}
