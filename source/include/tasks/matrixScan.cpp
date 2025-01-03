#include <Arduino.h>
#include "config.h"
#include "drivers/multiplexer/CD74HC4067.h"
#include "USB.h"
#include "tusb.h"
#include "usbDescriptors.c"

USBHIDKeyboard keyboard;

#define keyMap keyMapL0
#define keyName keyNameL0

void send_hid_key(uint8_t keycode) {
    uint8_t key_report[6] = {0};

    key_report[0] = keycode;

    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, key_report);
}


void matrixScan(void *parameters) {
    Serial.println(task_keyScanning_started);
    Serial.println("\n");

    unsigned long lastTime = millis();
    unsigned long pollCount[totalRows][totalCols] = {0};

    tusb_init();

    for (;;) {
        for (int row = 0; row < totalRows; row++) {
            GPIO.out_w1tc = (1ULL << rowPins[row]);
            
            // Scan the column combination with the current row
            for (int col = 0; col < totalCols; col++) {
                colPinsMultiplexer.fastSelect(col);
                ets_delay_us(3); // small delay for electrical stability

                if (colPinsMultiplexer.readChannel() == LOW) {  // Activate on low
                    switch (keyMap[row][col]) {
                        case 0:
                            Serial.printf("Empty key\n");
                            break;
                        default:
                            send_hid_key(keyMap[row][col]);
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

        vTaskDelay(2);
        //delayMicroseconds(500); // 0.5ms delay
    }
}