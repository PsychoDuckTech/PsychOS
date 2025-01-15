#include <Arduino.h>
#include "config.h"
#include "drivers/rotaryEncoder/KY-040.h"
#include "hostCommunicationBridge.h"

#define CLK_PIN 2
#define DT_PIN 36
#define SW_PIN 0
#define POLLING_RATE_MS 1 // 1 = 1000Hz, 2 = 500Hz

KY040 knob(CLK_PIN, DT_PIN, SW_PIN);

void knobHandler(void *parameters)
{
    knob.begin();
    Serial.println(task_knobHandler_started);

    for (;;)
    {
        int rotation = knob.readEncoder();

        if (rotation != 0)
        {
            HostMessage msg;
            msg.type = VOLUME_CHANGE;
            msg.data = rotation;
            xQueueSend(hostMessageQueue, &msg, 0); // Use 0 timeout
        }

        if (knob.checkButtonPress())
        {
            HostMessage msg;
            msg.type = VOLUME_MUTE;
            msg.data = 0;
            xQueueSend(hostMessageQueue, &msg, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(POLLING_RATE_MS));
    }
}