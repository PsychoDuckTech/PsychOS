#pragma once

#include "arduino_compat.h"

// Maximum length for media information strings
#define MAX_MEDIA_TITLE_LENGTH 30

// Global variables for media information
extern char currentSongTitle[MAX_MEDIA_TITLE_LENGTH];
extern char defaultSongTitle[MAX_MEDIA_TITLE_LENGTH];

// Function to update the current song title
void updateSongTitle(const char *newTitle);

// Initialize the media handler
void initMediaHandler();
