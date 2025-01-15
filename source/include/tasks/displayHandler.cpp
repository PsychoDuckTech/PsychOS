#include <Adafruit_ILI9341.h>
// no touch functionality due to having less pins than expected on the ESP32-S3 microcontroller ):
#include "UI/main.h" // main screen UI elements

#define LCD_CS 13
#define LCD_RST 17
#define LCD_RS 16
#define SPI_MOSI 12 // Connect to display's SDI (MOSI)
#define SPI_SCK 18  // Connect to display's SCK (clock)

Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST);

void displayHandler(void *parameters)
{
    SPI.begin(SPI_SCK, -1, SPI_MOSI); // -1 means that the MISO pin is not used, due to espressif robbing me my pins iykyk
    tft.begin();
    tft.invertDisplay(true); // Ensure normal color mode
    tft.setRotation(2);
    Serial.println("Display Handler started.");

    bool capsLockStatus = false; // Replace this with actual logic to check Caps Lock
    int hour = 10;               // Replace with actual hour value
    int minute = 36;             // Replace with actual minute value

    for (;;)
    {
        String line = Serial.readStringUntil('\n');
        Serial.println(line);

        // Parse the received data
        int comma1 = line.indexOf(',');
        int comma2 = line.lastIndexOf(',');

        bool capsLockStatus = line.substring(0, comma1).toInt() == 1;
        int hour = line.substring(comma1 + 1, comma2).toInt();
        int minute = line.substring(comma2 + 1).toInt();
        tft.fillScreen(ILI9341_BLACK);

        // bluetooth_connected
        tft.drawBitmap(182, 9, image_bluetooth_connected_bits, 14, 16, 0x9C1F);

        // cloud_sync
        tft.drawBitmap(157, 9, image_cloud_sync_bits, 17, 16, 0xF22B);

        // menu_settings_sliders
        tft.drawBitmap(11, 9, image_menu_settings_sliders_bits, 14, 16, 0xDED9);

        // Time
        tft.setTextColor(0xDED9);
        tft.setTextSize(4);
        tft.setFont(&FreeSansBold9pt7b);
        tft.setCursor(79, 62); // Set cursor position
        tft.print(hour);
        tft.setCursor(79, 124);
        tft.print(minute);

        // Layer 7
        tft.setTextSize(1);
        tft.setFont(&FreeMonoBold12pt7b);
        tft.setCursor(18, 250);
        tft.print("Now Playing");

        // Layer 7
        tft.setFont(&FreeMonoBold9pt7b);
        tft.setCursor(18, 274);
        tft.print("Bury you - Ari.");

        // music_play
        tft.drawBitmap(198, 263, image_music_play_bits, 15, 16, 0xDED9);

        // Layer 9
        tft.setFont();
        tft.setCursor(206, 8);
        tft.print("Caps");

        // Layer 10
        tft.setTextColor(capsLockStatus ? 0xFD40 : 0x7BEF); // ON or OFF
        tft.setCursor(206, 18);
        tft.print(capsLockStatus ? "ON" : "OFF");

        // Layer 11
        tft.setTextColor(0xFD40);
        tft.setCursor(90, 304);
        tft.print("3D Modeling");

        vTaskDelay(1 / portTICK_PERIOD_MS); // Delay for 10ms
    }
}
