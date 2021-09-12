/**
 * @file animation.cpp
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief A simple animation for testing duble FrameBuffer
 * @version 0.1
 * @date 2021-07-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <apps/animation.hpp>
#include <dev/graphics/graphics.hpp>

namespace Apps {

void testAnimation()
{
    int x = 10;
    while (1) {
        if (x > 250)
            x = 10;

        Graphics::resetDoubleBuffer();
        //background
        Graphics::putrect(0,0,280,280,0x00FFFF);
        x+=10;
        //snake
        Graphics::putrect(x,10,10,10,0xFF0000);
        //apple
        Graphics::putrect(50,30,10,10,0xFFFF00);
        Graphics::swap();
    }
}

}
