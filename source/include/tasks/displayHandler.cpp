#include <Adafruit_ILI9341.h> // no touch functionality due to having less pins than expected on the ESP32-S3 microcontroller ): sadge
#include "UI/main.h"          // main screen UI elements
#include "globals.h"          // global variables

#define LCD_CS 13
#define LCD_RST 17
#define LCD_RS 16
#define SPI_MOSI 12 // Connect to display's SDI (MOSI)
#define SPI_SCK 18  // Connect to display's SCK (clock)

extern int hours;
extern int minutes;
extern int seconds;
extern int updatedMinutes;

Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST);

void displayTime(void *parameters);
void clearTime(void *parameters);
void displayTopBar(void *parameters);
void clearTopBar(void *parameters);
void displayDemo(void *parameters);

void displayHandler(void *parameters)
{
    SPI.begin(SPI_SCK, -1, SPI_MOSI); // -1 means that the MISO pin is not used, due to espressif robbing me my pins iykyk
    tft.begin();
    tft.invertDisplay(true); // Ensure normal color mode
    tft.fillScreen(0x10A2);
    tft.setRotation(2);
    Serial.println("Display Handler started.");

    bool capsLockStatus = true;

    bool refreshFlags[3] = {false, false, false}; // [capsLockStatus, hour, minute]
    displayDemo(parameters);

    for (;;)
    {
        displayTopBar(parameters);

        if (updatedMinutes)
        {
            displayTime(parameters);
            updatedMinutes = false;
        }

        vTaskDelay(1 / portTICK_PERIOD_MS); // Delay for 10ms
    }
}

void displayTime(void *parameters)
{
    clearTime(parameters);
    tft.setTextColor(0xDED9);
    tft.setTextSize(4);
    tft.setFont(&FreeSansBold9pt7b);

    char timeString[3]; // Buffer to hold formatted time strings

    // Format with leading zeros
    sprintf(timeString, "%02d", hours);
    tft.setCursor(79, 100); // Set cursor position
    tft.print(timeString);

    // Format with leading zeros
    sprintf(timeString, "%02d", minutes);
    tft.setCursor(79, 175);
    tft.print(timeString);
}

void displayTopBar(void *parameters)
{
    tft.drawBitmap(11, 9, image_menu_settings_sliders_bits, 14, 16, 0xDED9);

    tft.drawBitmap(157, 9, image_cloud_sync_bits, 17, 16, connectionStatus ? 0x9C1F : 0xF22B);

    tft.drawBitmap(182, 9, image_bluetooth_connected_bits, 14, 16, moduleConnectionStatus ? 0x9C1F : 0xF22B);

    tft.setTextSize(1);
    tft.setTextColor(0xDED9);
    tft.setFont();
    tft.setCursor(206, 8);
    tft.print("Caps");

    tft.setTextColor(capsLockStatus ? 0xFD40 : 0x7BEF); // ON or OFF
    tft.fillRect(206, 18, 17, 8, 0x10A2);
    tft.setCursor(206, 18);
    tft.print(capsLockStatus ? "ON" : "OFF");
}

/*void clearTopBar()
{
    tft.fillRect(0, 0, ILI9341_TFTWIDTH, 40, 0x10A2);
}*/

void clearTime(void *parameters)
{
    tft.fillRect(75, 51, 90, 130, 0x10A2);
}

void displayDemo(void *parameters)
{

    // Layer 7
    tft.setTextSize(1);
    tft.setFont(&FreeMonoBold12pt7b);
    tft.setCursor(18, 250);
    tft.print("Now Playing");

    // Layer 7
    tft.setFont(&FreeMonoBold9pt7b);
    tft.setCursor(18, 274);
    tft.print("Bury you - Ari.");

    // music_play button
    tft.drawBitmap(198, 263, image_music_play_bits, 15, 16, 0xDED9);

    // Layer 11
    tft.setFont();
    tft.setTextColor(0xFD40);
    tft.setCursor(90, 304);
    tft.print("3D Modeling");
}