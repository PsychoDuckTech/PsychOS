#include "tasks/rgbHandler.h"

// Global variables
QueueHandle_t rgbCommandQueue = NULL;
CRGB leds[NUM_LEDS];
uint8_t currentBrightness = 100;
uint8_t globalMaxBrightnessPercent = 100;
RGBState rgbState;

// State variables
static RGBEffectConfig currentEffect = {RGB_EFFECT_STATIC, 128, 255};
static RGBEffectConfig previousEffect;
static CRGB effectColors[MAX_COLORS];
static CRGB previousColors[MAX_COLORS];
static uint8_t numColors = 1;
static uint8_t previousNumColors;
static bool inTemporaryEffect = false;
static unsigned long temporaryEffectEnd = 0;

// Helper function to convert hex string to CRGB
static CRGB hexToCRGB(const char *hex)
{
    if (hex[0] == '#')
        hex++; // Skip '#' if present
    uint32_t rgb = strtoul(hex, NULL, 16);
    return CRGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

// Update brightness with global max limit
static void updateBrightness()
{
    float scale = globalMaxBrightnessPercent / 100.0f;
    uint8_t actualBrightness = (currentBrightness / 100.0f) * scale * 255;
    FastLED.setBrightness(actualBrightness);
}

// Simplified effect implementation (expand as needed)
static void applyCurrentEffect()
{
    if (currentEffect.effect == RGB_EFFECT_STATIC)
    {
        fill_solid(leds, NUM_LEDS, effectColors[0]);
    }
    else if (currentEffect.effect == RGB_EFFECT_FLASH)
    {
        static bool state = false;
        uint16_t period = map(currentEffect.speed, 0, 255, 2000, 100);
        state = (millis() % period) < (period / 2);
        fill_solid(leds, NUM_LEDS, state ? effectColors[0] : (numColors > 1 ? effectColors[1] : CRGB::Black));
    }
    else if (currentEffect.effect == RGB_EFFECT_SCROLL)
    {
        static uint16_t offset = 0;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = effectColors[(i + offset) % numColors];
        }
        offset = (offset + 1) % numColors;
    }
}

// Event effect handlers
static void triggerModuleConnect()
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_EFFECT;
    cmd.data.effect.config = {RGB_EFFECT_FLASH, 100, 255};
    strncpy(cmd.data.effect.colors[0], "#00FF00", HEX_COLOR_LENGTH); // Green
    strncpy(cmd.data.effect.colors[1], "#000000", HEX_COLOR_LENGTH); // Off
    cmd.data.effect.num_colors = 2;
    cmd.data.effect.set_colors = true;
    cmd.data.effect.temporary = true;
    cmd.data.effect.duration_ms = 1000;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

static void triggerModuleDisconnect()
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_EFFECT;
    cmd.data.effect.config = {RGB_EFFECT_FLASH, 100, 255};
    strncpy(cmd.data.effect.colors[0], "#FF0000", HEX_COLOR_LENGTH); // Red
    strncpy(cmd.data.effect.colors[1], "#000000", HEX_COLOR_LENGTH); // Off
    cmd.data.effect.num_colors = 2;
    cmd.data.effect.set_colors = true;
    cmd.data.effect.temporary = true;
    cmd.data.effect.duration_ms = 1000;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

