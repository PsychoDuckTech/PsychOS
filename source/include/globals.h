#pragma once

#define COLOR_SCHEME 1

#if COLOR_SCHEME == 1
#define BG_COLOR 0x0
#define TEXT_COLOR 0xD5F1 // 0x95E5,0xBD6C (old 0xDED9)
#define SUCCESS_COLOR 0x9C1F
#define ERROR_COLOR 0xF22B
#define MUTED_COLOR 0x7BEF
#define ULTRA_MUTED_COLOR 0x18C3
#define HIGHLIGHT_COLOR 0xFD40
#define SELECTED_COLOR 0x2945
#else
#define BG_COLOR 0x0
#define TEXT_COLOR 0xFFFF
#define HIGHLIGHT_COLOR 0xC618
#define MUTED_COLOR 0x39C7
#define ULTRA_MUTED_COLOR 0x18C3
#define SUCCESS_COLOR 0x0640
#define ERROR_COLOR 0xF800
#define SHADOW_1 0xCC40
#define SHADOW_2 0x9B20
#endif

extern const char *connectedModuleName;

// Include Adafruit GFX first as it contains the GFXfont definition
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "utils/nvs.h"

extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern unsigned long lastDataReceivedTime; // Timestamp of last data reception
extern bool capsLockStatus;
extern int hours;
extern int minutes;
extern int seconds;

extern NVSUtil nvs;

extern int settingsSelectedOption;
extern bool inSettingsSubmenu;

extern int rgbValues[4];

struct RGBState
{
    uint8_t currentSelection; // 0: Effect, 1: Brightness, 2: Speed
    uint8_t brightness;       // 0-100 (%)
    uint8_t speed;            // 1-20 scale
    uint8_t effect;           // RGB effect type (corresponds to RGBEffectType enum)
    bool needsRefresh;        // Flag for partial updates
};
extern int firstDraw;

extern RGBState rgbState;

extern bool pixelFlushComplete;