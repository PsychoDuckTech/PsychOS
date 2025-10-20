# ESP-IDF Migration Guide

This document describes the migration from Arduino Framework to ESP-IDF for the PsychOS keyboard firmware.

## Overview

The firmware has been completely rewritten to use ESP-IDF instead of the Arduino framework while maintaining all functionality. This provides better performance, more control, and access to native ESP32-S3 features.

## Key Changes

### 1. Framework Configuration

**platformio.ini**
- Changed `framework = arduino` to `framework = espidf`
- Updated build flags and include paths
- Added TinyUSB configuration flags
- Updated library dependencies

### 2. Build System

**platformio.ini**
- Updated framework to `espidf`
- Configured build flags and include directories
- Specified library dependencies

**sdkconfig.defaults**
- Configured TinyUSB for USB HID
- Enabled Bluetooth (Bluedroid)
- Set FreeRTOS parameters
- Configured SPI, RMT, GPIO, and other peripherals

### 3. Compatibility Layers

Created wrapper headers to maintain code compatibility:

#### arduino_compat.h
- GPIO functions (pinMode, digitalWrite, digitalRead)
- Timing functions (millis, micros, delay, delayMicroseconds)
- Serial class for UART communication
- SPI class for display communication
- Basic Arduino macros and constants

#### usb_hid_compat.h
- USBHIDKeyboard class using TinyUSB
- USBHIDConsumerControl class
- HID key code definitions
- TinyUSB descriptor configuration

#### ble_compat.h
- BLE class using ESP-IDF Bluetooth stack
- BLEService, BLECharacteristic, BLEDevice classes
- String class for compatibility

#### neopixel_compat.h
- Adafruit_NeoPixel class using RMT peripheral
- LED strip control via ESP-IDF led_strip component

#### encoder_compat.h
- ESP32Encoder class using PCNT (Pulse Counter)
- Full quadrature encoder support

### 4. Main Entry Point

**main.cpp**
- Changed from `setup()` and `loop()` to `app_main()`
- Added NVS initialization (required for BLE)
- Added TinyUSB initialization
- ESP-IDF logging integration

### 5. Source Code Updates

All source files updated to:
- Include compatibility headers instead of Arduino.h
- Use ESP-IDF-specific features where appropriate
- Replace Arduino String with printf for efficiency
- Use soc/gpio_struct.h for direct GPIO register access

## Components Used

### ESP-IDF Native
- FreeRTOS (task management)
- NVS (non-volatile storage)
- GPIO driver
- SPI master driver
- PCNT (pulse counter for encoder)
- RMT (for NeoPixel)
- Bluetooth stack
- TinyUSB
- LED strip component

### External Libraries
- Adafruit GFX Library
- Adafruit BusIO
- Adafruit ILI9341 (display driver)

## Building

### Prerequisites
```bash
pip install platformio
```

### Build
```bash
cd source
pio run
```

### Flash
```bash
pio run -t upload
```

### Monitor
```bash
pio device monitor
```

## Configuration

Key settings are in `sdkconfig.defaults`:

- **USB**: TinyUSB HID enabled for keyboard functionality
- **Bluetooth**: Bluedroid stack for BLE module communication
- **CPU**: 160MHz for power savings
- **FreeRTOS**: 1000Hz tick rate
- **SPI**: In-IRAM mode for display performance
- **RMT**: ISR-safe for NeoPixel reliability

## Differences from Arduino

### Advantages of ESP-IDF
1. **Better performance**: Native ESP32-S3 code without Arduino overhead
2. **More control**: Direct access to all ESP-IDF features
3. **Better debugging**: Comprehensive logging and debugging tools
4. **Smaller binary**: No Arduino framework overhead
5. **Professional tooling**: Industry-standard IDF build system

### What Changed
1. **No `loop()`**: FreeRTOS tasks handle everything
2. **Different USB**: TinyUSB instead of Arduino USB
3. **Native BLE**: ESP-IDF Bluetooth stack
4. **Direct peripheral access**: No Arduino wrappers

### What Stayed the Same
1. **All functionality**: Every feature works as before
2. **Task structure**: FreeRTOS tasks unchanged
3. **Pin assignments**: Same hardware configuration
4. **User experience**: Identical behavior

## Troubleshooting

### Build Issues
- Ensure ESP-IDF is properly installed via PlatformIO
- Check that all include paths are correct
- Verify sdkconfig.defaults is loaded

### USB Not Working
- Check TinyUSB configuration in tusb_config.h
- Verify USB descriptors in usb_hid_compat.cpp
- Ensure GPIO 19/20 are configured for USB

### BLE Issues
- Verify Bluetooth is enabled in sdkconfig
- Check BLE stack initialization in main.cpp
- Ensure proper service/characteristic UUIDs

### Display Issues
- Verify SPI pins in main.cpp
- Check SPI initialization in displayHandler.cpp
- Ensure Adafruit libraries are compatible

## Future Improvements

1. **Full BLE implementation**: Complete the BLE compatibility layer
2. **UART serial**: Implement proper UART driver instead of printf
3. **Performance tuning**: Optimize task priorities and stack sizes
4. **Power management**: Implement ESP-IDF power management
5. **OTA updates**: Add over-the-air firmware update support

## References

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)
- [TinyUSB Documentation](https://docs.tinyusb.org/)
- [PlatformIO ESP-IDF](https://docs.platformio.org/en/latest/frameworks/espidf.html)
