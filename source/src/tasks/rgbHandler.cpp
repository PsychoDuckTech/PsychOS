#include "tasks/rgbHandler.h"
#include "main.h" // Add main.h include to get pin definitions

// Global variables
QueueHandle_t rgbCommandQueue = NULL;
QueueHandle_t rgbResponseQueue = NULL; // New response queue
CRGB leds[NUM_LEDS];
uint8_t currentBrightness = 100;
uint8_t globalMaxBrightnessPercent = 100;
RGBState rgbState = {
    0,                 // currentSelection starts with effect now
    70,                // brightness default 70% (matches config)
    20,                // speed default 20 (matches config)
    RGB_EFFECT_RUNNER, // Default effect
    true               // Set needsRefresh to true initially
};

// State variables
static RGBEffectConfig currentEffect = {RGB_EFFECT_STATIC, 128, 255};
static RGBEffectConfig previousEffect;
static CRGB effectColors[MAX_COLORS];
static CRGB previousColors[MAX_COLORS];
static uint8_t numColors = 1;
static uint8_t previousNumColors;
static bool inTemporaryEffect = false;
static unsigned long temporaryEffectEnd = 0;

// Helper function to apply gamma correction (gamma 2.2)
static uint8_t applyGamma(uint8_t value)
{
    // Formula: (value/255)^(gamma) * 255
    // Using fixed-point arithmetic for speed, approximating gamma 2.2
    return (uint8_t)(pow((float)value / 255.0f, 2.2f) * 255.0f + 0.5f);
}

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

    // Apply gamma correction
    r = applyGamma(r);
    g = applyGamma(g);
    b = applyGamma(b);

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
    // Use non-blocking queue send with timeout
    if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
    {
        Serial.println("RGB queue full in connect event");
    }
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
    // Use non-blocking queue send with timeout
    if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
    {
        Serial.println("RGB queue full in disconnect event");
    }
}

