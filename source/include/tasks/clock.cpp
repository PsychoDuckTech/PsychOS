#include "clock.h"

void clockTask(void *parameters)
{
    bool refreshFlags[3] = {false, false, false}; // [capsLockStatus, hours, minutes]
    Serial.println("Clock Task started.");

    for (;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 seconds
        seconds++;
        if (seconds >= 60)
        {
            seconds = 0;
            minutes++;
            updatedMinutes = true;
            if (minutes >= 60)
            {
                minutes = 0;
                hours++;
                if (hours >= 24)
                {
                    hours = 0;
                }
            }
        }
    }
}

void updateClock(int newHours, int newMinutes, int newSeconds)
{
    if (newHours >= 0 && newHours < 24)
    {
        hours = newHours;
    }
    if (newMinutes >= 0 && newMinutes < 60)
    {
        minutes = newMinutes;
    }
    if (newSeconds >= 0 && newSeconds < 60)
    {
        seconds = newSeconds;
    }
    updatedMinutes = true;
}