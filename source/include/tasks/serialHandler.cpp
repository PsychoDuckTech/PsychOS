#include <Arduino.h>
#include "tasks/clock.h"

void serialHandler(void *parameters)
{
    Serial.println("Serial Handler started.");
    String input = ""; // Buffer to store the input string

    for (;;)
    {
        if (Serial.available() > 0)
        {
            char data = (char)Serial.read();

            // Check for newline character (indicating the end of a message)
            if (data == '\n')
            {
                processCommand(input);
                input = ""; // Clear the buffer
            }
            else
            {
                input += data; // Add character to the string
            }
        }
    }
}
