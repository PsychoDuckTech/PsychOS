/**
 * Key Array Map for the 2x2 Matrix
 */

#include <Arduino.h>
#include "usb_hid_keys.h" // by MightyPork on GitHub

// Matrix configuration
int const totalRows = 2;
int const totalCols = 2;

// Matrix Key Map
const uint8_t keyMap[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1},
    {KEY_1, KEY_2}
};

// Matrix Key Name
const char* keyName[totalRows][totalCols] = {
    {"ESC", "F1"},
    {"1", "2"}
};

// GPIO assignment
int const rowPins[totalRows] = {4, 5};
int const colPins[totalCols] = {9, 6};