#include <Adafruit_ILI9341.h>
#include "globals.h"         // For access to 'tft'
#include "freertos/semphr.h" // For FreeRTOS semaphore functions
#include "tasks/displayHandler.h"
#include "display/components/uiComponents.h"
#include "display/pixelFlushScreen.h" // Include the new header file
#include "translations.h"
#include "display/icons.h" // Include icons for iconArrowLeft and iconArrowRight
#include "tasks/hostCommunicationBridge.h" // Add this include for HostMessage and queue

extern SemaphoreHandle_t screenMutex;
extern Adafruit_ILI9341 tft;

// Screen dimensions
constexpr int SCREEN_WIDTH = 240;
constexpr int SCREEN_HEIGHT = 320;

// Progress bar constants
constexpr int PROGRESS_BAR_HEIGHT = 15;
constexpr int PROGRESS_BAR_Y = 0;
constexpr int ANIMATION_START_Y = PROGRESS_BAR_HEIGHT; // Start animations below progress bar
constexpr int ANIMATION_HEIGHT = SCREEN_HEIGHT - PROGRESS_BAR_HEIGHT;

// Pixel flush state variables
volatile bool pixelFlushRunning = false;
volatile bool pixelFlushCancelled = false;
int selectedDurationIndex = 2; // Default to 1 minute

// Duration options (in seconds for values < 60, minutes otherwise)
const int durationOptions[] = {30, 45, 60, 120, 300, 600, 900, 1800, 3600};
const int numDurationOptions = sizeof(durationOptions) / sizeof(durationOptions[0]);

// Timing constants (in milliseconds)
unsigned long FLUSH_DURATION_MS = durationOptions[selectedDurationIndex] * 60 * 1000; // Default duration
constexpr unsigned long COLOR_CYCLE_DELAY_MS = 5;                                     // Delay for color cycling
constexpr unsigned long STRIPE_DELAY_MS = 10;                                         // Delay for stripes
constexpr unsigned long CHECKER_DELAY_MS = 10;                                        // Delay for checkerboard
constexpr unsigned long NOISE_DELAY_MS = 20;                                          // Delay for noise effect
constexpr unsigned long COMPLETION_DISPLAY_MS = 1500;                                 // Show completion message duration

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
void displayProgress(unsigned long currentTime, unsigned long startTime)
{
    static unsigned long lastProgressUpdate = 0;
    const unsigned long PROGRESS_UPDATE_INTERVAL_MS = 200; // Update every 200ms for smooth bar
    
    // Only update if enough time has passed since last update
    if (currentTime - lastProgressUpdate >= PROGRESS_UPDATE_INTERVAL_MS)
    {
        int progress = ((currentTime - startTime) * 100) / FLUSH_DURATION_MS;
        
        // Draw progress bar background (full width, top of screen)
        tft.fillRect(0, PROGRESS_BAR_Y, SCREEN_WIDTH, PROGRESS_BAR_HEIGHT, ILI9341_BLACK);
        
        // Draw progress fill (green bar)
        int fillWidth = (progress * SCREEN_WIDTH) / 100;
        if (fillWidth > 0) {
            tft.fillRect(0, PROGRESS_BAR_Y, fillWidth, PROGRESS_BAR_HEIGHT, ILI9341_GREEN);
        }
        
        // Draw progress percentage text in the center of the bar
        tft.setTextSize(1);
        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(SCREEN_WIDTH / 2 - 15, PROGRESS_BAR_Y + 4);
        tft.printf("%d%%", progress);
        
        lastProgressUpdate = currentTime;
    }
}

void runColorCycling(unsigned long &startTime)
{
    // Phase 1: Rapid Color Cycling (30% of cycle)
    for (int i = 0; i < NUM_COLOR_CYCLES && (millis() - startTime < FLUSH_DURATION_MS) && !pixelFlushCancelled; i++)
    {
        // Fill only the animation area, leaving progress bar intact
        tft.fillRect(0, ANIMATION_START_Y, SCREEN_WIDTH, ANIMATION_HEIGHT, HARSH_COLORS[i % NUM_COLORS]);
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(COLOR_CYCLE_DELAY_MS));
    }
}

void runVerticalStripes(unsigned long &startTime)
{
    // Phase 2: Moving Vertical Stripes (30% of cycle)
    for (int offset = 0; offset < SCREEN_WIDTH && (millis() - startTime < FLUSH_DURATION_MS) && !pixelFlushCancelled; offset += STRIPE_STEP)
    {
        // Optimized: draw 2-pixel wide vertical bars instead of individual lines
        for (int x = 0; x < SCREEN_WIDTH; x += 2)
        {
            uint16_t color = ((x + offset) % 4 < 2) ? ILI9341_BLACK : ILI9341_WHITE;
            tft.fillRect(x, ANIMATION_START_Y, 2, ANIMATION_HEIGHT, color);
        }
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(STRIPE_DELAY_MS));
    }
}

