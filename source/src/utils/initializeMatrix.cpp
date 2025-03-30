#include "main.h"
#include "utils/initializeMatrix.h"

void initializeMatrix()
{
    setupRowPins();
    setupColPins(); // Present in the board specific configuration
    Serial.println(matrix_initialized_successfully);
}

void setupRowPins()
{
    for (int i = 0; i < totalRows; i++)
    {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH); // Set HIGH (inactive) initially
    }
}

void setupColPins()
{
#if USES_MULTIPLEXER
    // No column pins to setup here when using multiplexer
#else
    // This code path is not currently used as USES_MULTIPLEXER is defined as true
    // If you need to use direct column pins, define the colPins array in main.cpp
    // and uncomment the code below:
    /*
    extern const int colPins[];
    for (int i = 0; i < totalCols; i++)
    {
        pinMode(colPins[i], INPUT);
    }
    */
    Serial.println("Warning: Non-multiplexer mode is not currently implemented");
#endif
}