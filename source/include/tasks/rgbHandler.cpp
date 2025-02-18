// Update ledHandler.cpp
#include "rgbHandler.h"
#include "commandProcessor.h"

#define LED_DATA_PIN 3
#define LED_CLK_PIN 46
#define NUM_LEDS 60

CRGB leds[NUM_LEDS];
uint8_t currentBaseEffect = EFFECT_DYNAMIC_RAINBOW;
bool effectInterrupted = false;
CRGBPalette16 currentPalette;
TBlendType currentBlending;
unsigned long effectTimeout = 0;

RGBState rgbState = {
    .currentSelection = 0,
    .values = {255, 255, 255, 25}, // Default values
    .needsRefresh = true};

// Base Effects
void dynamicRainbow()
{
    static uint8_t hue = 0;
    fill_rainbow(leds, NUM_LEDS, hue++, 7);
    FastLED.show();
}

void colorWave()
{
    static uint8_t hue = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(hue + (i * 10), 255, rgbState.values[3] * 2.55);
    }
    hue += 2;
    FastLED.show();
}

void fireEffect()
{
    static uint8_t heat[NUM_LEDS];
    for (int i = 0; i < NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, 35));
    }

    for (int j = NUM_LEDS - 1; j >= 2; j--)
    {
        heat[j] = (heat[j - 1] + heat[j - 2] + heat[j - 2]) / 3;
    }

    if (random8() < 40)
    {
        heat[random8(4)] = random8(160, 255);
    }

    for (int k = 0; k < NUM_LEDS; k++)
    {
        leds[k] = ColorFromPalette(HeatColors_p, heat[k]);
    }
    FastLED.show();
}

void starfieldEffect()
{
    fadeToBlackBy(leds, NUM_LEDS, 10);
    if (random8() < 30)
    {
        leds[random16(NUM_LEDS)] = CRGB::White;
    }
    FastLED.show();
}

// Special Effects
void bleConnectedEffect()
{
    static uint8_t center = NUM_LEDS / 2;
    static uint8_t spread = 0;

    fadeToBlackBy(leds, NUM_LEDS, 20);
    leds[center] = CHSV(160, 255, 255);
    for (int i = 0; i < spread; i++)
    {
        if (center + i < NUM_LEDS)
            leds[center + i].nscale8(200);
        if (center - i >= 0)
            leds[center - i].nscale8(200);
    }
    spread = (spread + 1) % 8;
    FastLED.show();
}

void capsWarningEffect()
{
    static uint8_t pulseVal = 0;
    static int8_t direction = 1;

    pulseVal += direction;
    if (pulseVal >= 100 || pulseVal <= 0)
        direction *= -1;

    fill_gradient_RGB(leds, NUM_LEDS,
                      CHSV(40, 255, map(pulseVal, 0, 100, 50, 255)),
                      CHSV(40, 255, map(pulseVal, 0, 100, 20, 150)));
    FastLED.show();
}

void updateBaseEffect()
{
    switch (currentBaseEffect)
    {
    case EFFECT_DYNAMIC_RAINBOW:
        dynamicRainbow();
        break;
    case EFFECT_COLOR_WAVE:
        colorWave();
        break;
    case EFFECT_FIRE:
        fireEffect();
        break;
    case EFFECT_STARFIELD:
        starfieldEffect();
        break;
    case EFFECT_OFF:
        FastLED.clear(true);
        break;
    }
}

void ledTask(void *parameters)
{
    FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLK_PIN, BGR>(leds, NUM_LEDS);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.clear(true);

    // Initial fade-in sequence
    for (int bright = 0; bright <= rgbState.values[3]; bright++)
    {
        fill_solid(leds, NUM_LEDS, CRGB(rgbState.values[0], rgbState.values[1], rgbState.values[2]));
        FastLED.setBrightness(map(bright, 0, 100, 0, 255));
        FastLED.show();
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }

    for (;;)
    {
        static uint8_t lastBrightness = rgbState.values[3];
        static bool lastCaps = capsLockStatus;
        static bool lastBLEState = moduleConnectionStatus;

        // Handle brightness changes
        if (lastBrightness != rgbState.values[3])
        {
            FastLED.setBrightness(map(rgbState.values[3], 0, 100, 0, 255));
            lastBrightness = rgbState.values[3];
        }

        // Handle effect interruptions
        if (effectInterrupted)
        {
            if (millis() > effectTimeout)
            {
                effectInterrupted = false;
            }
            continue;
        }

        // Handle automatic effect changes
        if (!moduleConnectionStatus && currentBaseEffect == EFFECT_STARFIELD)
        {
            currentBaseEffect = EFFECT_DYNAMIC_RAINBOW;
        }

        // Handle BLE connection events
        if (moduleConnectionStatus != lastBLEState)
        {
            triggerSpecialEffect(moduleConnectionStatus ? EFFECT_BLE_CONNECTED : EFFECT_BLE_DISCONNECTED);
            lastBLEState = moduleConnectionStatus;
        }

        // Handle caps lock state changes
        if (capsLockStatus != lastCaps)
        {
            if (capsLockStatus)
                triggerSpecialEffect(EFFECT_CAPS_WARNING);
            lastCaps = capsLockStatus;
        }

        updateBaseEffect();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void triggerSpecialEffect(uint8_t effectType)
{
    effectInterrupted = true;
    effectTimeout = millis() + 2000; // Show effect for 2 seconds

    switch (effectType)
    {
    case EFFECT_CAPS_WARNING:
        for (int i = 0; i < 50; i++)
        {
            capsWarningEffect();
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
        break;

    case EFFECT_BLE_CONNECTED:
        for (int i = 0; i < 50; i++)
        {
            bleConnectedEffect();
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
        break;

    case EFFECT_BLE_DISCONNECTED:
        fill_solid(leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
        vTaskDelay(500);
        FastLED.clear();
        FastLED.show();
        break;
    }
    effectInterrupted = false;
}

void startRgbHandlerTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t ledTaskHandle;
    xTaskCreatePinnedToCore(
        ledTask,
        "RGB Glow Handler",
        stackDepth,
        NULL,
        priority,
        &ledTaskHandle,
        core);
}