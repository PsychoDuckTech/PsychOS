#include <Arduino.h>
#include <ArduinoBLE.h>
#include "config.h"
#include "tasks/matrixScan.cpp"
#include "tasks/moduleConnectionHandler.cpp"
#include "tasks/knobHandler.cpp"
#include "tasks/displayHandler.cpp"
#include "tasks/clock.h"
#include "tasks/clock.cpp"
#include "utils/initializeMatrix.h"
#include "utils/initializeBLE.h"

#include "soc/io_mux_reg.h" // Provides access to the IO_MUX registers
#include "driver/gpio.h"    // Standard GPIO functions

void disableJTAG()
{
    // Disable JTAG by reconfiguring pins to GPIO functionality
    REG_WRITE(IO_MUX_GPIO39_REG, 0x80); // Set GPIO 39 to GPIO mode
    REG_WRITE(IO_MUX_GPIO40_REG, 0x80); // Set GPIO 40 to GPIO mode
    REG_WRITE(IO_MUX_GPIO41_REG, 0x80); // Set GPIO 41 to GPIO mode
    REG_WRITE(IO_MUX_GPIO42_REG, 0x80); // Set GPIO 42 to GPIO mode
}

void setup()
{
    Serial.begin(115200);
    delay(2000); // Delay for the serial monitor to start
    Serial.println(String(OS_version) + ", " + byCompany);
    Serial.println("Mr. 95YCH0DUCK\n");

    // Disable JTAG to free GPIO 39–42 for general use THIS IS MERELY FOR LATER REFERENCE

    // Initialize watchdog
    esp_task_wdt_init(10, true);

    initializeBLE();
    initializeMatrix();

    TaskHandle_t keyTaskHandle;
    xTaskCreatePinnedToCore(
        matrixScan,          // Task function: The function that will execute as the task.
        "Keystroke Handler", // Name of the task: A human-readable name for debugging.
        8192,                // Stack size: Amount of stack memory allocated to the task (in bytes).
        NULL,                // Task parameters: A pointer to pass to the task function (optional).
        3,                   // Priority: The priority of the task (higher values = higher priority).
        &keyTaskHandle,      // Task handle: A pointer to store the task's handle (optional).
        0                    // Core ID: The CPU core (0 or 1) on which the task will run.
    );

    TaskHandle_t knobTaskHandle;
    xTaskCreatePinnedToCore(knobHandler, "Knob Handler", 2048, NULL, 1, &knobTaskHandle, 1);

    TaskHandle_t hostCommHandle;
    xTaskCreatePinnedToCore(hostCommunicationBridge, "Host Communication Bridge", 4096, NULL, 2, &hostCommHandle, 0);

    TaskHandle_t clock;
    xTaskCreatePinnedToCore(clockTask, "Clock", 1024, NULL, 1, &clock, 1);

    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(moduleConnectionHandler, "BLE Handler", 16384, NULL, 1, &bleTaskHandle, 1);

    TaskHandle_t displayHandle;
    xTaskCreatePinnedToCore(displayHandler, "Display Handler", 4096, NULL, 1, &displayHandle, 1);

    processCommand("time.hours 12");
}

void loop()
{
    // not required due to the use of FreeRTOS tasks
}