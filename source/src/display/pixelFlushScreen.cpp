#include <Adafruit_ILI9341.h>
#include "globals.h"         // For access to 'tft'
#include "freertos/semphr.h" // For FreeRTOS semaphore functions
#include "tasks/displayHandler.h"

extern SemaphoreHandle_t screenMutex;
extern Adafruit_ILI9341 tft;

// Screen dimensions
constexpr int SCREEN_WIDTH = 240;
constexpr int SCREEN_HEIGHT = 320;

// Timing constants (in milliseconds)
constexpr unsigned long FLUSH_DURATION_MS = 5 * 15 * 1000;  // Total duration
constexpr unsigned long COLOR_CYCLE_DELAY_MS = 5;           // Delay for color cycling
constexpr unsigned long STRIPE_DELAY_MS = 10;               // Delay for stripes
constexpr unsigned long CHECKER_DELAY_MS = 10;              // Delay for checkerboard
constexpr unsigned long NOISE_DELAY_MS = 20;                // Delay for noise effect
constexpr unsigned long COMPLETION_DISPLAY_MS = 1500;       // Show completion message duration

// Pattern constants
constexpr int CHECKERBOARD_SIZE = 8;
constexpr int STRIPE_STEP = 2;
constexpr int NUM_NOISE_FRAMES = 10;
constexpr int NUM_COLOR_CYCLES = 50;

// High-contrast colors for rapid cycling
const uint16_t HARSH_COLORS[] = {
    ILI9341_BLACK,   // #000000
    ILI9341_WHITE,   // #FFFFFF
    ILI9341_RED,     // #FF0000
    ILI9341_CYAN,    // #00FFFF (complement of red)
    ILI9341_GREEN,   // #00FF00
    ILI9341_MAGENTA, // #FF00FF (complement of green)
    ILI9341_BLUE,    // #0000FF
    ILI9341_YELLOW   // #FFFF00 (complement of blue)
};
const int NUM_COLORS = sizeof(HARSH_COLORS) / sizeof(HARSH_COLORS[0]);

// Helper functions for each phase
void displayProgress(unsigned long currentTime, unsigned long startTime) {
    int progress = ((currentTime - startTime) * 100) / FLUSH_DURATION_MS;
    tft.fillRect(10, 50, 220, 20, ILI9341_BLACK);  // Clear previous progress
    tft.setTextSize(1);
    tft.setCursor(10, 50);
    tft.printf("Progress: %d%%", progress);
}

void runColorCycling(unsigned long& startTime) {
    // Phase 1: Rapid Color Cycling (30% of cycle)
    for (int i = 0; i < NUM_COLOR_CYCLES && (millis() - startTime < FLUSH_DURATION_MS); i++) {
        tft.fillScreen(HARSH_COLORS[i % NUM_COLORS]);
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(COLOR_CYCLE_DELAY_MS));
    }
}

void runVerticalStripes(unsigned long& startTime) {
    // Phase 2: Moving Vertical Stripes (30% of cycle)
    for (int offset = 0; offset < SCREEN_WIDTH && (millis() - startTime < FLUSH_DURATION_MS); offset += STRIPE_STEP) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint16_t color = ((x + offset) % 4 < 2) ? ILI9341_BLACK : ILI9341_WHITE;
            tft.drawFastVLine(x, 0, SCREEN_HEIGHT, color);
        }
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(STRIPE_DELAY_MS));
    }
}

void runCheckerboard(unsigned long& startTime) {
    // Phase 3: Moving Checkerboard (30% of cycle)
    for (int offset = 0; offset < SCREEN_WIDTH && (millis() - startTime < FLUSH_DURATION_MS); offset += STRIPE_STEP) {
        for (int y = 0; y < SCREEN_HEIGHT; y += CHECKERBOARD_SIZE) {
            for (int x = 0; x < SCREEN_WIDTH; x += CHECKERBOARD_SIZE) {
                uint16_t color = ((x + y + offset) % 16 < 8) ? ILI9341_WHITE : ILI9341_BLACK;
                tft.fillRect(x, y, CHECKERBOARD_SIZE, CHECKERBOARD_SIZE, color);
            }
        }
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(CHECKER_DELAY_MS));
    }
}

void runRandomNoise(unsigned long& startTime) {
    // Phase 4: Random Noise (10% of cycle)
    for (int i = 0; i < NUM_NOISE_FRAMES && (millis() - startTime < FLUSH_DURATION_MS); i++) {
        tft.fillScreen(random(65536));
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(NOISE_DELAY_MS));
    }
}

// Initialize the pixel flush screen
void displayPixelFlushScreen(void *parameters) {
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    
    // Display title
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Pixel Flushing");
    tft.println("Starting...");
    
    // Display initial progress
    tft.setTextSize(1);
    tft.setCursor(10, 50);
    tft.println("Progress: 0%");
}

// Start the actual pixel flushing process in a separate task
void startPixelFlush(void *parameters) {
    vTaskDelay(pdMS_TO_TICKS(1000)); // Show initial message for 1 second
    unsigned long startTime = millis();

    while (millis() - startTime < FLUSH_DURATION_MS) {
        runColorCycling(startTime);
        if (millis() - startTime >= FLUSH_DURATION_MS) break;
        
        runVerticalStripes(startTime);
        if (millis() - startTime >= FLUSH_DURATION_MS) break;
        
        runCheckerboard(startTime);
        if (millis() - startTime >= FLUSH_DURATION_MS) break;
        
        runRandomNoise(startTime);
    }

    // Show completion message
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(10, 10);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.println("Flushing Done");
    tft.setTextSize(1);
    tft.setCursor(10, 50);
    tft.println("Progress: 100%");
    
    vTaskDelay(pdMS_TO_TICKS(COMPLETION_DISPLAY_MS));
    pixelFlushComplete = true;
    vTaskDelete(NULL);
}