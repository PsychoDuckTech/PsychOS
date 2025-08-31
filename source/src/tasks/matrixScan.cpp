#include "tasks/matrixScan.h"
#include "tasks/BLEHandler.h"
#include "utils/benchmark.h"

const auto keyMap = keyMapL0;
const auto keyName = keyNameL0;

constexpr bool BENCHMARK_ENABLED = false;  // Set to false to disable benchmarking entirely
constexpr unsigned long BENCHMARK_WINDOW_MS = 3000; // Benchmarking window in milliseconds
constexpr unsigned long DEBOUNCE_DELAY_MS = 10;
constexpr unsigned long ELECTRICAL_DELAY_US = 1;

void matrixScan(void *parameters)
{
    Serial.println(task_keyScanning_started);
    Serial.println("\n");

#if BENCHMARK_ENABLED
    unsigned long lastTime = millis();
    unsigned long pollCount[totalRows][totalCols] = {0};
    unsigned long totalPollCount[totalRows][totalCols] = {0};
#endif

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
            ets_delay_us(ELECTRICAL_DELAY_US); // Reduced delay for electrical stability

            bool reading = (colPinsMultiplexer.readChannel() == LOW);
            keyStates[row][col] = reading;
            lastReading[row][col] = reading;
            lastDebounceTime[row][col] = millis();
        }
        GPIO.out_w1ts = (1ULL << rowPins[row]); // Reset the row pin
    }

    for (;;)
    {
        unsigned long currentTime = millis();
        for (int row = 0; row < totalRows; row++)
        {
            GPIO.out_w1tc = (1ULL << rowPins[row]); // Activate current row pin

            for (int col = 0; col < totalCols; col++)
            {
                colPinsMultiplexer.fastSelect(col);
                ets_delay_us(ELECTRICAL_DELAY_US); // Reduced delay for electrical stability

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
                            if (BENCHMARK_ENABLED)
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
#if BENCHMARK_ENABLED
                pollCount[row][col]++;
                totalPollCount[row][col]++;
#endif
            }
            GPIO.out_w1ts = (1ULL << rowPins[row]); // Reset the row pin
        }

#if BENCHMARK_ENABLED
        if (currentTime - lastTime >= BENCHMARK_WINDOW_MS)
        {
            unsigned long pollRates[totalRows * totalCols];
            int index = 0;
            for (int row = 0; row < totalRows; row++)
            {
                for (int col = 0; col < totalCols; col++)
                {
                    pollRates[index++] = totalPollCount[row][col] / (BENCHMARK_WINDOW_MS / 1000.0);
                    totalPollCount[row][col] = 0; // Reset total poll count for the next window
                }
            }

            // Find the median without full sorting
            int mid = (totalRows * totalCols) / 2;
            std::nth_element(pollRates, pollRates + mid, pollRates + (totalRows * totalCols));
            unsigned long medianRate = pollRates[mid];

            Serial.print("Benchmark (polls per second per key): ");
            Serial.println(medianRate);

            lastTime = currentTime; // Reset the timer
        }
#endif
        vTaskDelay(1); // allow other tasks to run, 1ms delay
    }
}

void startMatrixScanTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    xTaskCreatePinnedToCore(
        matrixScan, "MatrixScan", stackDepth, NULL, priority, NULL, core);
}