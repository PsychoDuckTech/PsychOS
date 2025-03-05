#pragma once
#include <FastLED.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#define NUM_LEDS 60
#define MAX_COLORS 7
#define MAX_EFFECT_PARAMS 3
#define HEX_COLOR_LENGTH 7 // #RRGGBB

extern uint8_t currentBrightness;

typedef enum
{
    RGB_EFFECT_STATIC,
    RGB_EFFECT_SCROLL,
    RGB_EFFECT_BREATHE,
    RGB_EFFECT_FLASH,
    RGB_EFFECT_RAINBOW,
    RGB_EFFECT_FIRE,
    RGB_EFFECT_STARFIELD,
    RGB_EFFECT_TEMPORARY // For connection/disconnection effects
} RGBEffectType;

typedef enum
{
    RGB_CMD_SET_EFFECT,
    RGB_CMD_SET_BRIGHTNESS,
    RGB_CMD_TRIGGER_EVENT
} RGBCommandType;

typedef enum
{
    RGB_EVENT_CONNECT,
    RGB_EVENT_DISCONNECT,
    RGB_EVENT_CAPS_WARNING
} RGBEventType;

typedef struct
{
    RGBEffectType effect;
    uint8_t speed;     // 0-255
    uint8_t intensity; // 0-255
    uint8_t params[MAX_EFFECT_PARAMS];
} RGBEffectConfig;

typedef struct
{
    RGBCommandType type;
    union
    {
        struct
        {
            RGBEffectConfig config;
            char colors[MAX_COLORS][HEX_COLOR_LENGTH];
            uint8_t num_colors;
            bool temporary;
            uint16_t duration_ms;
        } effect;
        struct
        {
            uint8_t brightness;
        } brightness;
        struct
        {
            RGBEventType event_type;
            uint16_t duration_ms;
        } trigger;
    } data;
} RGBCommand;

extern QueueHandle_t rgbCommandQueue;

// Conversion function for hex colors
CRGB hexToCRGB(const char *hexColor);

void rgbTask(void *parameters);
void startRgbTask(UBaseType_t core = 0, uint32_t stackDepth = 4096, UBaseType_t priority = 1);