// screen_manager.h
#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "globals.h"

enum ScreenType
{
    MainScreen,
    SettingsScreen,
    ModulesSubmenu,
    KeybindsSubmenu,
    IntegrationsSubmenu,
    RGBSubmenu,
    ModuleSpecificSubmenu,
    ClockSubmenu, // Add this line
};

struct MenuItem
{
    const char *name;
    void (*action)();
};

void switchScreen(ScreenType newScreen);

extern ScreenType currentScreen;
extern int settingsSelectedOption;
extern bool inSettingsSubmenu;
extern int moduleCount; // Add this line

void switchScreen(ScreenType newScreen);
void displaySettingsScreen(void *parameters);
void displayModulesSubmenu(void *parameters);
void displayClockSubmenu(void *parameters);

void displayHandler(void *parameters);
void switchScreen(ScreenType newScreen);

void handleRotation(int rotation);
void handleShortPress();
void handleLongPress();
void handleDoublePress();

#endif // SCREEN_MANAGER_H