void rgbTask(void *parameters)
{
    // Initialize FastLED
    FastLED.addLeds<APA102, RGB_DATA_PIN, RGB_CLOCK_PIN, BGR>(leds, NUM_LEDS);
    FastLED.setCorrection(TypicalLEDStrip);

    // Create the response queue for getting values back from the RGB task
    rgbResponseQueue = xQueueCreate(1, sizeof(RGBResponse));

    // Set initial default values
    currentEffect = {RGB_EFFECT_STATIC, 128, 255}; // Default effect: static, medium speed
    effectColors[0] = CRGB::White;                 // Default color: white
    numColors = 1;
    uint8_t targetBrightness = 100; // Default target brightness: 100%

    // Process initial commands from the queue
    RGBCommand cmd;
    while (xQueueReceive(rgbCommandQueue, &cmd, 0) == pdTRUE)
    {
        switch (cmd.type)
        {
        case RGB_CMD_SET_COLOR:
            if (cmd.data.color.index < MAX_COLORS && !cmd.data.color.remove)
            {
                effectColors[cmd.data.color.index] = hexToCRGB(cmd.data.color.hex);
                if (cmd.data.color.index + 1 > numColors)
                {
                    numColors = cmd.data.color.index + 1;
                }
            }
            break;
        case RGB_CMD_SET_EFFECT:
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
            targetBrightness = cmd.data.brightness; // Set the target brightness from the command
            break;
        case RGB_CMD_SET_SPEED:
            // Update the speed parameter in the current effect config
            currentEffect.speed = static_cast<uint8_t>(map(cmd.data.speed, 1, 20, 1, 255));
            break;
            // Add other command cases if needed
        }
    }

    // Start with brightness at 0 and apply the current effect
    currentBrightness = 0;
    applyCurrentEffect();
    updateBrightness();
    FastLED.show();

    // Fade in to the target brightness over 1000ms
    setBrightnessWithFade(targetBrightness, 1000);

    // Main loop to handle ongoing commands
    while (true)
    {
        // Process any pending commands
        RGBCommand cmd;
        while (xQueueReceive(rgbCommandQueue, &cmd, 0) == pdTRUE) // Non-blocking check
        {
            switch (cmd.type)
            {
            case RGB_CMD_SET_COLOR:
                if (cmd.data.color.index < MAX_COLORS && !cmd.data.color.remove)
                {
                    effectColors[cmd.data.color.index] = hexToCRGB(cmd.data.color.hex);
                    if (cmd.data.color.index + 1 > numColors)
                    {
                        numColors = cmd.data.color.index + 1;
                    }
                }
                break;
            case RGB_CMD_SET_EFFECT:
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
                currentBrightness = cmd.data.brightness;
                updateBrightness();
                break;
            case RGB_CMD_SET_SPEED:
                currentEffect.speed = static_cast<uint8_t>(map(cmd.data.speed, 1, 20, 1, 255));
                break;

            case RGB_CMD_GET_BRIGHTNESS:
            case RGB_CMD_GET_SPEED:
            case RGB_CMD_SYNC_UI:
                // Send back current values
                if (rgbResponseQueue != NULL)
                {
                    RGBResponse response;
                    response.brightness = currentBrightness;
                    response.speed = map(currentEffect.speed, 1, 255, 1, 20); // Convert back to 1-20 scale
                    response.effect = currentEffect.effect;

                    // Send values without blocking
                    xQueueOverwrite(rgbResponseQueue, &response);

                    // Update the rgbState directly if requested
                    if (cmd.type == RGB_CMD_SYNC_UI)
                    {
                        rgbState.brightness = currentBrightness;
                        rgbState.speed = map(currentEffect.speed, 1, 255, 1, 20);
                        rgbState.effect = currentEffect.effect; // Also sync the effect type
                        rgbState.needsRefresh = true;
                    }
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

void uRGBClass::configure(const RGBConfig &config)
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_EFFECT;
    cmd.data.effect.config = {config.effect,
                              static_cast<uint8_t>(map(config.speed, 1, 20, 1, 255)),
                              255};

    // Set colors if provided
    cmd.data.effect.set_colors = (config.numColors > 0);
    cmd.data.effect.num_colors = std::min(config.numColors, static_cast<uint8_t>(MAX_COLORS));

    for (uint8_t i = 0; i < cmd.data.effect.num_colors; i++)
    {
        if (config.colors[i])
        {
            strncpy(cmd.data.effect.colors[i], config.colors[i], HEX_COLOR_LENGTH);
        }
        else
        {
            strncpy(cmd.data.effect.colors[i], "#000000", HEX_COLOR_LENGTH);
        }
    }

    cmd.data.effect.temporary = false;
    cmd.data.effect.duration_ms = 0;
    // Use non-blocking queue send with timeout
    if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
    {
        Serial.println("RGB queue full in configure()");
    }

    // Set brightness if different from current
    if (config.brightness != currentBrightness)
    {
        RGBCommand brightnessCmd;
        brightnessCmd.type = RGB_CMD_SET_BRIGHTNESS;
        brightnessCmd.data.brightness = config.brightness;
        // Use non-blocking queue send with timeout
        if (xQueueSend(rgbCommandQueue, &brightnessCmd, pdMS_TO_TICKS(50)) != pdPASS)
        {
            Serial.println("RGB queue full in configure() (brightness)");
        }
    }
}

void uRGBClass::setMaxBrightness(uint8_t percent)
{
    globalMaxBrightnessPercent = constrain(percent, 0, 100);
    updateBrightness();
}

void uRGBClass::event(RGBEventType event)
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_TRIGGER_EVENT;
    cmd.data.event = event;
    // Use non-blocking queue send with timeout
    if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
    {
        Serial.println("RGB queue full in event()");
    }
}

void uRGBClass::setColor(uint8_t index, const char *hex, bool remove)
{
    RGBCommand cmd;
    cmd.type = RGB_CMD_SET_COLOR;
    cmd.data.color.index = index;
    cmd.data.color.remove = remove;
    strncpy(cmd.data.color.hex, remove ? "#000000" : hex, HEX_COLOR_LENGTH);
    // Use non-blocking queue send with timeout
    if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
    {
        Serial.println("RGB queue full in setColor()");
    }
}

bool uRGBClass::getCurrentValues(uint8_t *brightness, uint8_t *speed, RGBEffectType *effect)
{
    if (rgbResponseQueue == NULL)
    {
        return false; // Queue not initialized yet
    }

    // Send a request to get current values
    RGBCommand cmd;
    cmd.type = RGB_CMD_GET_BRIGHTNESS; // Any get command will work

    if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
    {
        Serial.println("RGB queue full in getCurrentValues()");
        return false;
    }

    // Wait for response with a reasonable timeout
    RGBResponse response;
    if (xQueueReceive(rgbResponseQueue, &response, pdMS_TO_TICKS(100)) != pdPASS)
    {
        Serial.println("Timeout waiting for RGB response");
        return false;
    }

    // Update output parameters
    if (brightness)
        *brightness = response.brightness;
    if (speed)
        *speed = response.speed;
    if (effect)
        *effect = response.effect;

    return true;
}

void uRGBClass::syncUIValues()
{
    // Request RGB task to sync values to rgbState
    RGBCommand cmd;
    cmd.type = RGB_CMD_SYNC_UI;

    if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
    {
        Serial.println("RGB queue full in syncUIValues()");
    }
}

uRGBClass uRGB;