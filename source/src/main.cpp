#include <Arduino.h>
//#include <stdio.h>

int count2 = 0;
int count1 = 0;

/*#define ROWS 6
#define COLS 16

const uint8_t key_map[ROWS][COLS] = {
    {0x29, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x46, 0x49, 0x00, 0x00}, //row 1
    {}, //row 2
    {}, //row 3
    {}, //row 4
    {}, //row 5
    {} //row 6
};

void KeystrokeHandler(void *parameters) {
    for (;;) {
        // Read key press
        char key = getchar();
        printf("Key pressed: %c\n", key);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}*/

void Task2(void *parameters) {
    for (;;) {
        Serial.print("Task2: ");
        Serial.println(count2++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void TaskSpam(void *parameters) {
    for (;;) {
        Serial.print("TaskSpam: ");
        Serial.println(count1++);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);

    /*xTaskCreate(
        KeystrokeHandler,
        "Keystroke Handler",
        1000,
        NULL,
        2,
        NULL
    );*/
    xTaskCreate(
        Task2,
        "T2",
        1000,
        NULL,
        1,
        NULL
    );
    xTaskCreate(
        TaskSpam,
        "TS",
        1000,
        NULL,
        1,
        NULL
    );
}

void loop() {
    // Do nothing
}