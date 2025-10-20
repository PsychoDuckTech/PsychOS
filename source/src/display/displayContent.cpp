#include <Adafruit_ILI9341.h>
#include "arduino_compat.h"
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"

extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern bool capsLockStatus;
extern int hours;
extern int minutes;

// Define global variables that were previously in screens.h
bool needsFullRedraw = true;
int rgbValues[4] = {255, 255, 255, 100};
// Changed from static to global for access across files
unsigned long lastToggleTime = 0;
bool toggleDiscIcon = false;

void clearTime(void *parameters)
{
    tft.fillRect(75, 51, 90, 130, BG_COLOR);
}

void clearWPM(void *parameters)
{
    tft.fillRect(11, 297, 55, 16, BG_COLOR);
}

void clearLAYER(void *parameters)
{
    tft.fillRect(186, 297, 55, 16, BG_COLOR);
}