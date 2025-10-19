#include "tasks/serialHandler.h"
#include "utils/benchmark.h"
#include "tasks/commandProcessor.h"
#include <USBHIDKeyboard.h>
#include "tasks/wpmCounter.h"

// HID report state: handled by USBHIDKeyboard library
USBHIDKeyboard Keyboard;
QueueHandle_t hostMessageQueue;
USBHIDConsumerControl Consumer;

int numModifiersHeld = 0;

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
                steps = min(steps, 5); // Cap at 5 steps to prevent jumping to max/min
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
            case KEY_PRESS: {
                if (receivedMessage.data >= 0xE0 && receivedMessage.data <= 0xE7) {
                    // Modifier keys - use HID press
                    Keyboard.press(receivedMessage.data);
                    numModifiersHeld++;
                } else if (receivedMessage.data >= 32 && receivedMessage.data <= 126) {
                    // Printable ASCII characters
                    if (numModifiersHeld > 0) {
                        Keyboard.write((char)receivedMessage.data);
                    } else {
                        Keyboard.press((char)receivedMessage.data);
                    }
                    WPMCounter::recordKeyPress();
                } else {
                    // Special keys (HID codes) - use press
                    Keyboard.press(receivedMessage.data);
                }
                break;
            }
            case KEY_RELEASE: {
                if (receivedMessage.data >= 0xE0 && receivedMessage.data <= 0xE7) {
                    // Modifier keys - use release
                    Keyboard.release(receivedMessage.data);
                    numModifiersHeld--;
                } else if (receivedMessage.data >= 32 && receivedMessage.data <= 126) {
                    // Printable ASCII
                    if (numModifiersHeld <= 0) {
                        Keyboard.release((char)receivedMessage.data);
                    }
                } else {
                    // Special keys - use release
                    Keyboard.release(receivedMessage.data);
                }
                break;
            }
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