#include "tasks/serialHandler.h"
#include "utils/benchmark.h"

void serialHandler(void *parameters)
{
    Serial.println("Serial Handler started.");
    char input[64] = {0}; // Fixed-size buffer
    int index = 0;

    for (;;)
    {
        if (Serial.available() > 0)
        {
            char data = (char)Serial.read();
            if (data == '\n')
            {
                processCommand(input);
                index = 0;
                memset(input, 0, sizeof(input));
            }
            else if (index < 63)
            {
                input[index++] = data;
            }
            else
            {
                Serial.println("Input buffer overflow!");
                index = 0;
                memset(input, 0, sizeof(input));
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms delay for responsiveness
    }
}