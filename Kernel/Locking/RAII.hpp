/**
 * @file RAII.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Resource Acquisition Is Initialization mutex. Locks when constructed
 * and unlocks when destructed.
 * @version 0.1
 * @date 2022-01-26
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once

#include "Mutex.hpp"

class RAIIMutex {
public:
    /**
     * @brief Construct a new RAIIMutex object that locks when
     * constructed and unlocks when destructed.
     *
     * @param mutex Mutex to use for RAII (un)locking
     *
     */
    RAIIMutex(Mutex& mutex);

    /**
     * @brief Destroy the RAIIMutex object and unlock the mutex
     *
     */
    ~RAIIMutex();

private:
    Mutex& m_Mutex;
};

/**
 * @brief Mutex protected region lambda function. Executed the provided
 * function after locking the provided mutex. Unlocks before returning.
 * Provided lambda function cannot return any value, but may return at
 * any point safely.
 *
 * @tparam Function Lambda function template
 * @param lockedWork Function to be executed in a safe manner
 * @param mutex Mutex to lock before lambda execution
 */
template<typename Function>
void lockedRegion(Function lockedWork, Mutex& mutex)
{
    RAIIMutex raiiMutex(mutex);
    lockedWork();
}
