#include <Adafruit_ILI9341.h>
#include "globals.h"         // For access to 'tft'
#include "freertos/semphr.h" // For FreeRTOS semaphore functions
#include "tasks/displayHandler.h"

extern SemaphoreHandle_t screenMutex; // Declare screenMutex as an external variable

extern Adafruit_ILI9341 tft; // Defined in globals.h

#include <Adafruit_ILI9341.h>

// Define the TFT display object (assumed to be initialized elsewhere)
extern Adafruit_ILI9341 tft;

// Duration of the flushing process (30 seconds in milliseconds)
const unsigned long FLUSH_DURATION_MS = 2 * 15 * 1000;

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

// Main function to perform harsh pixel flushing
void displayPixelFlushScreen(void *parameters)
{
    unsigned long startTime = millis();

    // Display a warning message briefly
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(10, 10);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.println("Harsh Pixel Flush");
    tft.println("In Progress...");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Show message for 1 second

    while (millis() - startTime < FLUSH_DURATION_MS)
    {
        // Phase 1: Rapid Color Cycling (30% of cycle)
        for (int i = 0; i < 50; i++)
        {
            tft.fillScreen(HARSH_COLORS[i % NUM_COLORS]);
            vTaskDelay(pdMS_TO_TICKS(5)); // Very fast transitions
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }

        // Phase 2: Moving Vertical Stripes (30% of cycle)
        for (int offset = 0; offset < 240; offset += 2)
        {
            for (int x = 0; x < 240; x++)
            {
                uint16_t color = ((x + offset) % 4 < 2) ? ILI9341_BLACK : ILI9341_WHITE;
                tft.drawFastVLine(x, 0, 320, color);
            }
            vTaskDelay(pdMS_TO_TICKS(10)); // Fast movement
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }

        // Phase 3: Moving Checkerboard (30% of cycle)
        for (int offset = 0; offset < 240; offset += 2)
        {
            for (int y = 0; y < 320; y += 8)
            {
                for (int x = 0; x < 240; x += 8)
                {
                    uint16_t color = ((x + y + offset) % 16 < 8) ? ILI9341_WHITE : ILI9341_BLACK;
                    tft.fillRect(x, y, 8, 8, color);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(10)); // Fast alternation
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }

        // Phase 4: Random Noise (10% of cycle)
        for (int i = 0; i < 10; i++)
        {
            tft.fillScreen(random(65536)); // Random 16-bit color
            vTaskDelay(pdMS_TO_TICKS(20)); // Slightly slower for noise effect
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }
    }

    // Clear the screen after completion
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(10, 10);
    tft.println("Flush Complete");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Show completion message
    switchScreen(MainScreen);
}