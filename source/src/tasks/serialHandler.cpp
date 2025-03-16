#include "tasks/serialHandler.h"
#include "utils/benchmark.h"

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

void startSerialTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t serialHandle;
    xTaskCreatePinnedToCore(
        serialHandler,    // Function to be called
        "Serial Handler", // Name of the task
        stackDepth,       // Stack size in words
        NULL,             // Task input parameter
        priority,         // Priority of the task
        &serialHandle,    // Task handle
        core              // Core where the task should run
    );
}