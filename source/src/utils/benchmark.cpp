#include "utils/benchmark.h"

void printKeyPollingRates(int totalRows, int totalCols, unsigned long **pollCount, unsigned long &lastTime)
{
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= 1000)
    {
        Serial.println("Individual key polling rates (polls/sec):");
        for (int row = 0; row < totalRows; row++)
        {
            for (int col = 0; col < totalCols; col++)
            {
                Serial.printf("Key [%d][%d]: %lu\n", row, col, pollCount[row][col]);
                pollCount[row][col] = 0;
            }
        }
        lastTime = currentTime;
    }
}