#pragma once

#include "arduino_compat.h"
#include "driver/rmt_tx.h"
#include "led_strip.h"

// NeoPixel color orders
#define NEO_GRB 0
#define NEO_KHZ800 0

class Adafruit_NeoPixel {
private:
    led_strip_handle_t _strip;
    uint16_t _numLEDs;
    uint8_t _pin;
    uint8_t _brightness;
    bool _initialized;
    uint8_t* _pixels;  // RGB buffer
    
public:
    Adafruit_NeoPixel(uint16_t n, uint8_t pin, uint8_t type) 
        : _numLEDs(n), _pin(pin), _brightness(255), _initialized(false), _strip(NULL) {
        _pixels = (uint8_t*)malloc(n * 3);  // RGB: 3 bytes per pixel
        if (_pixels) {
            memset(_pixels, 0, n * 3);
        }
    }
    
    ~Adafruit_NeoPixel() {
        if (_pixels) {
            free(_pixels);
        }
        if (_strip) {
            led_strip_del(_strip);
        }
    }
    
    void begin() {
        if (_initialized) return;
        
        // Configure RMT for LED strip
        led_strip_config_t strip_config = {
            .strip_gpio_num = _pin,
            .max_leds = _numLEDs,
            .led_pixel_format = LED_PIXEL_FORMAT_GRB,
            .led_model = LED_MODEL_WS2812,
            .flags = {
                .invert_out = false,
            }
        };
        
        led_strip_rmt_config_t rmt_config = {
            .clk_src = RMT_CLK_SRC_DEFAULT,
            .resolution_hz = 10 * 1000 * 1000, // 10MHz
            .flags = {
                .with_dma = false,
            }
        };
        
        ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &_strip));
        _initialized = true;
        clear();
        show();
    }
    
    void show() {
        if (!_initialized || !_strip) return;
        
        // Apply brightness and update all LEDs
        for (uint16_t i = 0; i < _numLEDs; i++) {
            uint8_t r = (_pixels[i * 3] * _brightness) / 255;
            uint8_t g = (_pixels[i * 3 + 1] * _brightness) / 255;
            uint8_t b = (_pixels[i * 3 + 2] * _brightness) / 255;
            led_strip_set_pixel(_strip, i, r, g, b);
        }
        led_strip_refresh(_strip);
    }
    
    void setPixelColor(uint16_t n, uint32_t color) {
        if (n >= _numLEDs || !_pixels) return;
        
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        
        _pixels[n * 3] = r;
        _pixels[n * 3 + 1] = g;
        _pixels[n * 3 + 2] = b;
    }
    
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
        if (n >= _numLEDs || !_pixels) return;
        
        _pixels[n * 3] = r;
        _pixels[n * 3 + 1] = g;
        _pixels[n * 3 + 2] = b;
    }
    
    void setBrightness(uint8_t brightness) {
        _brightness = brightness;
    }
    
    uint8_t getBrightness() const {
        return _brightness;
    }
    
    void clear() {
        if (_pixels) {
            memset(_pixels, 0, _numLEDs * 3);
        }
    }
    
    uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }
    
    uint32_t getPixelColor(uint16_t n) {
        if (n >= _numLEDs || !_pixels) return 0;
        
        uint8_t r = _pixels[n * 3];
        uint8_t g = _pixels[n * 3 + 1];
        uint8_t b = _pixels[n * 3 + 2];
        
        return Color(r, g, b);
    }
    
    uint16_t numPixels() const {
        return _numLEDs;
    }
};
