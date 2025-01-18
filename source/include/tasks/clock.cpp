#include "clock.h"

int hours = 0;
int minutes = 0;
int seconds = 0;
int updatedMinutes = true;

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