#include "rgbHandler.h"

// Changed definition to default initialization to match the extern declaration in globals.h
RGBState rgbState;

QueueHandle_t rgbCommandQueue = NULL;
CRGB leds[NUM_LEDS];
RGBEffectConfig currentEffect = {RGB_EFFECT_STATIC, 128, 255};
RGBEffectConfig previousEffect;
CRGB effectColors[MAX_COLORS];
uint8_t numColors = 1;
uint8_t currentBrightness = 255;
bool inTemporaryEffect = false;
unsigned long temporaryEffectEnd = 0;

CRGB hexToCRGB(const char *hexColor)
{
    long number = strtol(&hexColor[1], NULL, 16);
    return CRGB((number >> 16) & 0xFF, (number >> 8) & 0xFF, number & 0xFF);
}

void applyConnectionEffect()
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_EFFECT;
    cmd.data.effect.config = {RGB_EFFECT_SCROLL, 50, 255};
    strncpy(cmd.data.effect.colors[0], "#0000FF", HEX_COLOR_LENGTH);
    strncpy(cmd.data.effect.colors[1], "#000000", HEX_COLOR_LENGTH);
    cmd.data.effect.num_colors = 2;
    cmd.data.effect.temporary = true;
    cmd.data.effect.duration_ms = 2000;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

void applyDisconnectionEffect()
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_EFFECT;
    cmd.data.effect.config = {RGB_EFFECT_FLASH, 100, 255};
    strncpy(cmd.data.effect.colors[0], "#FF0000", HEX_COLOR_LENGTH);
    strncpy(cmd.data.effect.colors[1], "#000000", HEX_COLOR_LENGTH);
    cmd.data.effect.num_colors = 2;
    cmd.data.effect.temporary = true;
    cmd.data.effect.duration_ms = 1000;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

void restorePreviousEffect()
{
    if (inTemporaryEffect)
    {
        RGBCommand cmd;
        cmd.type = RGB_CMD_SET_EFFECT;
        cmd.data.effect.config = previousEffect;
        cmd.data.effect.num_colors = numColors;
        cmd.data.effect.temporary = false;
        xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
    }
}

void rgbTask(void *parameters)
{
    FastLED.addLeds<APA102, 3, 46, BGR>(leds, NUM_LEDS);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.clear(true);
    FastLED.setBrightness(currentBrightness);
    FastLED.show();

    RGBCommand cmd;
    effectColors[0] = CRGB::White; // Default color

    for (;;)
    {
        // Check temporary effect timeout
        if (inTemporaryEffect && millis() > temporaryEffectEnd)
        {
            restorePreviousEffect();
        }

        // Process commands
        while (xQueueReceive(rgbCommandQueue, &cmd, 0) == pdTRUE)
        {
            switch (cmd.type)
            {
            case RGB_CMD_SET_EFFECT:
            {
                if (cmd.data.effect.temporary)
                {
                    previousEffect = currentEffect;
                    inTemporaryEffect = true;
                    temporaryEffectEnd = millis() + cmd.data.effect.duration_ms;
                }

                currentEffect = cmd.data.effect.config;
                numColors = cmd.data.effect.num_colors;

                for (int i = 0; i < numColors; i++)
                {
                    effectColors[i] = hexToCRGB(cmd.data.effect.colors[i]);
                }
                break;
            }

            case RGB_CMD_SET_BRIGHTNESS:
                currentBrightness = cmd.data.brightness.brightness;
                FastLED.setBrightness(currentBrightness);
                break;

            case RGB_CMD_TRIGGER_EVENT:
                switch (cmd.data.trigger.event_type)
                {
                case RGB_EVENT_CONNECT:
                    applyConnectionEffect();
                    break;
                case RGB_EVENT_DISCONNECT:
                    applyDisconnectionEffect();
                    break;
                case RGB_EVENT_CAPS_WARNING:
                    // Implement caps warning effect
                    break;
                }
                break;
            }
        }

        // Apply current effect
        switch (currentEffect.effect)
        {
        case RGB_EFFECT_STATIC:
            fill_solid(leds, NUM_LEDS, effectColors[0]);
            break;
            // Add other effect implementations
        }

        FastLED.show();
        vTaskDelay(pdMS_TO_TICKS(currentEffect.speed / 16));
    }
}

void startRgbTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    rgbCommandQueue = xQueueCreate(10, sizeof(RGBCommand));
    xTaskCreatePinnedToCore(
        rgbTask,
        "RGB Handler",
        stackDepth,
        NULL,
        priority,
        NULL,
        core);
}