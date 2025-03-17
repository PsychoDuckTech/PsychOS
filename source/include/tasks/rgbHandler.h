#pragma once
#include <FastLED.h>
#include "globals.h" // Assumes this defines shared variables like currentBrightness

extern RGBState rgbState;

#define NUM_LEDS 60        // Number of LEDs in the strip
#define MAX_COLORS 7       // Maximum number of colors supported
#define HEX_COLOR_LENGTH 7 // Length of hex color string "#RRGGBB"

// Enumerations
typedef enum
{
    RGB_EFFECT_STATIC,
    RGB_EFFECT_RUNNER,
    RGB_EFFECT_SCROLL,
    RGB_EFFECT_FLASH,
    RGB_EFFECT_BREATHE,
    // Add other effects as needed
} RGBEffectType;

typedef enum
{
    RGB_EVENT_MODULE_CONNECT,
    RGB_EVENT_MODULE_DISCONNECT
} RGBEventType;

typedef enum
{
    RGB_CMD_SET_COLOR,
    RGB_CMD_SET_EFFECT,
    RGB_CMD_SET_BRIGHTNESS,
    RGB_CMD_TRIGGER_EVENT,
    RGB_CMD_SET_SPEED
} RGBCommandType;

// Effect configuration structure
typedef struct
{
    RGBEffectType effect;
    uint8_t speed;
    uint8_t intensity;
} RGBEffectConfig;

// Command structure for queue
typedef struct
{
    RGBCommandType type;
    union
    {
        struct
        {
            uint8_t index;
            char hex[HEX_COLOR_LENGTH];
            bool remove;
        } color;
        struct
        {
            RGBEffectConfig config;
            char colors[MAX_COLORS][HEX_COLOR_LENGTH];
            uint8_t num_colors;
            bool set_colors;
            bool temporary;
            uint16_t duration_ms;
        } effect;
        uint8_t brightness;
        RGBEventType event;
        uint8_t speed; // Speed value for RGB_CMD_SET_SPEED
    } data;
} RGBCommand;

// External variables
extern QueueHandle_t rgbCommandQueue;
extern CRGB leds[NUM_LEDS];
extern uint8_t currentBrightness;
extern uint8_t globalMaxBrightnessPercent;

// Class definition
class uRGBClass
{
public:
    uRGBClass();
    void color1(const char *hex);
    void color2(const char *hex);
    void color3(const char *hex);
    void color4(const char *hex);
    void color5(const char *hex);
    void color6(const char *hex);
    void color7(const char *hex);
    void effect(RGBEffectType type);
    void brightness(uint8_t percent);
    void event(RGBEventType event);
    void setMaxBrightness(uint8_t percent);
    void speed(uint8_t level);

private:
    void setColor(uint8_t index, const char *hex, bool remove = false);
};

extern uRGBClass uRGB;

void rgbTask(void *parameters);
void startRgbTask(UBaseType_t core = 0, uint32_t stackDepth = 4096, UBaseType_t priority = 1);