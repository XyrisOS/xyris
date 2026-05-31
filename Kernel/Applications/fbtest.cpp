/**
 * @file entry.cpp
 * @author Amilie Baker(amiliefn@gmail.com)
 * @brief Simple framebuffer tester.
 * @version 0.2
 * @date 2026-05-25
 *
 * @copyright Copyright the Xyris Contributors (c) 2026
 *
 */

#include <Devices/Graphics/graphics.hpp>
#include <Arch/i686/timer.hpp>
#include <Applications/fbtest.hpp>

namespace Apps {

void fbtest()
{
    while (true) {
        sleep(100);
        Graphics::resetDoubleBuffer();
    }
}
}
