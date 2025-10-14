#include <Arduino.h>
#include "tasks/clock.h"
#include "globals.h"
#include "utils/benchmark.h"
#include "tasks/mediaHandler.h"

bool capsLockStatus = false;
bool connectionStatus = false;

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

    // Extract command and value/query using C-style strings for better performance
    char command[64] = {0};
    int value = 0;
    bool isQuery = (queryIndex != -1);

    // Extract command and value/query
    if (isQuery)
    {
        strncpy(command, data.c_str(), queryIndex);
        command[queryIndex] = '\0';
    }
    else
    {
        strncpy(command, data.c_str(), spaceIndex);
        command[spaceIndex] = '\0';
        value = atoi(data.c_str() + spaceIndex + 1);
    }

    // Iterate through the command table to find a match
    for (int i = 0; i < commandCount; i++)
    {
        if (strcmp(command, commandTable[i].name) == 0)
        {
            if (isQuery)
            {
                // Query command
                Serial.print(command);
                Serial.print(" is: ");
                Serial.println(commandTable[i].getFunc());
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
                    Serial.print("Invalid value for ");
                    Serial.print(command);
                    Serial.print(". Must be between ");
                    Serial.print(commandTable[i].minValue);
                    Serial.print(" and ");
                    Serial.print(commandTable[i].maxValue);
                    Serial.println(".");
                }
            }
            return; // Command handled
        }
    }

    // Special handling for text-based commands
    if (strcmp(command, "songTitle") == 0)
    {
        if (isQuery)
        {
            Serial.print("Current song title: ");
            Serial.println(currentSongTitle);
        }
        else
        {
            // For songTitle, we need to grab the entire rest of the command after the first space
            const char* songTitleStr = data.c_str() + spaceIndex + 1;
            updateSongTitle(songTitleStr);
            Serial.print("Song title updated to: ");
            Serial.println(songTitleStr);
        }
        return; // Command handled
    }

    Serial.print("Unknown command: ");
    Serial.println(command);
    vTaskDelay(1);
}