/**
 * @file New.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Placement new
 * @version 0.1
 * @date 2022-01-25
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <stddef.h>

inline void *operator new(size_t, void *p) { return p; }
inline void *operator new[](size_t, void *p) { return p; }
inline void  operator delete  (void *, void *) { };
inline void  operator delete[](void *, void *) { };
