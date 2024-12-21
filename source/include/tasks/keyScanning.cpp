#include <Arduino.h>
#include "config.h"
#include "drivers/multiplexer/CD74HC4067.h"

#define keyMap keyMapL0
#define keyName keyNameL0

void keyScanning(void *parameters) {
    USB_HID_Keyboard_Report_t hid_report;
    Serial.println(task_keystrokeHandler_started);
    Serial.println("\n");

    for (;;) {
        for (int row = 0; row < totalRows; row++) {
            digitalWrite(rowPins[row], HIGH);
            
            // Scan the column combination with the current row
            for (int col = 0; col < totalCols; col++) {
                colPinsMultiplexer.selectChannel(col);
                if (!colPinsMultiplexer.readChannel()) {  // Activate on low
                    switch (keyMap[row][col]) {
                        case 0:
                            Serial.printf("Empty key\n");
                            break;
                        default:
                            Serial.printf("Key: %s\n", keyName[row][col]);
                            Serial.printf("R: %d, C: %d\n\n", row, col);
                            delay(1000);
                            break;
                    }
                }
            }
            digitalWrite(rowPins[row], LOW);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS); // 1ms delay corresponding to a polling rate of 1000Hz
    }
}