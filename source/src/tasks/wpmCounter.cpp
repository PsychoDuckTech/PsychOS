#include <Arduino.h>

namespace WPMCounter
{
    const int MAX_EVENTS = 1000;      // Maximum stored key events
    unsigned long events[MAX_EVENTS]; // Circular buffer for key press timestamps
    int head = 0;                     // Index of the oldest event
    int count = 0;                    // Number of events in the buffer

    // Global variables to hold the current WPM and updated flag.
    volatile int wpm = 0;
    volatile bool updated = false;

    // Record a key press timestamp.
    void recordKeyPress()
    {
        unsigned long now = millis();
        if (count < MAX_EVENTS)
        {
            int tail = (head + count) % MAX_EVENTS;
            events[tail] = now;
            count++;
        }
        else
        {
            // Buffer full: overwrite the oldest event.
            events[head] = now;
            head = (head + 1) % MAX_EVENTS;
        }
    }

    // Count keypresses in the past 10 seconds.
    int getKeyPressCount()
    {
        unsigned long now = millis();
        int cnt = 0;
        for (int i = 0; i < count; i++)
        {
            int index = (head + i) % MAX_EVENTS;
            if (now - events[index] <= 10000UL)
            {
                cnt++;
            }
        }
        return cnt;
    }

    // Calculate WPM based on a 10-second window (5 keystrokes = 1 word).
    int computeWPM()
    {
        int count10s = getKeyPressCount();
        // Words in 10 sec = count10s / 5, scale to per minute by multiplying by 6.
        return (count10s * 6) / 5;
    }

    // Task that updates the wpm variable and the updated flag.
    void wpmTask(void *parameters)
    {
        int lastWPM = 0;
        for (;;)
        {
            int newWPM = computeWPM();
            if (newWPM != lastWPM)
            {
                lastWPM = newWPM;
                wpm = newWPM;
                updated = true;
            }
            else
            {
                updated = false;
            }
            // Update more frequently for a quicker response.
            vTaskDelay(750 / portTICK_PERIOD_MS);
        }
    }

    // Start the WPM update task.
    void startWPMTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
    {
        xTaskCreatePinnedToCore(wpmTask, "WPMTask", stackDepth, NULL, priority, NULL, core);
    }
}
