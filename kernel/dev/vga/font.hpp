/**
 * @file font.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2021-07-25
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 */
#pragma once
#include <dev/vga/framebuffer.hpp>
#include <stdint.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 8

namespace graphics {
namespace font {

/**
 * @brief Draw the character at the given coordinates in the given color.
 * 
 * @param c Character to draw
 * @param x Horizontal position
 * @param y Vertical position
 * @param fore Character hex color
 */
void Draw(char c, uint32_t x, uint32_t y, uint32_t fore);
/**
 * @brief Draw the character at the given coordinates in the given color.
 * 
 * @param c Character to draw
 * @param x Horizontal position
 * @param y Vertical position
 * @param fore Character foreground hex color
 * @param back Character background hex color
 */
void Draw(char c, uint32_t x, uint32_t y, uint32_t fore, uint32_t back);

}
}
