#pragma once
#include "globals.h"

enum ScreenType
{
    MainScreen,
    SettingsScreen,
    ModulesSubmenu,
    KeybindsSubmenu,
    IntegrationsSubmenu,
    ClockSubmenu,
    IotSubmenu,
    RGBLightingSubmenu,
    PixelFlushScreen,
};

struct MenuItem
{
    const char *name;
    void (*action)();
};

// Add structure for settings rotation events
struct SettingsRotationEvent
{
    int totalSteps; // Accumulated rotation steps
};

extern QueueHandle_t settingsRotationQueue;
extern ScreenType currentScreen;
extern int settingsSelectedOption;
extern bool inSettingsSubmenu;

void switchScreen(ScreenType newScreen);
void displaySettingsScreen(void *parameters);
void displayClockSubmenu(void *parameters);
void displayHandler(void *parameters);
void displayPixelFlushScreen(void *parameters);

void startDisplayTask(UBaseType_t core = 1, uint32_t stackDepth = 4096, UBaseType_t priority = 1);