#include "hostCommunicationBridge.h"
#include "commandProcessor.h"

QueueHandle_t hostMessageQueue;
USBHIDConsumerControl ConsumerControl;

void hostCommunicationBridge(void *parameters)
{
    hostMessageQueue = xQueueCreate(10, sizeof(HostMessage));
    HostMessage receivedMessage;
    ConsumerControl.begin();

    Serial.println("Host Communication Bridge started.");

    for (;;)
    {
        if (xQueueReceive(hostMessageQueue, &receivedMessage, portMAX_DELAY) == pdTRUE)
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
                Serial.println(receivedMessage.data);
                break;
            case KEY_RELEASE:
                Serial.println(receivedMessage.data);
                break;
            }
        }
    }
}

void startHostCommTask(UBaseType_t core = 0, uint32_t stackDepth = 4096, UBaseType_t priority = 2)
{
    TaskHandle_t hostCommHandle;
    xTaskCreatePinnedToCore(
        hostCommunicationBridge,     // Function to be called
        "Host Communication Bridge", // Name of the task
        stackDepth,                  // Stack size in words
        NULL,                        // Task input parameter
        priority,                    // Priority of the task
        &hostCommHandle,             // Task handle
        core                         // Core where the task should run
    );
}