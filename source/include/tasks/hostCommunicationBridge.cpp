#include "hostCommunicationBridge.h"
#include "commandProcessor.cpp"

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
            }
        }
    }
}