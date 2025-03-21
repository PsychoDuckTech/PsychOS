#pragma once

#define BG_COLOR 0x0
#define TEXT_COLOR 0xD5F1 // 0x95E5,0xBD6C (old 0xDED9)
#define SUCCESS_COLOR 0x9C1F
#define ERROR_COLOR 0xF22B
#define MUTED_COLOR 0x7BEF
#define HIGHLIGHT_COLOR 0xFD40
#define SELECTED_COLOR 0x2945

#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern bool capsLockStatus;
extern int hours;
extern int minutes;
extern int seconds;

extern int settingsSelectedOption;
extern bool inSettingsSubmenu;

extern int rgbValues[4];

struct RGBState
{
    uint8_t currentSelection; // 0-3: Red, Green, Blue, Brightness
    uint8_t values[4];        // [Red, Green, Blue, Brightness%]
    bool needsRefresh;        // Flag for partial updates
};
extern int firstDraw;

extern RGBState rgbState;