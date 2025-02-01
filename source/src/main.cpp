/**
 * Prototype 0 Keyboard Matrix Configuration Refresh
 */
#include "main.h"

// GPIO assignment
#define MULTIPLEXER_S0 5
#define MULTIPLEXER_S1 6
#define MULTIPLEXER_S2 7
#define MULTIPLEXER_S3 15
#define MULTIPLEXER_SIG 35

Multiplexer colPinsMultiplexer(MULTIPLEXER_S0, MULTIPLEXER_S1, MULTIPLEXER_S2, MULTIPLEXER_S3, MULTIPLEXER_SIG);

// Matrix Configuration
const int totalRows = 6;
const int totalCols = 16;
const int usesMultiplexer = true;
const int rowPins[totalRows] = {21, 4, 3, 8, 11, 12};

// Key Mapping (Layer 0)
const uint8_t keyMapL0[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_SYSRQ, KEY_INSERT, KEY_DELETE},
    {KEY_BACKSLASH, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_APOSTROPHE, 0, KEY_BACKSPACE, 0, KEY_HOME},
    {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_PLUS_ASTERISK, KEY_ACUTE_GRAVE, 0, 0, KEY_END},
    {KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_C_CEDILLA, KEY_ORDINAL_MASCULINE, KEY_CIRCUMFLEX_TILDE, KEY_ENTER, 0, KEY_PAGEUP},
    {KEY_LEFTSHIFT, KEY_LESS_GREATER, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_MINUS_UNDERSCORE, KEY_RIGHT_SHIFT, 0, KEY_UP, KEY_PAGEDOWN},
    {KEY_LEFTCTRL, KEY_LEFTMETA, 0, KEY_LEFTALT, 0, 0, KEY_SPACE, 0, 0, 0, KEY_RIGHTALT, 0, KEY_RIGHTCTRL, KEY_LEFT, KEY_DOWN, KEY_RIGHT}};

const char *keyNameL0[totalRows][totalCols] = {
    {"ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "SYSRQ", "INSERT", "DELETE"},
    {"BACKSLASH", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "APOSTROPHE", 0, "BACKSPACE", 0, "HOME"},
    {"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "EQUAL", "GRAVE", 0, 0, "END"},
    {"CAPSLOCK", "A", "S", "D", "F", "G", "H", "J", "K", "L", "C_CEDILLA", "ORDINAL_MASCULINE", "CIRCUMFLEX_TILDE", "ENTER", 0, "PAGEUP"},
    {"LEFTSHIFT", "LESS_GREATER", "Z", "X", "C", "V", "B", "N", "M", "COMMA", "DOT", "MINUS", "RIGHT_SHIFT", 0, "UP", "PAGEDOWN"},
    {"LEFTCTRL", "LEFTMETA", 0, "LEFTALT", 0, 0, "SPACE", 0, 0, 0, "RIGHTALT", 0, "RIGHTCTRL", "LEFT", "DOWN", "RIGHT"}};

void setup()
{
    delay(1000); // Wait for Serial to initialize
    Serial.begin(115200);
    Serial.println(String(OS_version) + ", " + byCompany);

    initializeMatrix();

    startClockTask();
    startDisplayTask();
    startMatrixScanTask();                // Core 0, priority 3
    WPMCounter::startWPMTask(0, 2048, 1); // Adjust core, stackDepth, and priority as needed.
    startKnobHandlerTask();
    startBleTask();
    startSerialTask();

#ifdef BLE_MASTER
    startHostCommTask();
#elif BLE_SLAVE
    nullptr;
#endif
}

void loop() {} // FreeRTOS handles tasks