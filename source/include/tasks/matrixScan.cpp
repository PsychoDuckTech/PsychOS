#include "matrixScan.h"
#include "BLEHandler.h"

extern BLECharacteristic psychoCharacteristic;

#define keyMap keyMapL0
#define keyName keyNameL0

#define benchmark false
#define DEBOUNCE_DELAY_MS 3

void matrixScan(void *parameters)
{
    Serial.println(task_keyScanning_started);
    Serial.println("\n");

    unsigned long lastTime = millis();
    unsigned long pollCount[totalRows][totalCols] = {0};

    // Array to store the confirmed (debounced) state of each key.
    bool keyStates[totalRows][totalCols] = {0};
    // Array to store the last instantaneous reading.
    bool lastReading[totalRows][totalCols] = {0};
    // Array to store the last time a state change was detected.
    unsigned long lastDebounceTime[totalRows][totalCols] = {0};

    for (;;)
    {
        for (int row = 0; row < totalRows; row++)
        {
            GPIO.out_w1tc = (1ULL << rowPins[row]); // Activate current row pin

            // Scan the column combination with the current row
            for (int col = 0; col < totalCols; col++)
            {
                colPinsMultiplexer.fastSelect(col);
                ets_delay_us(4); // Small delay for electrical stability

                bool reading = (colPinsMultiplexer.readChannel() == LOW);

                // If the reading has changed from the last reading, reset the debounce timer.
                if (reading != lastReading[row][col])
                {
                    lastDebounceTime[row][col] = millis();
                }
                // If the reading is stable for longer than the debounce delay,
                // and it's different from the confirmed state, update the state.
                if ((millis() - lastDebounceTime[row][col]) > DEBOUNCE_DELAY_MS)
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
#ifdef BLE_MASTER
                            Serial.printf("Key %s %s\n", keyName[row][col], (reading ? "pressed" : "released"));
#elif defined(BLE_SLAVE)
                            if (moduleConnectionStatus)
                            {
                                uint8_t data[2] = {keyMap[row][col], (reading ? 1 : 0)};
                                psychoCharacteristic.writeValue(data, 2);
                            }
#endif
                            break;
                        }
                    }
                }
                // Save the current reading for the next iteration.
                lastReading[row][col] = reading;
                pollCount[row][col]++;
            }
            GPIO.out_w1ts = (1ULL << rowPins[row]); // Reset the row pin
        }

        if (benchmark)
        {
            unsigned long currentTime = millis();
            if (currentTime - lastTime >= 1000)
            {
                Serial.println("Individual key polling rates (polls/sec):");
                for (int row = 0; row < totalRows; row++)
                {
                    for (int col = 0; col < totalCols; col++)
                    {
                        Serial.printf("Key [%d][%d]: %lu\n", row, col, pollCount[row][col]);
                        pollCount[row][col] = 0;
                    }
                }
                lastTime = currentTime;
            }
        }
        vTaskDelay(1); // allow other tasks to run, 1ms delay
    }
}

void startMatrixScanTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    xTaskCreatePinnedToCore(
        matrixScan, "MatrixScan", stackDepth, NULL, priority, NULL, core);
}