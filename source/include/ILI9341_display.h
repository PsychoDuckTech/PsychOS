#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

class ILI9341Display {
private:
    // SPI configuration
    spi_device_handle_t spi_device;
    
    // Pin configurations
    gpio_num_t dc_pin;     // Data/Command pin
    gpio_num_t rst_pin;    // Reset pin
    gpio_num_t cs_pin;     // Chip Select pin

    // Synchronization primitive
    SemaphoreHandle_t display_mutex;

    // Display configuration constants
    static constexpr uint16_t WIDTH = 320;
    static constexpr uint16_t HEIGHT = 240;

    // Initialization sequence (converted from Python driver)
    static constexpr uint8_t INIT_SEQUENCE[] = {
        // Software reset
        0x01, 0x80, 0x80,  // Software reset with 128ms delay
        
        // Various initialization commands from Python driver
        0xEF, 0x03, 0x03, 0x80, 0x02,
        0xCF, 0x03, 0x00, 0xC1, 0x30,
        0xED, 0x04, 0x64, 0x03, 0x12, 0x81,
        0xE8, 0x03, 0x85, 0x00, 0x78,
        0xCB, 0x05, 0x39, 0x2C, 0x00, 0x34, 0x02,
        0xF7, 0x01, 0x20,
        0xEA, 0x02, 0x00, 0x00,
        0xC0, 0x01, 0x23,  // Power control
        0xC1, 0x01, 0x10,  // Power control
        0xC5, 0x02, 0x3E, 0x28,  // VCM control
        0xC7, 0x01, 0x86,  // VCM control2
        0x37, 0x01, 0x00,  // Vertical scroll zero
        0x3A, 0x01, 0x55,  // Pixel Format Set
        0xB1, 0x02, 0x00, 0x18,  // Frame Rate Control
        0xB6, 0x03, 0x08, 0x82, 0x27,  // Display Function Control
        0xF2, 0x01, 0x00,  // 3Gamma Function Disable
        0x26, 0x01, 0x01,  // Gamma curve selected
        
        // Gamma settings (very important to preserve from original)
        0xE0, 0x0F, 
        0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 
        0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
        
        0xE1, 0x0F, 
        0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 
        0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        
        // Exit sleep and turn on display
        0x11, 0x80, 0x78,  // Exit sleep with 120ms delay
        0x29, 0x80, 0x78   // Display on with 120ms delay
    };

    // Private methods
    void send_command(uint8_t cmd);
    void send_data(const uint8_t* data, size_t len);
    void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

public:
    ILI9341Display(
        gpio_num_t mosi, 
        gpio_num_t sclk, 
        gpio_num_t cs, 
        gpio_num_t dc, 
        gpio_num_t rst
    );

    bool initialize();
    void fill_screen(uint16_t color);
    void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
    void draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

    // Color conversion utilities
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
};

// Implementation file would follow with method definitions