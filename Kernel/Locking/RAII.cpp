/**
 * @file RAII.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-01-26
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include "RAII.hpp"

RAIIMutex::RAIIMutex(Mutex& mutex)
    : m_Mutex(mutex)
{
    m_Mutex.lock();
}

RAIIMutex::~RAIIMutex()
{
    m_Mutex.unlock();
}
