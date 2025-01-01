#include <Arduino.h>
#include "config.h"
#include "drivers/multiplexer/CD74HC4067.h"
#include "hostCommunicationBridge.h" // Include the header for the message queue
#include "functions/customHID.h"

#define keyMap keyMapL0
#define keyName keyNameL0

extern QueueHandle_t hostMessageQueue; // Declare the external message queue

void matrixScan(void *parameters) {
    Serial.println(task_keyScanning_started);
    Serial.println("\n");
    matrixReport report = {0};

    unsigned long lastTime = millis();
    unsigned long pollCount[totalRows][totalCols] = {0};

    for (;;) {
        for (int row = 0; row < totalRows; row++) {
            GPIO.out_w1tc = (1ULL << rowPins[row]);
            
            // Scan the column combination with the current row
            for (int col = 0; col < totalCols; col++) {
                colPinsMultiplexer.fastSelect(col);
                ets_delay_us(5); // Small delay for electrical stability

                if (colPinsMultiplexer.readChannel() == LOW) {  // Activate on low
                    uint8_t keycode = keyMap[row][col];
                    
                    // Check if modifier using correct masks from usb_hid_keys.h
                    if (keycode >= KEY_LEFTCTRL && keycode <= KEY_RIGHTMETA) {
                        switch(keycode) {
                            case KEY_LEFTCTRL:  report.modifiers |= MOD_LCTRL;  break;
                            case KEY_LEFTSHIFT: report.modifiers |= MOD_LSHIFT; break;
                            case KEY_LEFTALT:   report.modifiers |= MOD_LALT;   break;
                            case KEY_LEFTMETA:  report.modifiers |= MOD_LGUI;   break;
                            case KEY_RIGHTCTRL: report.modifiers |= MOD_RCTRL;  break;
                            case KEY_RIGHTSHIFT:report.modifiers |= MOD_RSHIFT; break;
                            case KEY_RIGHTALT:  report.modifiers |= MOD_RALT;   break;
                            case KEY_RIGHTMETA: report.modifiers |= MOD_RGUI;   break;
                        }
                    }
                    // Regular key
                    else {
                        for (int i = 0; i < 6; i++) {
                            if (report.keys[i] == 0) {
                                report.keys[i] = keycode;
                                break;
                            }
                        }
                    }
                    
                    Keyboard.sendReport(&report);
                }
                pollCount[row][col]++;
            }
            GPIO.out_w1ts = (1ULL << rowPins[row]); // Directly set the row pin high instead of using digitalWrite
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

        // Clear report for next scan
        memset(&report, 0, sizeof(report));
        vTaskDelay(1);
        //delayMicroseconds(500); // 0.5ms delay
    }
}