#define IND_X 79
#define IND_Y 0

#include <devices/tty/indicator.hpp>

void px_set_indicator(px_tty_color color) {
    px_print_raw(' ', IND_X, IND_Y, color, color);
}
