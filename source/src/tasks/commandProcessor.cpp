#include <Arduino.h>
#include "tasks/clock.h"
#include "globals.h"
#include "utils/benchmark.h"

bool capsLockStatus = false;
bool connectionStatus = false;
bool moduleConnectionStatus = false;

// Define update and get functions for each variable
void updateHours(int value)
{
    updateClock(value, -1, -1);
    Serial.println("Hours updated to: " + String(hours));
}

int getHours()
{
    return hours;
}

void updateMinutes(int value)
{
    updateClock(-1, value, -1);
    Serial.println("Minutes updated to: " + String(minutes));
}

int getMinutes()
{
    return minutes;
}

void updateSeconds(int value)
{
    updateClock(-1, -1, value);
    Serial.println("Seconds updated to: " + String(seconds));
}

int getSeconds()
{
    return seconds;
}

void updateCaps(int value)
{
    capsLockStatus = (value != 0); // Set capsLockStatus to true if value is non-zero
    Serial.println("Caps lock status updated to: " + String(capsLockStatus));
}

int getCaps()
{
    return capsLockStatus;
}

void updateConnectionStatus(int value)
{
    connectionStatus = (value != 0); // Set capsLockStatus to true if value is non-zero
    Serial.println("Connection status updated to: " + String(connectionStatus));
}

int getConnectionStatus()
{
    return connectionStatus;
}

// Create a command table
Command commandTable[] = {
    {"time.hours", updateHours, getHours, 0, 23},
    {"time.minutes", updateMinutes, getMinutes, 0, 59},
    {"time.seconds", updateSeconds, getSeconds, 0, 59},
    {"caps", updateCaps, getCaps, 0, 1}, // Use 0 and 1 for boolean values
    {"connectionStatus", updateConnectionStatus, getConnectionStatus, 0, 1},
};

const int commandCount = sizeof(commandTable) / sizeof(commandTable[0]); // Count of commands

void processCommand(String data)
{
    data.trim(); // Remove leading/trailing spaces

    int spaceIndex = data.indexOf(' '); // Locate the space before the value
    int queryIndex = data.indexOf('?'); // Locate the question mark

    // Check if the command is valid
    if (spaceIndex == -1 && queryIndex == -1)
    {
        Serial.println("Invalid command format. Use: <command> <value> or <command>?");
        return;
    }

    String command;
    int value = 0;
    bool isQuery = (queryIndex != -1);

    // Extract command and value/query
    if (isQuery)
    {
        command = data.substring(0, queryIndex);
    }
    else
    {
        command = data.substring(0, spaceIndex);
        value = data.substring(spaceIndex + 1).toInt();
    }

    // Iterate through the command table to find a match
    for (int i = 0; i < commandCount; i++)
    {
        if (command == commandTable[i].name)
        {
            if (isQuery)
            {
                // Query command
                Serial.println(command + " is: " + String(commandTable[i].getFunc()));
            }
            else
            {
                // Validate and update the value
                if (value >= commandTable[i].minValue && value <= commandTable[i].maxValue)
                {
                    commandTable[i].updateFunc(value); // Call the associated function
                }
                else
                {
                    Serial.println("Invalid value for " + command + ". Must be between " +
                                   commandTable[i].minValue + " and " + commandTable[i].maxValue + ".");
                }
            }
            return; // Command handled
        }
    }

    Serial.println("Unknown command: " + command);
    vTaskDelay(1);
}