/**
 * @file shell.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-27
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#include <shell/shell.hpp>

void px_shell_print_shell_indicator();
void px_shell_busy_wait_kbd();
char px_shell_key_buff[256] = "\0";
char px_shell_last_comm[256] = "\0";
uint16_t px_shell_len_command = 0;
bool px_shell_is_shifted = false;

void px_shell_init() {
    px_shell_print_shell_indicator();
    px_shell_busy_wait_kbd();
}

void px_shell_print_shell_indicator() {
    px_kprint("basch# ");
}

void px_shell_backspace() {
    if (px_shell_len_command > 0) {
        --px_shell_len_command;
        // Backspace the buffer
        int length = strlen(px_shell_key_buff);
        px_shell_key_buff[length - 1] = '\0';
        // Print the backspace
        char backspace[] = { (char) 0x08, '\0' };
        px_kprint(backspace);
    }
}

void px_shell_scancode_handler(uint8_t scode) {
    // Handle the shift key
    if (scode == PX_SCODE_RIGHT_SHIFT || scode == PX_SCODE_LEFT_SHIFT) {
        px_shell_is_shifted = true;
    } else if (scode == 0xAA || scode == 0xB6) {
        px_shell_is_shifted = false;
    }
    // Handle the up arrow for last command
    if (scode == PX_SCODE_UP_ARROW && strlen(px_shell_last_comm) > 0) {
        while (px_shell_len_command > 0) {
            px_shell_backspace();
            --px_shell_len_command;
        }
        strcpy(px_shell_last_comm, px_shell_key_buff);
        px_shell_len_command = strlen(px_shell_last_comm);
        px_kprint(px_shell_last_comm);
    }
}

void px_shell_shift_handler() {

}

void px_shell_new_line_handler() {
    px_kprint("\n");
    // Add a null char to the end and copy
    px_shell_key_buff[px_shell_len_command] = '\0';
    strcpy(px_shell_key_buff, px_shell_last_comm);
    px_shell_len_command = 0;
    px_shell_key_buff[0] = '\0';
    // Handle command
    /*
    if (this->console != nullptr) {
        handleShellInput(lastCommand);
    }
    */
    if (px_shell_len_command >= 256) {
        px_shell_len_command = 255;
    }
}

void px_shell_special_char_handler(char c) {
    char str[2] = {c, '\0'};
    const char* cStr;
    // Handle special characters
    switch (c) {
        case '`': px_kprint("~"); break;
        case '1': px_kprint("!"); break;
        case '2': px_kprint("@"); break;
        case '3': px_kprint("#"); break;
        case '4': px_kprint("$"); break;
        case '5': px_kprint("%"); break;
        case '6': px_kprint("^"); break;
        case '7': px_kprint("&"); break;
        case '8': px_kprint("*"); break;
        case '9': px_kprint("("); break;
        case '0': px_kprint(")"); break;
        case '-': px_kprint("_"); break;
        case '=': px_kprint("+"); break;
        case '/': px_kprint("?"); break;
        case '.': px_kprint(">"); break;
        case ',': px_kprint("<"); break;
        case '[': px_kprint("{"); break;
        case ']': px_kprint("}"); break;
        default:
            cStr = to_upper(str);
            px_kprint(cStr);
            break;
    }
}

void px_shell_on_key_down(char c) {
    // If we get a newline
    if (c == '\n') {
        px_shell_new_line_handler();
        return;
    }
    // If the shift key boolean is enabled, print the capital version if not a space
    if (px_shell_is_shifted && c != ' ') {
        px_shell_special_char_handler(c);
    // print the lowercase version
    } else {
        const char str[2] = {c, '\0'};
        px_kprint(str);
    }
    // Append the letter to the buffer
    // Move this into the handler
    char str[2] = {c, '\0'};
    concat(px_shell_key_buff, &c);
    ++px_shell_len_command;
}

void px_shell_busy_wait_kbd() {
    // Define a beginning location for our input
    uint8_t start = px_kbd_scode_buff_pos;
    uint8_t buff[256];
    uint8_t scode = px_kbd_scode_buff[px_kbd_scode_buff_pos];
    while (1) {
        // If a scancode is pressed down
        if (scode < 0x80) {
            if (scode == PX_SCODE_BACKSPACE) {
                px_shell_backspace();
            } else {
                // Print the key to the screen
                char key = px_kbd_usa_ascii[(int) scode];
                // If there is no ascii code when we've done all other checks then
                // hand it off to the event handler. This might need to be checked
                // if any weird keyboard bugs come up in the future.
                if (key == '\0') {
                    px_shell_scancode_handler(scode);
                }
                px_shell_on_key_down(key);
            }
        // Else the scancode is released.
        } else {
            px_shell_scancode_handler(scode);
        }
    }
}