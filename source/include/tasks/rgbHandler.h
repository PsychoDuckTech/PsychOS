#pragma once

#include <FastLED.h>
#include "globals.h"

void ledTask(void *parameters);
void triggerSpecialEffect(uint8_t effectType);
void startRgbHandlerTask(UBaseType_t core = 0, uint32_t stackDepth = 4096, UBaseType_t priority = 1);

enum LEDEffects
{
    EFFECT_DYNAMIC_RAINBOW,
    EFFECT_COLOR_WAVE,
    EFFECT_FIRE,
    EFFECT_STARFIELD,
    EFFECT_OFF
};

enum SpecialEffects
{
    EFFECT_CAPS_WARNING,
    EFFECT_BLE_CONNECTED,
    EFFECT_BLE_DISCONNECTED
};

extern uint8_t currentBaseEffect;
extern bool effectInterrupted;
extern CRGBPalette16 currentPalette;