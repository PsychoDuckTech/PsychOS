#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "config.h"
#include "drivers/multiplexer/CD74HC4067.h"
#include "USB.h"

#define keyMap keyMapL0
#define keyName keyNameL0

Adafruit_USBD_HID usb_hid;

// State tracking for debounce
static bool keyStates[totalRows][totalCols] = {false};

void matrixScan(void *parameters)
{
    int usb_hid_poll_interval = 1; // Set HID poll interval to 1ms (1kHz)

    usb_hid.begin();
    usb_hid.setPollInterval(usb_hid_poll_interval);

    while (!TinyUSBDevice.mounted())
    {
        delay(1);
    }

    TinyUSB_Device_Task();

    Serial.println(task_keyScanning_started);
    Serial.println("\n");

    // unsigned long lastTime = millis();
    // unsigned long pollCount[totalRows][totalCols] = {0};

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

                // In an active-low design, LOW means the key is pressed
                bool isPressed = colPinsMultiplexer.readChannel() == LOW;

                if (isPressed != keyStates[row][col])
                {
                    keyStates[row][col] = isPressed;
                    switch (keyMap[row][col])
                    {
                    case 0:
                        Serial.printf("Empty key\n");
                        break;
                    default:
                        Serial.printf("Key: %s\n", keyName[row][col]);
                        Serial.printf("R: %d, C: %d\n\n", row, col);
                        break;
                    }

                    // If key state changed from 'not pressed' to 'pressed'
                    if (isPressed && !keyStates[row][col])
                    {
                        keyStates[row][col] = true;
                        usb_hid.keyboardPress(keyMap[row][col], 0);
                    }
                    // If key state changed from 'pressed' to 'not pressed'
                    else if (!isPressed && keyStates[row][col])
                    {
                        keyStates[row][col] = false;
                        usb_hid.keyboardRelease(keyMap[row][col]);
                    }
                }

                // if (isPressed) {
                //     switch (keyMap[row][col]) {
                //         case 0:
                //             Serial.printf("Empty key\n");
                //             break;
                //         default:
                //             Serial.printf("Key: %s\n", keyName[row][col]);
                //             Serial.printf("R: %d, C: %d\n\n", row, col);
                //             break;
                //     }
                // }
                // pollCount[row][col]++;
            }
            GPIO.out_w1ts = (1ULL << rowPins[row]); // Reset the row pin
        }

        // Print individual key polling rates
        // unsigned long currentTime = millis();
        // if (currentTime - lastTime >= 1000) {
        //    Serial.println("Individual key polling rates (polls/sec):");
        //    for (int row = 0; row < totalRows; row++) {
        //        for (int col = 0; col < totalCols; col++) {
        //            Serial.printf("Key [%d][%d]: %lu\n", row, col, pollCount[row][col]);
        //            pollCount[row][col] = 0; // Reset the count for the next second
        //        }
        //    }
        //    lastTime = currentTime;
        //}

        vTaskDelay(1);
        // delayMicroseconds(500); // 0.5ms delay
    }
}