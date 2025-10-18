#pragma once
#include <Adafruit_NeoPixel.h>
#include "globals.h" // Assumes this defines shared variables like currentBrightness

extern RGBState rgbState;

#define NUM_LEDS 61        // Number of LEDs in the strip
#define MAX_COLORS 7       // Maximum number of colors supported
#define HEX_COLOR_LENGTH 7 // Length of hex color string "#RRGGBB"

// Pins
#define LED_PIN 3        // WS2812 data pin
#define GPIO46 46        // Power enable pin

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
    RGB_CMD_SET_SPEED,
    RGB_CMD_GET_BRIGHTNESS,  // New command to request current brightness
    RGB_CMD_GET_SPEED,       // New command to request current speed
    RGB_CMD_SYNC_UI          // New command to sync all values to the UI
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

// Response structure for getting values back from RGB task
typedef struct
{
    uint8_t brightness;   // Current brightness value (0-100)
    uint8_t speed;        // Current speed value (1-20)
    RGBEffectType effect; // Current effect
} RGBResponse;

// External variables
extern QueueHandle_t rgbCommandQueue;
extern QueueHandle_t rgbResponseQueue; // New queue for receiving responses from RGB task
extern Adafruit_NeoPixel strip;
extern uint8_t currentBrightness;
extern uint8_t globalMaxBrightnessPercent;

// Class definition
struct RGBConfig {
    RGBEffectType effect = RGB_EFFECT_STATIC;
    const char* colors[MAX_COLORS] = {nullptr};
    uint8_t numColors = 0;
    uint8_t brightness = 100;
    uint8_t speed = 10;
};

class uRGBClass {
public:
    uRGBClass();
    void configure(const RGBConfig& config);
    void setMaxBrightness(uint8_t percent);
    void event(RGBEventType event);
    
    // New methods for getting values from RGB task
    bool getCurrentValues(uint8_t* brightness, uint8_t* speed, RGBEffectType* effect = nullptr);
    void syncUIValues(); // Request the RGB task to update rgbState with current values
    
private:
    void setColor(uint8_t index, const char *hex, bool remove = false);
};

extern uRGBClass uRGB;

void rgbTask(void *parameters);
void startRgbTask(UBaseType_t core = 0, uint32_t stackDepth = 4096, UBaseType_t priority = 1);
static void applyCurrentEffect();