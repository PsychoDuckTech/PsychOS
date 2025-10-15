# RGB Lighting System Documentation

This document describes the RGB LED lighting system in PsychOS keyboard firmware, including configuration, effects, and customization options.

## Overview

The RGB system uses WS2812B (NeoPixel) LEDs with support for multiple lighting effects, color configurations, and dynamic events. The system is controlled by a dedicated FreeRTOS task for smooth operation.

## Hardware Configuration

- **LED Count**: 60 LEDs (configurable via `NUM_LEDS`)
- **Data Pin**: GPIO 3
- **Power Control**: GPIO 46
- **Power Supply**: 5V with current limiting

## Configuration Structure

### RGBConfig Structure

```cpp
struct RGBConfig {
    RGBEffectType effect = RGB_EFFECT_STATIC;  // Lighting effect
    const char* colors[MAX_COLORS];            // Array of hex colors
    uint8_t numColors = 0;                     // Number of colors used
    uint8_t brightness = 100;                  // Brightness (0-100%)
    uint8_t speed = 10;                        // Effect speed (1-20)
};
```

### Basic Setup

```cpp
#include "tasks/rgbHandler.h"

RGBConfig config;
config.effect = RGB_EFFECT_RUNNER;
config.colors[0] = "#FFA500";  // Orange
config.colors[1] = "#FF69B4";  // Hot Pink
config.colors[2] = "#800080";  // Purple
config.numColors = 3;
config.speed = 20;             // Fast
config.brightness = 43;        // 43% brightness

uRGB.configure(config);
```

## Available Effects

### RGB_EFFECT_STATIC
Solid color display using the first configured color.

**Parameters:**
- `colors[0]`: Color to display
- `brightness`: Overall brightness

**Example:**
```cpp
config.effect = RGB_EFFECT_STATIC;
config.colors[0] = "#FF0000";  // Red
config.numColors = 1;
```

### RGB_EFFECT_RUNNER
Animated runner effect cycling through configured colors.

**Parameters:**
- `colors[]`: Array of colors to cycle through
- `numColors`: Number of colors in cycle
- `speed`: Animation speed (1-20, higher = faster)
- `brightness`: Overall brightness

**Example:**
```cpp
config.effect = RGB_EFFECT_RUNNER;
config.colors[0] = "#FFA500";  // Orange
config.colors[1] = "#FF69B4";  // Hot Pink
config.colors[2] = "#800080";  // Purple
config.numColors = 3;
config.speed = 15;
```

### RGB_EFFECT_SCROLL
Scrolling rainbow effect across the LED strip.

**Parameters:**
- `speed`: Scroll speed (1-20)
- `brightness`: Overall brightness

**Example:**
```cpp
config.effect = RGB_EFFECT_SCROLL;
config.speed = 10;
config.brightness = 75;
```

### RGB_EFFECT_FLASH
Rapid flashing between configured colors.

**Parameters:**
- `colors[]`: Colors to flash between
- `numColors`: Number of colors
- `speed`: Flash frequency (1-20, higher = faster flashing)
- `brightness`: Overall brightness

**Example:**
```cpp
config.effect = RGB_EFFECT_FLASH;
config.colors[0] = "#FFFFFF";  // White
config.colors[1] = "#000000";  // Black
config.numColors = 2;
config.speed = 20;  // Very fast flashing
```

### RGB_EFFECT_BREATHE
Smooth breathing/pulsing effect.

**Parameters:**
- `colors[0]`: Base color
- `speed`: Breathing speed (1-20, higher = faster)
- `brightness`: Maximum brightness

**Example:**
```cpp
config.effect = RGB_EFFECT_BREATHE;
config.colors[0] = "#00FF00";  // Green
config.speed = 5;  // Slow breathing
config.brightness = 80;
```

## Event System

The RGB system responds to keyboard events:

### Connection Events

```cpp
// Module connected
uRGB.event(RGB_EVENT_MODULE_CONNECT);

// Module disconnected
uRGB.event(RGB_EVENT_MODULE_DISCONNECT);
```

### Automatic Event Responses

- **Module Connect**: Brief flash in green
- **Module Disconnect**: Brief flash in red
- **Key Press**: Subtle brightness increase (configurable)

## Brightness Control

