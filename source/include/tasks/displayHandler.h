// screen_manager.h
#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

enum ScreenType
{
    MainScreen,
    SettingsScreen,
    ModulesSubmenu,
    KeybindsSubmenu,
    IntegrationsSubmenu,
    RGBSubmenu,
};

struct MenuItem
{
    const char *name;
    void (*action)();
};

void switchScreen(ScreenType newScreen);
void displayMainScreen(void *parameters);
void displaySettingsScreen(void *parameters);

extern ScreenType currentScreen;
extern int settingsSelectedOption;
extern bool inSettingsSubmenu;

void switchScreen(ScreenType newScreen);
void displaySettingsScreen(void *parameters);

#endif // SCREEN_MANAGER_H