/**
 * @file scode.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Contains all of the sets of available keyboard scancodes
 * and their corresponding ASCII values. To be updated later with
 * more keyboard options for other countries.
 * @version 0.1
 * @date 2019-11-18
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_KEYBOARD_SCANCODES_HPP
#define PANIX_KEYBOARD_SCANCODES_HPP

#define PX_SCODE_BACKSPACE 0x0E
#define PX_SCODE_ENTER 0x1C
#define PX_SCODE_LEFT_SHIFT 0x2A
#define PX_SCODE_RIGHT_SHIFT 0x36
#define PX_SCODE_UP_ARROW 0x48

// Any non-ASCII keys should have '\0'
const char px_kbd_usa_ascii[] = {
    '\0', '`', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\0', '?',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', '\0', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '\0', '*',
    '\0', ' '
};
/*
    "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab",
    "Q", "W", "E", "R", "T", "Y", "U", "I",
    "O", "P", "[", "]", "Enter", "Lctrl", "A", "S",
    "D", "F", "G", "H", "J", "K", "L", ";",
    "'", "`", "LShift", "\\", "Z", "X", "C", "V",
    "B", "N", "M", ",", ".", "/", "RShift", "Keypad *",
    "LAlt", "Spacebar"
*/

#endif /* PANIX_KEYBOARD_SCANCODES_HPP */