#include "tasks/BluetoothSerialHandler.h"
#include "tasks/hostCommunicationBridge.h"
#include "main.h"
HardwareSerial BTSerial(2);

void bluetoothSerialHandler(void *parameters)
{
    Serial.println("Starting HC-06 configuration...");

    // Initialize Serial2 for HC-06 on pins RX=GPIO43, TX=GPIO44
    Serial2.begin(9600, SERIAL_8N1, 43, 44);
    delay(1000); // Give the module time to initialize

    // Test basic communication with "AT" command
    Serial.println("Sending AT...");
    Serial2.println("AT");
    delay(1000); // Wait for a response
    Serial.print("Response for AT: ");
    while (Serial2.available())
    {
        Serial.write(Serial2.read());
    }
    Serial.println(); // Newline for readability

    // Set the name
    Serial.println("Setting name to 'Kibodo One'...");
    Serial2.println("AT+NAMEKibodo One");
    delay(1000); // Wait for the module to process
    Serial.print("Response for name setting: ");
    while (Serial2.available())
    {
        Serial.write(Serial2.read());
    }
    Serial.println();

    // Set the PIN
    Serial.println("Setting PIN to '1987'...");
    Serial2.println("AT+PIN1987");
    delay(1000); // Wait for the module to process
    Serial.print("Response for PIN setting: ");
    while (Serial2.available())
    {
        Serial.write(Serial2.read());
    }
    Serial.println();

    Serial.println("Configuration complete.");
    Serial.println("Bluetooth Serial Handler started.");

    // Main loop for handling key presses
    for (;;)
    {
        if (Serial2.available() >= 2)
        { // Wait for at least 2 bytes
            uint8_t action = Serial2.read();
            uint8_t keycode = Serial2.read();

            HostMessage msg;
            if (action == 0)
            { // Key press
                msg.type = KEY_PRESS;
            }
            else if (action == 1)
            { // Key release
                msg.type = KEY_RELEASE;
            }
            else
            {
                continue; // Skip invalid action
            }
            msg.data = keycode;

            // Send to hostMessageQueue (non-blocking)
            xQueueSend(hostMessageQueue, &msg, 0);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms delay to avoid busy loop
    }
}

void startBluetoothSerialHandlerTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    xTaskCreatePinnedToCore(
        bluetoothSerialHandler,
        "BluetoothSerialHandler",
        stackDepth,
        NULL,
        priority,
        NULL,
        core);
}