void runCheckerboard(unsigned long &startTime)
{
    // Phase 3: Moving Checkerboard (30% of cycle)
    for (int offset = 0; offset < SCREEN_WIDTH && (millis() - startTime < FLUSH_DURATION_MS) && !pixelFlushCancelled; offset += STRIPE_STEP)
    {
        for (int y = ANIMATION_START_Y; y < SCREEN_HEIGHT; y += CHECKERBOARD_SIZE)
        {
            for (int x = 0; x < SCREEN_WIDTH; x += CHECKERBOARD_SIZE)
            {
                uint16_t color = ((x + y + offset) % 16 < 8) ? ILI9341_WHITE : ILI9341_BLACK;
                tft.fillRect(x, y, CHECKERBOARD_SIZE, CHECKERBOARD_SIZE, color);
            }
        }
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(CHECKER_DELAY_MS));
    }
}

void runRandomNoise(unsigned long &startTime)
{
    // Phase 4: Random Noise (10% of cycle)
    for (int i = 0; i < NUM_NOISE_FRAMES && (millis() - startTime < FLUSH_DURATION_MS) && !pixelFlushCancelled; i++)
    {
        // Fill only the animation area with random colors
        for (int y = ANIMATION_START_Y; y < SCREEN_HEIGHT; y++)
        {
            for (int x = 0; x < SCREEN_WIDTH; x++)
            {
                tft.drawPixel(x, y, random(65536));
            }
        }
        displayProgress(millis(), startTime);
        vTaskDelay(pdMS_TO_TICKS(NOISE_DELAY_MS));
    }
}

// Function declarations
void drawDurationSelector();
void startFlushProcess();

// Initialize the pixel flush screen with duration selection
void displayPixelFlushScreen(void *parameters)
{    // Reset state
    pixelFlushCancelled = false;
    pixelFlushRunning = false;

    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);

    // Display title
    drawScreenTitle("Screen Flush", 30);    // Draw warning box (positioned below duration selector)
    drawFrame(15, 140, 210, 70, ERROR_COLOR, 1);
      // Display warning icon (using error color)
    tft.fillTriangle(43, 160, 58, 185, 28, 185, ERROR_COLOR);
    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(2);
    tft.setCursor(39, 168);
    tft.print("!");
    
    // Warning text
    tft.setTextColor(ERROR_COLOR);
    tft.setTextSize(1);
    tft.setCursor(70, 155);
    tft.println("WARNING:");
    tft.setCursor(70, 168);
    tft.setTextColor(ILI9341_WHITE);
    tft.println("This screen contains");
    tft.setCursor(70, 178);
    tft.println("flashing patterns that");
    tft.setCursor(70, 188);
    tft.println("may trigger seizures");

    // Display duration selection screen
    drawDurationSelector();

    // Draw custom help indicators that match actual button functions
    tft.setTextSize(1);
    tft.setFont();
    tft.setTextColor(ILI9341_WHITE);

    // Custom help text for duration selection screen
    tft.setCursor(15, 250);
    tft.print("Rotate to select duration");
    tft.setCursor(15, 258);
    tft.print("Press to start");
    tft.setCursor(15, 266);
    tft.print("Long press to exit");
}

// Draw the duration selection interface
void drawDurationSelector()
{
    tft.fillRect(0, 60, SCREEN_WIDTH, 80, ILI9341_BLACK); // Clear only the duration selector area

    // Draw instruction text
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(20, 65);
    tft.println("Select flush duration:");

    // Format duration string based on seconds vs minutes
    char durationStr[20];
    if (durationOptions[selectedDurationIndex] < 60)
    {
        sprintf(durationStr, "%d seconds", durationOptions[selectedDurationIndex]);
    }
    else
    {
        sprintf(durationStr, "%d minutes", durationOptions[selectedDurationIndex] / 60);
    }

    // Use the standard slider button with text component for consistent UI
    const int DURATION_Y = 85;

    // Use the exact same style as the first slider in underglow menu
    drawSliderButtonWithText("Duration", durationStr,
                             nullptr, 0, 0,
                             DURATION_Y, true);
}

