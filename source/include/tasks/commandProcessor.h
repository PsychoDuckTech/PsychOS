#pragma once

// Declare variables with extern (no initialization)
extern bool capsLockStatus;
extern bool connectionStatus;
extern bool moduleConnectionStatus;

// Command structure declaration
struct Command
{
    const char *name;
    void (*updateFunc)(int value);
    int (*getFunc)();
    int minValue;
    int maxValue;
};

// Function declarations
void processCommand(String data);