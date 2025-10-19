/**
 * Prototype 0 Keyboard Matrix Configuration Refresh
 */
#include "main.h"
#include "globals.h"

// ESP-IDF specific includes
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "PsychOS";

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

// Key Mapping (Layer 0) - Using ASCII for letters/numbers/space, HID for special keys/modifiers
const uint8_t keyMapL0[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, HID_KEY_SYSREQ_ATTENTION, KEY_INSERT, KEY_DELETE},
    {92, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 39, 0, 8, 0, 0x4A}, // \, 1-0, ', BACKSPACE, HOME (ASCII + HID)
    {9, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 0, 96, 0, 0, 0x4D}, // TAB, q-p, `, END (ASCII + HID)
    {KEY_CAPS_LOCK, 97, 115, 100, 102, 103, 104, 106, 107, 108, 0, 0, 96, HID_KEY_ENTER, 0, 0x4B}, // CAPS, a-l, `, ENTER, PAGEUP (HID + ASCII)
    {0xE1, 0, 122, 120, 99, 118, 98, 110, 109, 44, 46, 45, 0xE5, 0xD4, 0, 0x4E}, // LSHIFT, z-m, , . -, RSHIFT, UP, PAGEDOWN (HID + ASCII)
    {KEY_LEFT_CTRL, KEY_LEFT_GUI, 0, 0xE2, 0, 32, 0, 0, 32, 0, 0xE6, 0, 0xE4, 0xD9, 0xD7, 0xD6}}; // LCTRL, LGUI, LALT, SPACE, RALT, RCTRL, LEFT, DOWN, RIGHT (HID + ASCII)

const char *keyNameL0[totalRows][totalCols] = {
    {"ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "SYSRQ", "INSERT", "DELETE"},
    {"BACKSLASH", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "APOSTROPHE", 0, "BACKSPACE", 0, "HOME"},
    {"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "EQUAL", "GRAVE", 0, 0, "END"},
    {"CAPSLOCK", "A", "S", "D", "F", "G", "H", "J", "K", "L", "C_CEDILLA", "ORDINAL_MASCULINE", "idk what dis is", "ENTER", 0, "PAGEUP"},
    {"LEFTSHIFT", "LESS_GREATER", "Z", "X", "C", "V", "B", "N", "M", "COMMA", "DOT", "MINUS", "RIGHT_SHIFT", "UP", 0, "PAGEDOWN"},
    {"LEFTCTRL", "LEFTMETA", "hid left gui", "LEFTALT", 0, "SPACE", "SPACE", 0, "SPACE", 0, "RIGHTALT", 0, "RIGHTCTRL", "LEFT", "DOWN", "RIGHT"}};

// Global variable definitions
bool pixelFlushComplete = false;

extern "C" void app_main(void)
{
    // Initialize NVS (required for WiFi/BLE)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize serial communication
    Serial.begin(115200);
    ESP_LOGI(TAG, "%s, %s", OS_version, byCompany);
    Serial.printf("%s, %s\n", OS_version, byCompany);

    // Reduce CPU frequency for power savings (handled by sdkconfig)
    // setCpuFrequencyMhz(160); // Not needed in ESP-IDF - set via menuconfig

    initializeMatrix();

    startClockTask(0, 1024, 1);
    startDisplayTask(1, 4096, 2); // Adjust core, stackDepth, and priority respectively.
    startHostCommTask(0, 4096, 2);
    startMatrixScanTask(0, 8192, 3);
    startKnobHandlerTask(0, 4096, 1);
    WPMCounter::startWPMTask(1, 2048, 1);
    startBleTask(1, 8192, 2);
    startRgbTask(0, 4096, 2);
    //startBuzzerTask(1, 2048, 1);

    uRGB.setMaxBrightness(24); // 100% in the firmware is actually 24% of the maximum output by the LED driver
    RGBConfig config;
    config.effect = RGB_EFFECT_RUNNER;
    config.colors[0] = "#FFA500"; // Orange
    config.colors[1] = "#FF69B4"; // Hot Pink
    config.colors[2] = "#800080"; // Purple
    config.numColors = 3;
    config.speed = 20;      // 1-20 scale
    config.brightness = 43; // 0-100 scale
    uRGB.configure(config);
    
    // In ESP-IDF, app_main returns and tasks continue running
    // No loop() needed as FreeRTOS tasks handle everything
}

