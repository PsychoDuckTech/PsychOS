#include "utils/initializeBLE.h"

void initializeBLE()
{
    if (!BLE.begin())
    {
        Serial.println(BLE_initialization_failed);
        ESP.restart();
    }
    Serial.println(BLE_initialized_successfully);
}