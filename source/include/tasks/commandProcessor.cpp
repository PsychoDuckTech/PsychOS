#include <Arduino.h>
#include "clock.h"

void processCommand(String data)
{
    int dotIndex = data.indexOf('.');   // Locate the dot in the command
    int spaceIndex = data.indexOf(' '); // Locate the space before the value

    if (dotIndex == -1 || spaceIndex == -1 || spaceIndex < dotIndex)
    {
        Serial.println("Invalid command format. Use: time.hours <value>");
        return;
    }

    // Extract the command part (e.g., "time.hours")
    String command = data.substring(0, spaceIndex);
    Serial.println("Command part: " + command); // Debug print

    // Extract the value part
    int value = data.substring(spaceIndex + 1).toInt();
    Serial.println("Value part: " + String(value)); // Debug print

    // Determine which variable to update
    if (command == "time.hours")
    {
        if (value >= 0 && value < 24)
        {
            updateClock(value, -1, -1);
            Serial.println("Hours updated to: " + String(hours));
        }
        else
        {
            Serial.println("Invalid hours value. Must be between 0 and 23.");
        }
    }
    else if (command == "time.minutes")
    {
        if (value >= 0 && value < 60)
        {
            updateClock(-1, value, -1);
            Serial.println("Minutes updated to: " + String(minutes));
        }
        else
        {
            Serial.println("Invalid minutes value. Must be between 0 and 59.");
        }
    }
    else if (command == "time.seconds")
    {
        if (value >= 0 && value < 60)
        {
            updateClock(-1, -1, value);
            Serial.println("Seconds updated to: " + String(seconds));
        }
        else
        {
            Serial.println("Invalid seconds value. Must be between 0 and 59.");
        }
    }
    else
    {
        Serial.println("Unknown command: " + command);
    }
}