### Global Brightness

```cpp
// Set maximum brightness percentage (affects all effects)
uRGB.setMaxBrightness(50);  // 50% of maximum
```

### Per-Effect Brightness

```cpp
config.brightness = 75;  // 75% brightness for this effect
```

## Color Format

Colors are specified as hex strings:
- Format: `#RRGGBB`
- Examples: `#FF0000` (red), `#00FF00` (green), `#0000FF` (blue)
- Case-insensitive

## Advanced Configuration

### Temporary Effects

```cpp
RGBCommand cmd;
cmd.type = RGB_CMD_SET_EFFECT;
cmd.data.effect.temporary = true;
cmd.data.effect.duration_ms = 5000;  // 5 seconds
// ... configure effect ...
xQueueSend(rgbCommandQueue, &cmd, 0);
```

### Dynamic Color Changes

```cpp
// Change individual colors
uRGB.setColor(0, "#FF0000", false);  // Set color 0 to red
uRGB.setColor(1, "#00FF00", false);  // Set color 1 to green

// Remove a color
uRGB.setColor(2, nullptr, true);     // Remove color 2
```

### Speed Control

```cpp
// Set effect speed
RGBCommand cmd;
cmd.type = RGB_CMD_SET_SPEED;
cmd.data.speed = 15;
xQueueSend(rgbCommandQueue, &cmd, 0);
```

## Performance Considerations

- **Memory Usage**: ~2KB RAM for LED buffer
- **CPU Usage**: Minimal when static, moderate during animations
- **Power Consumption**: Scales with brightness and LED count

## Troubleshooting

### LEDs Not Lighting
1. Check power supply (5V, adequate current)
2. Verify data pin connection (GPIO 3)
3. Confirm LED strip wiring (GND, 5V, Data)
4. Check `NUM_LEDS` definition matches actual count

### Effects Not Working
1. Verify effect type is valid
2. Check color strings are proper hex format
3. Ensure `numColors` matches configured colors
4. Confirm speed/brightness values are in valid ranges

### Performance Issues
1. Reduce `NUM_LEDS` if strip is shorter
2. Lower brightness for reduced power consumption
3. Decrease speed for smoother animation
4. Use static effects for minimal CPU usage

## Customization

### Adding New Effects

1. Add new effect to `RGBEffectType` enum
2. Implement effect logic in `applyCurrentEffect()`
3. Update configuration validation
4. Add effect documentation

### Hardware Modifications

- Change `LED_PIN` for different data pin
- Modify `NUM_LEDS` for different strip lengths
- Adjust power control pin if needed

## Integration Examples

### Arduino Setup Integration

```cpp
void setup() {
    // Initialize RGB system
    RGBConfig config;
    config.effect = RGB_EFFECT_RUNNER;
    config.colors[0] = "#FFA500";
    config.colors[1] = "#FF69B4";
    config.colors[2] = "#800080";
    config.numColors = 3;
    config.speed = 20;
    config.brightness = 43;

    uRGB.configure(config);
    uRGB.setMaxBrightness(24);  // 24% of max for power savings
}
```

### Event Response

```cpp
// In key press handler
if (keyPressed) {
    uRGB.event(RGB_EVENT_KEY_PRESS);
}
```

### Dynamic Reconfiguration

```cpp
// Change to breathe effect at night
if (currentHour >= 22 || currentHour <= 6) {
    RGBConfig nightConfig;
    nightConfig.effect = RGB_EFFECT_BREATHE;
    nightConfig.colors[0] = "#000033";  // Dark blue
    nightConfig.speed = 3;
    nightConfig.brightness = 20;
    uRGB.configure(nightConfig);
}
```

## Future Enhancements

Planned features:
- **RGB_EFFECT_WAVE**: Wave animation across keys
- **RGB_EFFECT_REACTIVE**: Key press reactive lighting
- **RGB_EFFECT_SPECTRUM**: Audio spectrum visualization
- **Per-key RGB control**: Individual key lighting
- **Profile system**: Multiple RGB configurations
- **Bluetooth sync**: RGB sync across multiple devices</content>
<parameter name="filePath">c:\Users\Gabii\Documents\PsychoDuck\PsychOS\source\RGB_DOCUMENTATION.md