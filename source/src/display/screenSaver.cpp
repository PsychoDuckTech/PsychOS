#include <Adafruit_ILI9341.h>
#include "arduino_compat.h"
#include "display/screens.h"
#include "globals.h"

extern Adafruit_ILI9341 tft;

void displayScreenSaver(void *parameters)
{
    static int x = 120;
    static int y = 160;
    static int dx = 2;
    static int dy = 2;

    // Draw a bouncing rectangle
    tft.fillRect(x, y, 20, 20, ILI9341_BLACK); // Clear old position

    x += dx;
    y += dy;

    // Bounce off edges
    if (x <= 0 || x >= 220)
        dx = -dx;
    if (y <= 0 || y >= 300)
        dy = -dy;

    tft.fillRect(x, y, 20, 20, ILI9341_BLUE); // Draw new position
    delay(50);
}
