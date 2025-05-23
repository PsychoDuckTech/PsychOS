/**
 * Prototype 0 Keyboard Matrix Configuration Refresh
 */
#include "main.h"
#include "globals.h"

// Define the global constants
const char *OS_version = "0.4.16b ";
const char *byCompany = "by PsychoDuck Tech ";
const char *PRODUCT_NAME = "Kibodo one"; // In Japanese, a keyboard is called "キーボード" (kiiboodo)

// ===== PIN DEFINITIONS =====
// Matrix Configuration
const int totalRows = 6, totalCols = 16;
const int rowPins[totalRows] = {21, 4, 14, 10, 11, 9};
// Multiplexer Pins
#define USES_MULTIPLEXER true
const int MULTIPLEXER_S0 = 5, MULTIPLEXER_S1 = 6, MULTIPLEXER_S2 = 7, MULTIPLEXER_S3 = 15, MULTIPLEXER_SIG = 35;
Multiplexer colPinsMultiplexer(MULTIPLEXER_S0, MULTIPLEXER_S1, MULTIPLEXER_S2, MULTIPLEXER_S3, MULTIPLEXER_SIG);
// Display Pins
const int LCD_CS = 13;
const int LCD_RST = 17;
const int LCD_RS = 16;
const int SPI_MOSI = 12;
const int SPI_SCK = 18;
// Rotary Encoder Pins
const int KNOB_CLK_PIN = 36;
const int KNOB_DT_PIN = 2;
const int KNOB_SW_PIN = 0;
// Buzzer Pin
const int BUZZER_PIN = 8;
// ===== END PIN DEFINITIONS =====

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

NVSUtil nvs("psychos");

// Global variable definitions
bool pixelFlushComplete = false;

void setup()
{
    // Initialize serial communication
    const int serialBaudRate = 115200;
    Serial.begin(serialBaudRate);
    Serial.printf("%s, %s\n", OS_version, byCompany);

    initializeMatrix();

    startClockTask(0, 1024, 1);
    startDisplayTask(1, 4096, 2); // Adjust core, stackDepth, and priority respectively.
    startHostCommTask(1, 4096, 2);
    startMatrixScanTask(0, 8192, 3);
    startKnobHandlerTask(0, 4096, 1);
    WPMCounter::startWPMTask(1, 2048, 1);
    // startBleTask(1, 8192, 2);
    startRgbTask(0, 4096, 2);
    startBuzzerTask(1, 2048, 1);

    uRGB.setMaxBrightness(60); // 100% in the firmware is actually 65% of the maximum output by the LED driver
    RGBConfig config;
    config.effect = RGB_EFFECT_RUNNER;
    config.colors[0] = "#FFA500"; // Orange
    config.colors[1] = "#FF69B4"; // Hot Pink
    config.colors[2] = "#800080"; // Purple
    config.numColors = 3;
    config.speed = 20;      // 1-20 scale
    config.brightness = 45; // 0-100 scale
    uRGB.configure(config);
}

void loop() {}