#include "tasks/clock.h"

int hours = 0;
int minutes = 0;
int seconds = 0; // Add this line
bool updatedMinutes = false;

void saveTime()
{
    nvs.saveInt("hours", hours);
    nvs.saveInt("minutes", minutes);
    nvs.saveInt("seconds", seconds);
}

void loadTime()
{
    hours = nvs.loadInt("hours", 0);
    minutes = nvs.loadInt("minutes", 0);
    seconds = nvs.loadInt("seconds", 0);
    Serial.printf("Loaded time on boot: %02d:%02d:%02d\n", hours, minutes, seconds);
}

void clockTask(void *parameters)
{
    bool refreshFlags[3] = {false, false, false}; // [capsLockStatus, hours, minutes]
    Serial.println(task_clock_started);

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
            saveTime();
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

void startClockTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    loadTime();
    TaskHandle_t clock;
    xTaskCreatePinnedToCore(
        clockTask,
        "Clock",
        stackDepth,
        NULL,
        priority,
        &clock,
        core);
}
