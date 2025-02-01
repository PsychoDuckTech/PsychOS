#ifndef WPM_COUNTER_H
#define WPM_COUNTER_H

#include <Arduino.h>

namespace WPMCounter
{
    extern volatile int wpm;      // Global variable to hold the current words per minute value.
    extern volatile bool updated; // Flag to indicate a new WPM value is available.

    // Record a key press timestamp.
    void recordKeyPress();

    // Count keypresses in the past 60 seconds.
    int getKeyPressCount();

    // Calculate WPM (assuming 5 keystrokes per word).
    int computeWPM();

    // Task that updates the wpm variable every second.
    void wpmTask(void *parameters);

    // Start the WPM update task.
    void startWPMTask(UBaseType_t core = 1, uint32_t stackDepth = 4096, UBaseType_t priority = 1);
}

#endif // WPM_COUNTER_H