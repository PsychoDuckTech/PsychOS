#include <Arduino.h>
#include "config.h"
#include "drivers/multiplexer/CD74HC4067.h"

#define keyMap keyMapL0
#define keyName keyNameL0

void matrixScan(void *parameters) {
    Serial.println(task_keyScanning_started);
    Serial.println("\n");

    unsigned long lastTime = millis();
    unsigned long pollCount[totalRows][totalCols] = {0};

    for (;;) {
        for (int row = 0; row < totalRows; row++) {
            GPIO.out_w1tc = (1ULL << rowPins[row]);
            
            // Scan the column combination with the current row
            for (int col = 0; col < totalCols; col++) {
                colPinsMultiplexer.fastSelect(col);
                ets_delay_us(3); // Small delay for electrical stability

                if (colPinsMultiplexer.readChannel() == LOW) {  // Activate on low
                    switch (keyMap[row][col]) {
                        case 0:
                            Serial.printf("Empty key\n");
                            break;
                        default:
                            Serial.printf("Key: %s\n", keyName[row][col]);
                            Serial.printf("R: %d, C: %d\n\n", row, col);
                            break;
                    }
                }
                pollCount[row][col]++;
            }
            GPIO.out_w1ts = (1ULL << rowPins[row]);
        }

        unsigned long currentTime = millis();
        if (currentTime - lastTime >= 1000) {
            Serial.println("Individual key polling rates (polls/sec):");
            for (int row = 0; row < totalRows; row++) {
                for (int col = 0; col < totalCols; col++) {
                    Serial.printf("Key [%d][%d]: %lu\n", row, col, pollCount[row][col]);
                    pollCount[row][col] = 0; // Reset the count for the next second
                }
            }
            lastTime = currentTime;
        }

        vTaskDelay(1);
        //delayMicroseconds(500); // 0.5ms delay
    }
}