#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "display/screens.h"
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"
#include "tasks/displayHandler.h"

extern SemaphoreHandle_t screenMutex;
extern Adafruit_ILI9341 tft;
extern bool needsFullRedraw;
extern int settingsSelectedOption;

void drawSettingsStatic(void *parameters)
{
    tft.fillScreen(BG_COLOR);
    // Draw title
    drawScreenTitle(ui_settings);

    // Draw footer text before options are loaded
    tft.setTextSize(1);
    tft.setFont();
    tft.setTextColor(ULTRA_MUTED_COLOR);
    tft.setCursor(76, 287);
    tft.print(ui_secured_by);
    tft.setTextColor(MUTED_COLOR);
    tft.setCursor(65, 296);
    tft.print(ui_powered_by);
    tft.setCursor(79, 305);
    tft.print(ui_build);
    tft.print(" ");
    tft.print(OS_version);
}

void displayPixelFlushScreen()
{
    xSemaphoreTake(screenMutex, portMAX_DELAY); // Take mutex before accessing display

    // 40 seconds in milliseconds
    const unsigned long FLUSH_DURATION_MS = 40 * 1000;
    const uint16_t HARSH_COLORS[] = {
        ILI9341_BLACK, ILI9341_WHITE, ILI9341_RED, ILI9341_CYAN,
        ILI9341_GREEN, ILI9341_MAGENTA, ILI9341_BLUE, ILI9341_YELLOW};
    const int NUM_COLORS = sizeof(HARSH_COLORS) / sizeof(HARSH_COLORS[0]);
    unsigned long startTime = millis();
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(10, 10);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.println("Harsh Pixel Flush");
    tft.println("In Progress...");
    delay(1000);
    while (millis() - startTime < FLUSH_DURATION_MS)
    {
        for (int i = 0; i < 50; i++)
        {
            tft.fillScreen(HARSH_COLORS[i % NUM_COLORS]);
            delay(5);
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }
        for (int offset = 0; offset < 240; offset += 2)
        {
            for (int x = 0; x < 240; x++)
            {
                uint16_t color = ((x + offset) % 4 < 2) ? ILI9341_BLACK : ILI9341_WHITE;
                tft.drawFastVLine(x, 0, 320, color);
            }
            delay(10);
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }
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
            delay(10);
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }
        for (int i = 0; i < 10; i++)
        {
            tft.fillScreen(random(65536));
            delay(20);
            if (millis() - startTime >= FLUSH_DURATION_MS)
                break;
        }
    }
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(10, 10);
    tft.println("Flush Complete");
    delay(1000);

    xSemaphoreGive(screenMutex); // Release mutex after we're done
}

void displaySettingsScreen(void *parameters)
{
    if (needsFullRedraw)
    {
        drawSettingsStatic(parameters);
        needsFullRedraw = false;
    }
    tft.fillRect(4, 60, 232, 215, BG_COLOR);
    const int MENU_START_Y = 62;
    const int ITEM_SPACING = 54;
    const char *menuItems[] = {ui_modules, ui_underglow, ui_clock, ui_pixel_flush};
    const uint8_t *icons[] = {iconBleConnectedBig, iconLightBulb, iconTranslation, nullptr};
    const int iconWidths[] = {14, 18, 22, 22, 0};
    const int iconHeights[] = {22, 23, 22, 22, 0};
    for (int i = 0; i < 4; i++)
    {
        bool selected = (i == settingsSelectedOption);
        int baseY = MENU_START_Y + (i * ITEM_SPACING);
        drawButton(menuItems[i], icons[i], iconWidths[i], iconHeights[i], true, baseY, selected);
    }
}