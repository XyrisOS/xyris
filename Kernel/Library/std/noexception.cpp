/**
 * @file noexception.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */

namespace std {

/**
 * Replaces the deprecated `bool uncaught_exception()` since C++17.
 * Detects how many exceptions have been thrown or rethrown and not yet
 * entered their matching catch clauses.
 */
int uncaught_exceptions() throw()
{
    return 0;
}

} // !namespace std
