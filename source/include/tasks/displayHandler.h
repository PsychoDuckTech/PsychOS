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
    ClockSubmenu,
    IotSubmenu,
    RGBLightingSubmenu,
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

void switchScreen(ScreenType newScreen);
void displaySettingsScreen(void *parameters);
void displayClockSubmenu(void *parameters);

void displayHandler(void *parameters);
void switchScreen(ScreenType newScreen);

#endif // SCREEN_MANAGER_H