#include "rgbHandler.h"
#include "commandProcessor.h"
#include "globals.h"

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
    0,                   // currentSelection
    {127, 127, 127, 25}, // values
    true                 // needsRefresh
};

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
    static uint8_t wavePos = 0;

    // Gentle fade for smooth transitions
    fadeToBlackBy(leds, NUM_LEDS, 20);

    // Ripple outward with a color gradient
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int distance = abs(i - center);
        uint8_t brightness = sin8(wavePos + distance * 4);
        brightness = scale8(brightness, 200);             // Subtle max brightness
        uint8_t hue = map(distance, 0, center, 160, 180); // Blue to cyan
        leds[i] = CHSV(hue, 255, brightness);
    }

    wavePos += 2; // Slow wave movement
    FastLED.show();
}

void bleDisconnectedEffect()
{
    static uint8_t center = NUM_LEDS / 2;
    static uint8_t wavePos = 0;

    // Gentle fade for smooth transitions
    fadeToBlackBy(leds, NUM_LEDS, 15);

    // Ripple inward with a warm gradient
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int distance = abs(i - center);
        // Inverse sine wave to pull light inward
        uint8_t brightness = sin8(wavePos - distance * 3);
        brightness = scale8(brightness, 180);           // Slightly dimmer max brightness
        uint8_t hue = map(distance, 0, center, 30, 60); // Orange to yellow
        leds[i] = CHSV(hue, 255, brightness);
    }

    wavePos += 2; // Slow wave movement
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

    // Set initial brightness to 0 for fade-in effect
    FastLED.setBrightness(0);

    // Start the base effect immediately
    currentBaseEffect = EFFECT_DYNAMIC_RAINBOW; // Or any default effect you prefer

    for (;;)
    {
        static uint8_t lastBrightness = 0; // Track the last set brightness
        static bool fadingIn = true;       // Flag to indicate if we are in fade-in phase
        static bool lastCaps = capsLockStatus;
        static bool lastBLEState = moduleConnectionStatus;

        // Handle fade-in effect
        if (fadingIn)
        {
            uint8_t targetBrightness = map(rgbState.values[3], 0, 100, 0, 255);
            uint8_t currentBrightness = FastLED.getBrightness();
            if (currentBrightness < targetBrightness)
            {
                currentBrightness += 1; // Increment brightness gradually
                if (currentBrightness > targetBrightness)
                    currentBrightness = targetBrightness;
                FastLED.setBrightness(currentBrightness);
            }
            else
            {
                fadingIn = false; // Fade-in complete
            }
        }
        else
        {
            // Handle brightness changes after fade-in
            uint8_t targetBrightness = map(rgbState.values[3], 0, 100, 0, 255);
            if (lastBrightness != targetBrightness)
            {
                FastLED.setBrightness(targetBrightness);
                lastBrightness = targetBrightness;
            }
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
    effectTimeout = millis() + 4000; // 4-second duration for both effects

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
        // Outward ripple for ~2.5 seconds
        for (int i = 0; i < 50; i++)
        {
            bleConnectedEffect();
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
        // Contract back to center with a lingering glow
        for (int i = 0; i < 30; i++)
        {
            fadeToBlackBy(leds, NUM_LEDS, 25);
            uint8_t center = NUM_LEDS / 2;
            for (int j = 0; j < NUM_LEDS; j++)
            {
                int distance = abs(j - center);
                uint8_t brightness = sin8(255 - (i * 8 + distance * 4)); // Reverse wave
                brightness = scale8(brightness, 180);                    // Softer glow
                uint8_t hue = map(distance, 0, center, 160, 180);
                leds[j] = CHSV(hue, 255, brightness);
            }
            FastLED.show();
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
        // Final fade to dark
        for (int i = 180; i >= 0; i -= 6)
        {
            FastLED.setBrightness(i);
            FastLED.show();
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        break;

    case EFFECT_BLE_DISCONNECTED:
        /* Inward ripple for ~2.5 seconds
        for (int i = 0; i < 50; i++)
        {
            bleDisconnectedEffect();
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }*/
        // Contract further with a dim glow
        for (int i = 0; i < 30; i++)
        {
            fadeToBlackBy(leds, NUM_LEDS, 20);
            uint8_t center = NUM_LEDS / 2;
            for (int j = 0; j < NUM_LEDS; j++)
            {
                int distance = abs(j - center);
                uint8_t brightness = sin8(255 - (i * 6 + distance * 3)); // Reverse wave
                brightness = scale8(brightness, 150);                    // Dimmer glow
                uint8_t hue = map(distance, 0, center, 30, 60);
                leds[j] = CHSV(hue, 255, brightness);
            }
            FastLED.show();
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
        // Final fade to dark
        for (int i = 150; i >= 0; i -= 5)
        {
            FastLED.setBrightness(i);
            FastLED.show();
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
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