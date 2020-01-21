#include <devices/tty/kprint.hpp>
#define IND_X 79
#define IND_Y 0

uint8_t ttyCoordsX = 0;
uint8_t ttyCoordsY = 0;
px_tty_color backColor = Black;
px_tty_color foreColor = White;

void px_print_debug(char* msg, px_print_level lvl) {
    // Reset the color to the default and print the opening bracket
    px_tty_set_color(White, Black);
    px_kprint("[");
    // Change the color and print the tag according to the level
    switch (lvl) {
        case Info:
            px_tty_set_color(LightGrey, Black);
            px_kprint(" INFO ");
            break;
        case Warning:
            px_tty_set_color(Yellow, Black);
            px_kprint(" WARN ");
            break;
        case Error:
            px_tty_set_color(Red, Black);
            px_kprint("FAILED");
            break;
        case Success:
            px_tty_set_color(LightGreen, Black);
            px_kprint("  OK  ");
            break;
        default:
            px_tty_set_color(Magenta, Black);
            px_kprint("UNKNOWN");
            break;
    }
    // Reset the color to the default and print the closing bracket and message
    px_tty_set_color(White, Black);
    px_kprint("] ");
    px_kprint(msg);
    px_kprint("\n");
}

void px_print_raw(char c, uint8_t x, uint8_t y, px_tty_color fg, px_tty_color bg) {
    volatile uint16_t* where;
    uint16_t attrib = (bg << 4) | (fg & 0x0F);
    where = videoMemory + (y * TTY_WIDTH + x);
    *where = c | (attrib << 8);
}

void px_shift_tty_up() {
    // start on the second row
    volatile uint16_t* where = videoMemory + TTY_WIDTH;
    for (size_t row = 1; row < TTY_HEIGHT; ++row) {
        for (size_t col = 0; col < TTY_WIDTH; ++col) {
            // copy the char to the previous row
            *(where - TTY_WIDTH) = *where;
            // increment the pointer
            ++where;
        }
    }
}

void px_kprint(const char* str) {
    volatile uint16_t* where;
    uint16_t attrib = (backColor << 4) | (foreColor & 0x0F);
    // For each character in the string
    for(int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            // Backspace
            case 0x08:
                if (ttyCoordsX > 0) {
                    ttyCoordsX--;
                }
                where = videoMemory + (ttyCoordsY * TTY_WIDTH + ttyCoordsX);
                *where = ' ' | (attrib << 8);
                break;
            // Newline
            case '\n':
                ttyCoordsX = 0;
                ttyCoordsY++;
                break;
            // Anything else
            default:
                where = videoMemory + (ttyCoordsY * TTY_WIDTH + ttyCoordsX);
                *where = str[i] | (attrib << 8);
                ttyCoordsX++;
                break;
        }
        // Move to the next line
        if(ttyCoordsX >= TTY_WIDTH) {
            ttyCoordsX = 0;
            ttyCoordsY++;
        }
        // Clear the screen
        if(ttyCoordsY >= TTY_HEIGHT) {
            px_shift_tty_up();
            where = videoMemory + ((TTY_HEIGHT - 1) * TTY_WIDTH - 1);
            for (size_t col = 0; col < TTY_WIDTH; ++col) {
                *(++where) = ' ' | (attrib << 8);
            }
            ttyCoordsX = 0;
            ttyCoordsY = TTY_HEIGHT - 1;
        }
    }
}

void putchar(char character) {
    px_kprint(&character);
}

void px_kprint_pos(const char* str, uint8_t positionX, uint8_t positionY, bool resetCursor) {
    volatile uint16_t* where;
    uint16_t attrib = (backColor << 4) | (foreColor & 0x0F);
    for(int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            // Backspace
            case 0x08:
                if (positionX > 0) {
                    positionX--;
                }
                where = videoMemory + (positionY * TTY_WIDTH + positionX);
                *where = ' ' | (attrib << 8);
                break;
            // Newline
            case '\n':
                positionX = 0;
                positionY++;
                break;
            // Anything else
            default:
                where = videoMemory + (positionY * TTY_WIDTH + positionX);
                *where = str[i] | (attrib << 8);
                positionX++;
                break;
        }
        // Move to the next line
        if(positionX >= TTY_WIDTH) {
            positionX = 0;
            positionY++;
        }
        // Clear the screen
        if(positionY >= TTY_HEIGHT) {
            px_shift_tty_up();
            where = videoMemory + ((TTY_HEIGHT - 1) * TTY_WIDTH - 1);
            for (size_t col = 0; col < TTY_WIDTH; ++col) {
                *(++where) = ' ' | (attrib << 8);
            }
            positionX = 0;
            positionY = TTY_HEIGHT - 1;
        }
    }
    // If we are told to reset the cursor
    if (resetCursor) {
        ttyCoordsX = 0;
        ttyCoordsY = 0;
    }
}

void px_kprint_base(int value, int base) {
    char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    if ((value / base) != 0 ) {
        px_kprint_base(value / base, base);
    }
    putchar(digits[ value % base ]);
}

void px_kprint_hex(uint32_t key) {
    char* foo = "00000000";
    static const char* hex = "0123456789ABCDEF";
    for (size_t i = 0; i < 8; ++i) {
        foo[7 - i] = hex[(key >> (i * 4)) & 0x0f];
    }
    px_kprint(foo);
}

void px_kprint_color(char* str, px_tty_color color) {
    px_tty_color oldBack = backColor;
    px_tty_color oldFore = foreColor;
    px_tty_set_color(color, backColor);
    px_kprint(str);
    px_tty_set_color(oldFore, oldBack);
}

void px_tty_set_color(px_tty_color fore, px_tty_color back) {
    foreColor = fore;
    backColor = back;
}

void px_clear_tty() {
    char str[] =  { ' ', '\0' };
    for (int y = 0; y < TTY_HEIGHT; y++) {
        for (int x = 0; x < TTY_WIDTH; x++) {
            px_kprint_pos(str, x, y, true);
        }
    }
}

void px_set_indicator(px_tty_color color) {
    px_print_raw(' ', IND_X, IND_Y, color, color);
}