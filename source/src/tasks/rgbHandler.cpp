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

const uint8_t gamma8[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};

// Helper function to convert hex string to CRGB
static CRGB hexToCRGB(const char *hex)
{
    // Skip the '#' if present
    if (hex[0] == '#')
        hex++;

    // Convert hex string to 32-bit integer (e.g., "FF0000" → 0xFF0000)
    uint32_t rgb = strtoul(hex, NULL, 16);

    // Extract raw RGB components
    uint8_t r = (rgb >> 16) & 0xFF; // Red: bits 23–16
    uint8_t g = (rgb >> 8) & 0xFF;  // Green: bits 15–8
    uint8_t b = rgb & 0xFF;         // Blue: bits 7–0

    // Apply gamma correction using the lookup table
    r = gamma8[r];
    g = gamma8[g];
    b = gamma8[b];

    return CRGB(r, g, b);
}

// Update brightness with global max limit
static void updateBrightness()
{
    float scale = globalMaxBrightnessPercent / 100.0f;
    uint8_t actualBrightness = (currentBrightness / 100.0f) * scale * 255;
    FastLED.setBrightness(actualBrightness);
}

// Set brightness with optional fade effect
static void setBrightnessWithFade(uint8_t targetBrightness, uint32_t fadeDurationMs = 0)
{
    if (fadeDurationMs == 0)
    {
        // Immediate brightness change
        currentBrightness = constrain(targetBrightness, 0, 100);
        updateBrightness();
        FastLED.show();
        return;
    }

    // Fade from current brightness to target brightness
    const int fadeSteps = 100; // High step count for smoothness
    const int fadeDelay = fadeDurationMs / fadeSteps;
    int startBrightness = currentBrightness;
    int brightnessDelta = targetBrightness - startBrightness;

    for (int i = 0; i <= fadeSteps; i++)
    {
        currentBrightness = startBrightness + (brightnessDelta * i) / fadeSteps;
        updateBrightness();
        applyCurrentEffect(); // Ensure the current effect is applied at each step
        FastLED.show();
        vTaskDelay(pdMS_TO_TICKS(fadeDelay));
    }

    // Ensure final brightness is exact
    currentBrightness = constrain(targetBrightness, 0, 100);
    updateBrightness();
    FastLED.show();
}

// Simplified effect implementation (expand as needed)
static void applyCurrentEffect()
{
    if (currentEffect.effect == RGB_EFFECT_STATIC)
    {
        fill_solid(leds, NUM_LEDS, effectColors[0]);
    }
    else if (currentEffect.effect == RGB_EFFECT_BREATHE)
    {
        static float phase = 0.0f;
        const float speed = 0.01f; // Adjust this value to control breathing speed

        // Calculate brightness using a sine wave (ranges from 0 to 1)
        float brightness = (sin(phase * 2 * PI) + 1) / 2;

        // Cycle through colors
        static float colorPhase = 0.0f;
        float colorPosition = fmod(colorPhase, 1.0f);
        int colorIndex1 = floor(colorPosition * (numColors - 1));
        int colorIndex2 = ceil(colorPosition * (numColors - 1));
        float ratio = (colorPosition * (numColors - 1)) - colorIndex1;

        // Blend between two colors to get the base color
        CRGB baseColor = blend(effectColors[colorIndex1 % numColors], effectColors[colorIndex2 % numColors], ratio * 255);

        // Apply brightness scaling
        CRGB dimmedColor = baseColor;
        dimmedColor.nscale8_video(brightness * 255);

        // Set all LEDs to the dimmed color
        fill_solid(leds, NUM_LEDS, dimmedColor);

        // Update phases
        phase += speed;
        if (phase >= 1.0f)
            phase -= 1.0f;
        colorPhase += speed / 10.0f; // Slower color transition
        if (colorPhase >= 1.0f)
            colorPhase -= 1.0f;
    }
    else if (currentEffect.effect == RGB_EFFECT_RUNNER)
    {
        static float phase = 0.0f;
        float speed = (float)currentEffect.speed / 255.0f * 0.005f; // Max speed = 0.005f

        for (int i = 0; i < NUM_LEDS; i++)
        {
            float position = fmod((float)i / NUM_LEDS + phase, 1.0f);
            int colorIndex1 = floor(position * (numColors - 1));
            int colorIndex2 = ceil(position * (numColors - 1));
            float ratio = (position * (numColors - 1)) - colorIndex1;

            CRGB color1 = effectColors[colorIndex1 % numColors];
            CRGB color2 = effectColors[colorIndex2 % numColors];
            leds[i] = blend(color1, color2, ratio * 255);
        }

        phase += speed;
        if (phase >= 1.0f)
            phase -= 1.0f;
    }
    else if (currentEffect.effect == RGB_EFFECT_SCROLL)
    {
        static float phase = 0.0f;
        phase += (float)currentEffect.speed / 255.0f * 0.01f;
        if (phase >= 1.0f)
            phase -= 1.0f;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            float position = fmod((float)i / NUM_LEDS + phase, 1.0f);
            float scaledPosition = position * numColors;
            int colorIndex1 = static_cast<int>(floor(scaledPosition)) % numColors;
            int colorIndex2 = (colorIndex1 + 1) % numColors;
            float ratio = scaledPosition - floor(scaledPosition);
            CRGB color1 = effectColors[colorIndex1];
            CRGB color2 = effectColors[colorIndex2];
            leds[i] = blend(color1, color2, ratio * 255);
        }
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

    // Set default effect and colors
    currentEffect = {RGB_EFFECT_STATIC, 128, 255};
    effectColors[0] = CRGB::White;
    numColors = 1;

    // Start with brightness 0 and fade to initial value
    currentBrightness = 0;
    applyCurrentEffect();
    updateBrightness();
    FastLED.show();

    // Fade in to the initial brightness (100) over 1000ms
    setBrightnessWithFade(100, 1000);

    // Proceed to the main loop
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
                setBrightnessWithFade(cmd.data.brightness, 500); // Fade over 500ms for command-based changes
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

            case RGB_CMD_SET_SPEED:
                currentEffect.speed = cmd.data.speed;
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
void uRGBClass::color5(const char *hex) { setColor(5, hex, false); }
void uRGBClass::color6(const char *hex) { setColor(6, hex, false); }
void uRGBClass::color7(const char *hex) { setColor(7, hex, false); }

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

void uRGBClass::speed(uint8_t level)
{
    // Validate input range
    if (level < 1 || level > 20)
    {
        return; // Ignore invalid levels
    }

    // Map level 1-20 to speed 1-255
    uint8_t speed = map(level, 1, 20, 1, 255);

    // Create and send the command
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_SPEED;
    cmd.data.speed = speed;
    xQueueSend(rgbCommandQueue, &cmd, portMAX_DELAY);
}

uRGBClass uRGB;