#include "tasks/serialHandler.h"
#include "utils/benchmark.h"
#include "tasks/commandProcessor.h"
#include <USBHIDKeyboard.h>
#include "tasks/wpmCounter.h"
#include "tusb.h"

uint8_t keycode[6] = {0, 0, 0, 0, 0, 0}; // Initialize as empty

USBHIDKeyboard Keyboard;
QueueHandle_t hostMessageQueue;
USBHIDConsumerControl ConsumerControl;

#define SERIAL_BUFFER_SIZE 128

void handleSerialCommands(char *serialBuffer, size_t &bufferIndex)
{
    while (Serial.available() > 0)
    {
        char data = (char)Serial.read();
        if (data == '\n')
        {
            serialBuffer[bufferIndex] = '\0'; // Null-terminate the string
            processCommand(serialBuffer);
            bufferIndex = 0; // Reset buffer index
        }
        else
        {
            if (bufferIndex < SERIAL_BUFFER_SIZE - 1)
            {
                serialBuffer[bufferIndex++] = data;
            }
            else
            {
                Serial.println("Input buffer overflow! Command ignored.");
                bufferIndex = 0; // Reset buffer index on overflow
            }
        }
    }
}

void hostCommunicationBridge(void *parameters)
{
    hostMessageQueue = xQueueCreate(50, sizeof(HostMessage)); // Increased to 50
    HostMessage receivedMessage;
    Keyboard.begin();
    ConsumerControl.begin();
    Serial.println("Host Communication Bridge started.");

    char serialBuffer[SERIAL_BUFFER_SIZE] = {0};
    size_t bufferIndex = 0;

    for (;;)
    {
        // Handle serial input
        handleSerialCommands(serialBuffer, bufferIndex);

        // Handle host communication
        if (xQueueReceive(hostMessageQueue, &receivedMessage, 0) == pdTRUE)
        {
            switch (receivedMessage.type)
            {
            case VOLUME_CHANGE:
                if (receivedMessage.data > 0)
                {
                    ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
                    ConsumerControl.release();
                }
                else
                {
                    ConsumerControl.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
                    ConsumerControl.release();
                }
                break;
            case VOLUME_MUTE:
                ConsumerControl.press(CONSUMER_CONTROL_MUTE);
                ConsumerControl.release();
                break;
            case KEY_PRESS:
                WPMCounter::recordKeyPress();
                // Use tud_hid_keyboard_report directly
                memset(keycode, 0, sizeof(keycode));
                keycode[0] = receivedMessage.data;
                tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, keycode);
                break;
            case KEY_RELEASE:
                // Send empty report to release all keys
                memset(keycode, 0, sizeof(keycode));
                tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, keycode);
                break;
            }
        }

        // Yield to other tasks
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms delay for responsiveness
    }
}

void startHostCommTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t hostCommHandle;
    xTaskCreatePinnedToCore(
        hostCommunicationBridge, "Host Communication Bridge", stackDepth, NULL, priority, &hostCommHandle, core);
}