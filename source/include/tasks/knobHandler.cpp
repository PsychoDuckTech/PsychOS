#include <Arduino.h>
#include "config.h"
#include "drivers/rotaryEncoder/KY-040.h"

#define CLK_PIN 2
#define DT_PIN 1
#define SW_PIN 4

KY040 knob(CLK_PIN, DT_PIN, SW_PIN);

void knobHandler(void *parameters) {
    knob.begin();
    Serial.println(task_knobHandler_started);

    for (;;) {
        int rotation = knob.readEncoder();
        if (rotation != 0) {
            Serial.printf("Knob rotated: %s\n", rotation > 0 ? "Clockwise" : "Counter-clockwise");
        }

        if (knob.isButtonPressed()) {
            Serial.println("Knob button pressed");
            vTaskDelay(250 / portTICK_PERIOD_MS); // Debounce delay, 250ms
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms delay
    }
}