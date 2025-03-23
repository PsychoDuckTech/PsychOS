#include "tasks/matrixScan.h"
#include "tasks/BLEHandler.h"
#include "utils/benchmark.h"

#define keyMap keyMapL0
#define keyName keyNameL0

#define benchmark true
#define DEBOUNCE_DELAY_MS 10

void matrixScan(void *parameters)
{
    Serial.println(task_keyScanning_started);
    Serial.println("\n");

    unsigned long lastTime = millis();
    unsigned long pollCount[totalRows][totalCols] = {0};

    // Arrays to store key states and debounce information.
    bool keyStates[totalRows][totalCols];
    bool lastReading[totalRows][totalCols];
    unsigned long lastDebounceTime[totalRows][totalCols];

    // Initialize all key states with a single scan.
    for (int row = 0; row < totalRows; row++)
    {
        GPIO.out_w1tc = (1ULL << rowPins[row]);   // Activate current row pin
        for (int col = 0; col < totalCols; col++) // Scan the column combination with the current row
        {
            colPinsMultiplexer.fastSelect(col);
            ets_delay_us(2); // Reduced delay for electrical stability

            bool reading = (colPinsMultiplexer.readChannel() == LOW);
            keyStates[row][col] = reading;
            lastReading[row][col] = reading;
            lastDebounceTime[row][col] = millis();
        }
        GPIO.out_w1ts = (1ULL << rowPins[row]); // Reset the row pin
    }

    for (;;)
    {
        unsigned long currentTime = millis(); // Cache the current time
        for (int row = 0; row < totalRows; row++)
        {
            GPIO.out_w1tc = (1ULL << rowPins[row]); // Activate current row pin

            for (int col = 0; col < totalCols; col++)
            {
                colPinsMultiplexer.fastSelect(col);
                ets_delay_us(2); // Reduced delay for electrical stability

                bool reading = (colPinsMultiplexer.readChannel() == LOW);

                // If the reading has changed from the last reading, reset the debounce timer.
                if (reading != lastReading[row][col])
                {
                    lastDebounceTime[row][col] = currentTime;
                }

                // If the reading is stable for longer than the debounce delay, update the confirmed state.
                if ((currentTime - lastDebounceTime[row][col]) > DEBOUNCE_DELAY_MS)
                {
                    if (reading != keyStates[row][col])
                    {
                        keyStates[row][col] = reading;

                        switch (keyMap[row][col])
                        {
                        case 0:
                            Serial.printf("Empty key %s\n", (reading ? "pressed" : "released"));
                            break;
                        default:
                            if (benchmark)
                            {
                                Serial.printf("Key %s %s\n", keyName[row][col], (reading ? "pressed" : "released"));
                            }
                            if (reading)
                            {
                                HostMessage msg;
                                msg.type = KEY_PRESS;
                                msg.data = keyMap[row][col];
                                xQueueSend(hostMessageQueue, &msg, 0);
                            }
                            else
                            {
                                HostMessage msg;
                                msg.type = KEY_RELEASE;
                                msg.data = keyMap[row][col];
                                xQueueSend(hostMessageQueue, &msg, 0);
                            }
                            break;
                        }
                    }
                }

                lastReading[row][col] = reading;
                pollCount[row][col]++;
            }
            GPIO.out_w1ts = (1ULL << rowPins[row]); // Reset the row pin
        }

        if (benchmark)
        {
            unsigned long **pollCountPtr = new unsigned long *[totalRows];
            for (int i = 0; i < totalRows; i++)
            {
                pollCountPtr[i] = pollCount[i];
            }
            printKeyPollingRates(totalRows, totalCols, pollCountPtr, lastTime);
            delete[] pollCountPtr;
        }
        vTaskDelay(1); // allow other tasks to run, 1ms delay
    }
}

void startMatrixScanTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    xTaskCreatePinnedToCore(
        matrixScan, "MatrixScan", stackDepth, NULL, priority, NULL, core);
}