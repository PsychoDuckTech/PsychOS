#include "tasks/serialHandler.h"
#include "utils/benchmark.h"
#include "tasks/commandProcessor.h"
#include <USBHIDKeyboard.h>
#include "tasks/wpmCounter.h"
#include "tusb.h"

uint8_t current_modifiers = 0; // Track currently pressed modifiers
uint8_t current_keycodes[6] = {0, 0, 0, 0, 0, 0}; // Track currently pressed keys

USBHIDKeyboard Keyboard;
QueueHandle_t hostMessageQueue;
USBHIDConsumerControl Consumer;

constexpr size_t SERIAL_BUFFER_SIZE = 128;
constexpr int HOST_MESSAGE_QUEUE_SIZE = 50;
constexpr unsigned long TASK_DELAY_MS = 10;
constexpr unsigned long VOLUME_COMMAND_DELAY_MS = 5;

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
    hostMessageQueue = xQueueCreate(HOST_MESSAGE_QUEUE_SIZE, sizeof(HostMessage)); // Increased to 50
    HostMessage receivedMessage;
    Keyboard.begin();
    Consumer.begin();
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
            {
                // Apply volume changes based on the magnitude of rotation
                // This takes advantage of encoder acceleration
                int steps = abs(receivedMessage.data);
                for (int i = 0; i < steps; i++)
                {
                    if (receivedMessage.data > 0)
                    {
                        Consumer.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
                        Consumer.release();
                    }
                    else
                    {
                        Consumer.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
                        Consumer.release();
                    }
                    // Small delay between volume commands to ensure they register correctly
                    delay(VOLUME_COMMAND_DELAY_MS);
                }
                break;
            }
            case VOLUME_MUTE:
                Consumer.press(CONSUMER_CONTROL_MUTE);
                Consumer.release();
                break;
            case KEY_PRESS:
                WPMCounter::recordKeyPress();
                // Check if this is a modifier key (0xE0-0xE7)
                if (receivedMessage.data >= 0xE0 && receivedMessage.data <= 0xE7)
                {
                    // Set the appropriate bit in the modifier byte
                    current_modifiers |= (1 << (receivedMessage.data - 0xE0));
                }
                else
                {
                    // Regular key - add to keycode array
                    // Find first empty slot
                    for (int i = 0; i < 6; i++)
                    {
                        if (current_keycodes[i] == 0)
                        {
                            current_keycodes[i] = receivedMessage.data;
                            break;
                        }
                    }
                }
                // Send updated HID report with current modifiers and keycodes
                tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, current_modifiers, current_keycodes);
                break;
            case KEY_RELEASE:
                // Check if this is a modifier key (0xE0-0xE7)
                if (receivedMessage.data >= 0xE0 && receivedMessage.data <= 0xE7)
                {
                    // Clear the appropriate bit in the modifier byte
                    current_modifiers &= ~(1 << (receivedMessage.data - 0xE0));
                }
                else
                {
                    // Regular key - remove from keycode array
                    for (int i = 0; i < 6; i++)
                    {
                        if (current_keycodes[i] == receivedMessage.data)
                        {
                            current_keycodes[i] = 0;
                            break;
                        }
                    }
                }
                // Send updated HID report with current modifiers and keycodes
                tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, current_modifiers, current_keycodes);
                break;
            }
        }

        // Yield to other tasks
        vTaskDelay(TASK_DELAY_MS / portTICK_PERIOD_MS); // 10ms delay for responsiveness
    }
}

void startHostCommTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t hostCommHandle;
    xTaskCreatePinnedToCore(
        hostCommunicationBridge, "Host Communication Bridge", stackDepth, NULL, priority, &hostCommHandle, core);
}