// RGB Task
void rgbTask(void *parameters)
{
    FastLED.addLeds<APA102, 3, 46, BGR>(leds, NUM_LEDS);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.clear();
    effectColors[0] = CRGB::White; // Default color
    updateBrightness();
    FastLED.show();

    RGBCommand cmd;
    while (true)
    {
        if (inTemporaryEffect && millis() > temporaryEffectEnd)
        {
            currentEffect = previousEffect;
            numColors = previousNumColors;
            for (int i = 0; i < numColors; i++)
            {
                effectColors[i] = previousColors[i];
            }
            inTemporaryEffect = false;
        }

        while (xQueueReceive(rgbCommandQueue, &cmd, 0) == pdTRUE)
        {
            switch (cmd.type)
            {
            case RGB_CMD_SET_COLOR:
                if (cmd.data.color.index < MAX_COLORS)
                {
                    if (!cmd.data.color.remove)
                    {
                        effectColors[cmd.data.color.index] = hexToCRGB(cmd.data.color.hex);
                        if (cmd.data.color.index + 1 > numColors)
                        {
                            numColors = cmd.data.color.index + 1;
                        }
                    }
                }
                break;

            case RGB_CMD_SET_EFFECT:
                if (cmd.data.effect.temporary)
                {
                    previousEffect = currentEffect;
                    previousNumColors = numColors;
                    for (int i = 0; i < numColors; i++)
                    {
                        previousColors[i] = effectColors[i];
                    }
                    inTemporaryEffect = true;
                    temporaryEffectEnd = millis() + cmd.data.effect.duration_ms;
                }
                currentEffect = cmd.data.effect.config;
                if (cmd.data.effect.set_colors)
                {
                    numColors = cmd.data.effect.num_colors;
                    for (int i = 0; i < numColors; i++)
                    {
                        effectColors[i] = hexToCRGB(cmd.data.effect.colors[i]);
                    }
                }
                break;

            case RGB_CMD_SET_BRIGHTNESS:
                currentBrightness = constrain(cmd.data.brightness, 0, 100);
                updateBrightness();
                break;

            case RGB_CMD_TRIGGER_EVENT:
                switch (cmd.data.event)
                {
                case RGB_EVENT_MODULE_CONNECT:
                    triggerModuleConnect();
                    break;
                case RGB_EVENT_MODULE_DISCONNECT:
                    triggerModuleDisconnect();
                    break;
                }
                break;
            }
        }

        applyCurrentEffect();
        FastLED.show();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void startRgbTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    rgbCommandQueue = xQueueCreate(10, sizeof(RGBCommand));
    xTaskCreatePinnedToCore(rgbTask, "RGB Handler", stackDepth, NULL, priority, NULL, core);
}

// uRGB Class Implementation
uRGBClass::uRGBClass() {}

void uRGBClass::setColor(uint8_t index, const char *hex, bool remove)
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_COLOR;
    cmd.data.color.index = index;
    cmd.data.color.remove = remove;
    if (remove)
    {
        strncpy(cmd.data.color.hex, "#000000", HEX_COLOR_LENGTH); // Default color
    }
    else
    {
        strncpy(cmd.data.color.hex, hex, HEX_COLOR_LENGTH);
    }
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

void uRGBClass::color1(const char *hex) { setColor(0, hex, false); }
void uRGBClass::color2(const char *hex) { setColor(1, hex, false); }
void uRGBClass::color3(const char *hex) { setColor(2, hex, false); }
void uRGBClass::color4(const char *hex) { setColor(3, hex, false); }
void uRGBClass::color5(const char *hex) { setColor(4, hex, false); }
void uRGBClass::color6(const char *hex) { setColor(5, hex, false); }
void uRGBClass::color7(const char *hex) { setColor(6, hex, false); }

void uRGBClass::effect(RGBEffectType type)
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_EFFECT;
    cmd.data.effect.config = {type, 128, 255};
    cmd.data.effect.set_colors = false;
    cmd.data.effect.temporary = false;
    cmd.data.effect.duration_ms = 0;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

void uRGBClass::brightness(uint8_t percent)
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_BRIGHTNESS;
    cmd.data.brightness = percent;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

void uRGBClass::event(RGBEventType event)
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_TRIGGER_EVENT;
    cmd.data.event = event;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

void uRGBClass::setMaxBrightness(uint8_t percent)
{
    globalMaxBrightnessPercent = constrain(percent, 0, 100);
    updateBrightness();
}

uRGBClass uRGB;