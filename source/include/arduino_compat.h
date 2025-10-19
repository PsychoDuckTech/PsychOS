#pragma once

// ESP-IDF includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

// Arduino compatibility layer for ESP-IDF

// Pin modes
#define INPUT           GPIO_MODE_INPUT
#define OUTPUT          GPIO_MODE_OUTPUT
#define INPUT_PULLUP    GPIO_MODE_INPUT
#define INPUT_PULLDOWN  GPIO_MODE_INPUT

// Pin values
#define HIGH 1
#define LOW  0

// Constrain macro
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

// Min/Max macros
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

// Get milliseconds since boot
static inline unsigned long millis() {
    return (unsigned long)(esp_timer_get_time() / 1000ULL);
}

// Get microseconds since boot
static inline unsigned long micros() {
    return (unsigned long)esp_timer_get_time();
}

// Delay functions
static inline void delay(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

static inline void delayMicroseconds(uint32_t us) {
    esp_rom_delay_us(us);
}

// GPIO functions
static inline void pinMode(uint8_t pin, uint8_t mode) {
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.mode = (gpio_mode_t)mode;
    
    if (mode == INPUT_PULLUP) {
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    } else if (mode == INPUT_PULLDOWN) {
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    } else {
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}

static inline void digitalWrite(uint8_t pin, uint8_t val) {
    gpio_set_level((gpio_num_t)pin, val);
}

static inline int digitalRead(uint8_t pin) {
    return gpio_get_level((gpio_num_t)pin);
}

// Serial compatibility - using printf/puts for now
class SerialClass {
public:
    void begin(unsigned long baud) {
        // UART is already initialized by ESP-IDF
    }
    
    void print(const char* str) {
        printf("%s", str);
    }
    
    void print(int val) {
        printf("%d", val);
    }
    
    void print(unsigned int val) {
        printf("%u", val);
    }
    
    void print(long val) {
        printf("%ld", val);
    }
    
    void print(unsigned long val) {
        printf("%lu", val);
    }
    
    void print(float val) {
        printf("%f", val);
    }
    
    void println(const char* str = "") {
        printf("%s\n", str);
    }
    
    void println(int val) {
        printf("%d\n", val);
    }
    
    void println(unsigned int val) {
        printf("%u\n", val);
    }
    
    void println(long val) {
        printf("%ld\n", val);
    }
    
    void println(unsigned long val) {
        printf("%lu\n", val);
    }
    
    void println(float val) {
        printf("%f\n", val);
    }
    
    void printf(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
    
    int available() {
        // Simplified - would need proper UART implementation
        return 0;
    }
    
    int read() {
        // Simplified - would need proper UART implementation
        return -1;
    }
};

extern SerialClass Serial;

// CPU frequency control
static inline bool setCpuFrequencyMhz(uint32_t freq) {
    // ESP-IDF handles this through menuconfig
    return true;
}

// GPIO register access compatibility
// In ESP-IDF, GPIO register access is through the GPIO struct
// This is already defined in soc/gpio_struct.h
// No need to redefine it here

// Legacy delay function
static inline void ets_delay_us(uint32_t us) {
    esp_rom_delay_us(us);
}
