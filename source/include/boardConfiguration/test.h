/**
 * Key Array Map for the Prototype 0 Matrix
 */

#include <Arduino.h>
#include "usb_hid_keys.h" // by MightyPork on GitHub

// Matrix configuration
int const totalRows = 6;
int const totalCols = 16;

// Matrix Key Map
const uint8_t keyMap[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1,KEY_F2,KEY_F3,KEY_F4, KEY_F5,KEY_F6,KEY_F7,KEY_F8, KEY_F9,KEY_F10,KEY_F11,KEY_F12, KEY_SYSRQ,KEY_INSERT, KEY_DELETE},
    {KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8,KEY_9, KEY_0, KEY_APOSTROPHE, KEY_EQUAL, KEY_BACKSPACE, KEY_HOME, 0, 0},
    {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U,KEY_I, KEY_O, KEY_P, KEY_LEFTBRACE, KEY_GRAVE, KEY_ENTER, KEY_END, 0},
    {KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J,KEY_K, KEY_L, KEY_SEMICOLON, KEY_HASHTILDE, KEY_BACKSLASH, 0, KEY_PAGEUP, 0},
    {KEY_LEFTSHIFT, KEY_102ND, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, 0, 0, 0, 0},
    {KEY_LEFTCTRL, KEY_LEFTMETA, KEY_LEFTALT, KEY_SPACE, 0, 0, 0,KEY_RIGHTALT, 0, KEY_RIGHTCTRL, 0, 0, 0, 0, 0, 0}
};

// Matrix Key Name
const char* keyName[totalRows][totalCols] = {
    {"ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "SYSRQ", "INSERT", "DELETE"},
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "'", "=", "BACKSPACE", "HOME", "", ""},
    {"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "`", "ENTER", "END", ""},
    {"CAPSLOCK", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "#~", "\\", "", "PAGEUP", ""},
    {"LSHIFT", "102ND", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "", "", "", ""},
    {"LCTRL", "LMETA", "LALT", "SPACE", "", "", "", "RALT", "", "RCTRL", "", "", "", "", "", ""}
};

// GPIO assignment
int const rowPins[totalRows] = {4, 5, 7, 18, 8, 3};
int const colPins[totalCols] = {9, 6, 10, 11, 12, 13, 14, 20, 21, 35, 36, 37, 38, 39, 40, 41};