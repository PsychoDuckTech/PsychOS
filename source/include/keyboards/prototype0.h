/**
 * Prototype 0 Keyboard Matrix Configuration
 */
#pragma once
#include <Arduino.h>
#include "../include/usbHIDKeyCodes.h" // by MightyPork on GitHub, customized by GabiBrawl
#include "drivers/multiplexer/CD74HC4067.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

extern int colPins[];

// Matrix configuration
int const totalRows = 6;
int const totalCols = 16;
int const usesMultiplexer = true;

// GPIO assignment
int const rowPins[totalRows] = {21, 4, 3, 8, 11, 12}; // PINS THAT WONT WORK: 47, 48, 45, 36, 37, 38, 39, 40, 41, 42, 19, 20
// int const colPins[totalCols] = {22, 23, 24, 25, 27, 28, 44, 43, 31, 32, 33, 34, 35, 36, 37}; // DONT USE 14, 16, 15, 46, 1, 17, 30

// 16bit multiplexer pins and instance
#define MULTIPLEXER_S0 5
#define MULTIPLEXER_S1 6
#define MULTIPLEXER_S2 7
#define MULTIPLEXER_S3 15
#define MULTIPLEXER_SIG 35

Multiplexer colPinsMultiplexer(MULTIPLEXER_S0, MULTIPLEXER_S1, MULTIPLEXER_S2, MULTIPLEXER_S3, MULTIPLEXER_SIG);

// LAYER 0
// Matrix Key Map
const uint8_t keyMapL0[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_SYSRQ, KEY_INSERT, KEY_DELETE},                        // done
    {KEY_BACKSLASH, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_APOSTROPHE, 0, KEY_BACKSPACE, 0, KEY_HOME},                                   // missing one key
    {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_PLUS_ASTERISK, KEY_ACUTE_GRAVE, 0, 0, KEY_END},                                     // done, one empty key
    {KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_C_CEDILLA, KEY_ORDINAL_MASCULINE, KEY_CIRCUMFLEX_TILDE, KEY_ENTER, 0, KEY_PAGEUP},    // done
    {KEY_LEFTSHIFT, KEY_LESS_GREATER, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_MINUS_UNDERSCORE, KEY_RIGHT_SHIFT, 0, KEY_UP, KEY_PAGEDOWN}, // done
    {KEY_LEFTCTRL, KEY_LEFTMETA, 0, KEY_LEFTALT, 0, 0, KEY_SPACE, 0, 0, 0, KEY_RIGHTALT, 0, KEY_RIGHTCTRL, KEY_LEFT, KEY_DOWN, KEY_RIGHT}                                   // done
};

// Matrix Key Name
const char *keyNameL0[totalRows][totalCols] = {
    {"ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "SYSRQ", "INSERT", "DELETE"},
    {"BACKSLASH", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "APOSTROPHE", 0, "BACKSPACE", 0, "HOME"},
    {"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "EQUAL", "GRAVE", 0, 0, "END"},
    {"CAPSLOCK", "A", "S", "D", "F", "G", "H", "J", "K", "L", "C_CEDILLA", "ORDINAL_MASCULINE", "CIRCUMFLEX_TILDE", "ENTER", 0, "PAGEUP"},
    {"LEFTSHIFT", "LESS_GREATER", "Z", "X", "C", "V", "B", "N", "M", "COMMA", "DOT", "MINUS", "RIGHT_SHIFT", 0, "UP", "PAGEDOWN"},
    {"LEFTCTRL", "LEFTMETA", 0, "LEFTALT", 0, 0, "SPACE", 0, 0, 0, "RIGHTALT", 0, "RIGHTCTRL", "LEFT", "DOWN", "RIGHT"}};

// LAYER 1