// Prepare the flush screen UI and start the process
void startFlushProcess()
{
    // Update the duration based on user selection - convert seconds directly to milliseconds
    FLUSH_DURATION_MS = durationOptions[selectedDurationIndex] * 1000;

    // Clear the animation area (leave progress bar area for now)
    tft.fillRect(0, ANIMATION_START_Y, SCREEN_WIDTH, ANIMATION_HEIGHT, ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);

    // Display title in animation area
    tft.setTextSize(2);
    tft.setCursor(10, ANIMATION_START_Y + 10);
    tft.println("Pixel Flushing");

    // Show selected duration
    tft.setTextSize(1);
    tft.setCursor(10, ANIMATION_START_Y + 40);
    tft.print("Duration: ");
    tft.print(durationOptions[selectedDurationIndex]);
    tft.println(" minutes");

    // Draw initial progress bar
    displayProgress(0, 0); // Force initial draw

    // Mark flushing as running
    pixelFlushRunning = true;
}

// Start the actual pixel flushing process in a separate task
void startPixelFlush(void *parameters)
{
    // Wait for user to press the button on the selection screen
    while (!pixelFlushRunning && !pixelFlushCancelled)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // If cancelled, end the task
    if (pixelFlushCancelled)
    {
        pixelFlushComplete = true;
        vTaskDelete(NULL);
    }

    startFlushProcess();
    unsigned long startTime = millis();

    while (millis() - startTime < FLUSH_DURATION_MS)
    {
        // Check for cancellation
        if (pixelFlushCancelled)
        {
            break;
        }

        runColorCycling(startTime);
        if (millis() - startTime >= FLUSH_DURATION_MS || pixelFlushCancelled)
            break;

        runVerticalStripes(startTime);
        if (millis() - startTime >= FLUSH_DURATION_MS || pixelFlushCancelled)
            break;

        runCheckerboard(startTime);
        if (millis() - startTime >= FLUSH_DURATION_MS || pixelFlushCancelled)
            break;

        runRandomNoise(startTime);
    }

    // If cancelled, show cancellation message
    if (pixelFlushCancelled)
    {
        // Clear animation area but keep progress bar
        tft.fillRect(0, ANIMATION_START_Y, SCREEN_WIDTH, ANIMATION_HEIGHT, ILI9341_BLACK);
        tft.setCursor(10, ANIMATION_START_Y + 10);
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.println("Flushing Cancelled");
        tft.setTextSize(1);
        tft.setCursor(10, ANIMATION_START_Y + 50);
        unsigned long elapsedTime = millis() - startTime;
        int progress = (elapsedTime * 100) / FLUSH_DURATION_MS;
        tft.printf("Progress: %d%%", progress);
    }
    else
    {
        // Show completion message
        // Clear animation area but keep progress bar
        tft.fillRect(0, ANIMATION_START_Y, SCREEN_WIDTH, ANIMATION_HEIGHT, ILI9341_BLACK);
        tft.setCursor(10, ANIMATION_START_Y + 10);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.println("Flushing Complete");
        tft.setTextSize(1);
        tft.setCursor(10, ANIMATION_START_Y + 50);
        tft.println("Progress: 100%");
    }

    vTaskDelay(pdMS_TO_TICKS(COMPLETION_DISPLAY_MS));
    pixelFlushComplete = true;
    pixelFlushRunning = false;
    vTaskDelete(NULL);
}

// Handle knob rotation for pixel flush screen
void handlePixelFlushKnobRotation(int direction)
{
    // If we're running the flush, control volume
    if (pixelFlushRunning)
    {
        HostMessage msg;
        msg.type = VOLUME_CHANGE;
        msg.data = direction;
        xQueueSend(hostMessageQueue, &msg, 0);
        return;
    }

    static int accumulatedSteps = 0;
    accumulatedSteps += direction;

    // Only change duration when accumulated steps reach a threshold of 2
    if (abs(accumulatedSteps) >= 2)
    {
        // Update the selected duration index
        if (accumulatedSteps > 0)
        {
            // Rotate right - increase duration
            selectedDurationIndex = (selectedDurationIndex + 1) % numDurationOptions;
        }
        else
        {
            // Rotate left - decrease duration
            selectedDurationIndex = (selectedDurationIndex + numDurationOptions - 1) % numDurationOptions;
        }

        // Reset accumulated steps
        accumulatedSteps = 0;

        // Redraw the selection screen with the new duration
        drawDurationSelector();
    }
}

// Handle knob press for pixel flush screen
void handlePixelFlushKnobPress()
{
    // If we're in the selection screen, start the flush
    if (!pixelFlushRunning)
    {
        pixelFlushRunning = true;
    }
}

// Handle long press for pixel flush screen
void handlePixelFlushLongPress()
{
    // Cancel the flush
    pixelFlushCancelled = true;

    // If we're still in selection mode, exit to main screen
    if (!pixelFlushRunning)
    {
        switchScreen(MainScreen);
    }
}