#include "tasks/mediaHandler.h"
#include "arduino_compat.h"

// Initialize the global variables
char currentSongTitle[MAX_MEDIA_TITLE_LENGTH] = "No media";
char defaultSongTitle[MAX_MEDIA_TITLE_LENGTH] = "No media";

// Function to update the current song title
void updateSongTitle(const char *newTitle)
{
    // Copy the new title to the current song title, ensuring it won't overflow
    strncpy(currentSongTitle, newTitle, MAX_MEDIA_TITLE_LENGTH - 1);
    currentSongTitle[MAX_MEDIA_TITLE_LENGTH - 1] = '\0'; // Ensure null-terminated

    // Signal that we need to refresh the main screen
    extern bool needsFullRedraw;
    needsFullRedraw = true;
}

// Initialize the media handler
void initMediaHandler()
{
    // Initialize with default title
    updateSongTitle(defaultSongTitle);
}
