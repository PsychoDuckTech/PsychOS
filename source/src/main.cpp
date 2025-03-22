/**
 * Prototype 0 Keyboard Matrix Configuration Refresh
 */
#include "main.h"

// Define the global constants
const char *OS_version = "0.3.0b ";
const char *byCompany = "by PsychoDuck Tech ";
const char *PRODUCT_NAME = "Kibodo one"; // In Japanese, a keyboard is called "キーボード" (kiiboodo)
#define USE_PT_PT

// Matrix Configuration
const int totalRows = 6, totalCols = 16;
const int rowPins[totalRows] = {21, 4, 14, 10, 11, 9};
const int colPins[] = {/* Add the actual pin numbers here */};

const int usesMultiplexer = true;
const int MULTIPLEXER_S0 = 5, MULTIPLEXER_S1 = 6, MULTIPLEXER_S2 = 7, MULTIPLEXER_S3 = 15, MULTIPLEXER_SIG = 35;
Multiplexer colPinsMultiplexer(MULTIPLEXER_S0, MULTIPLEXER_S1, MULTIPLEXER_S2, MULTIPLEXER_S3, MULTIPLEXER_SIG);

// Key Mapping (Layer 0)
const uint8_t keyMapL0[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, HID_KEY_SYSREQ_ATTENTION, KEY_INSERT, KEY_DELETE},
    {HID_KEY_BACKSLASH, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_APOSTROPHE, 0, KEY_BACKSPACE, 0, KEY_HOME},
    {KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, 0, HID_KEY_GRAVE, 0, 0, KEY_END},
    {KEY_CAPS_LOCK, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, 0, 0, HID_KEY_GRAVE, HID_KEY_ENTER, 0, KEY_PAGE_UP},
    {KEY_LEFT_SHIFT, 0, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, HID_KEY_N, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_MINUS, KEY_RIGHT_SHIFT, 0, KEY_UP_ARROW, KEY_PAGE_DOWN},
    {HID_KEY_CONTROL_LEFT, KEY_LEFT_GUI, 0, KEY_LEFT_ALT, 0, 0, HID_KEY_SPACE, 0, 0, 0, HID_KEY_ALT_RIGHT, 0, HID_KEY_CONTROL_RIGHT, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW}};

const char *keyNameL0[totalRows][totalCols] = {
    {"ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "SYSRQ", "INSERT", "DELETE"},
    {"BACKSLASH", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "APOSTROPHE", 0, "BACKSPACE", 0, "HOME"},
    {"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "EQUAL", "GRAVE", 0, 0, "END"},
    {"CAPSLOCK", "A", "S", "D", "F", "G", "H", "J", "K", "L", "C_CEDILLA", "ORDINAL_MASCULINE", "CIRCUMFLEX_TILDE", "ENTER", 0, "PAGEUP"},
    {"LEFTSHIFT", "LESS_GREATER", "Z", "X", "C", "V", "B", "N", "M", "COMMA", "DOT", "MINUS", "RIGHT_SHIFT", 0, "UP", "PAGEDOWN"},
    {"LEFTCTRL", "LEFTMETA", 0, "LEFTALT", 0, 0, "SPACE", 0, 0, 0, "RIGHTALT", 0, "RIGHTCTRL", "LEFT", "DOWN", "RIGHT"}};

void setup()
{
    delay(0); // Wait for Serial to initialize

    const int serialBaudRate = 115200;
    Serial.begin(serialBaudRate);

    char versionString[50];
    snprintf(versionString, sizeof(versionString), "%s, %s", OS_version, byCompany);
    Serial.println(versionString);

    initializeMatrix();

    startClockTask();
    startDisplayTask();
    startMatrixScanTask();                // Core 0, priority 3
    WPMCounter::startWPMTask(0, 2048, 1); // Adjust core, stackDepth, and priority as needed.
    startKnobHandlerTask();
    startBleTask(1, 16384, 1);
    startSerialTask();
    startHostCommTask();
    startRgbTask();
    startBuzzerTask();

    // RGB configuration
    uRGB.setMaxBrightness(50);
    uRGB.color1("#1e130c");
    uRGB.color2("#9a8478");
    uRGB.color3("remove");
    uRGB.effect(RGB_EFFECT_SCROLL);
    uRGB.speed(15);
    uRGB.brightness(60);
    // uRGB.event(RGB_EVENT_MODULE_CONNECT); // Trigger connect event (commented out)
}

void loop() {} // FreeRTOS handles tasks