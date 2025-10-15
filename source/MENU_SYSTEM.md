# Menu System Documentation

## Scrollable Settings Menu

The settings menu system has been redesigned to support any number of menu items with automatic scrolling. This makes it easy to add new settings options without worrying about screen space limitations.

### Key Components

1. **settingsSelectedOption**: Index of the currently selected menu item (0 to N-1)
2. **settingsScrollOffset**: Index of the topmost visible item on screen
3. **MAX_VISIBLE_ITEMS**: Maximum number of items that fit on screen at once (4 items)

### How Scrolling Works

When the user rotates the knob to navigate the menu:
- The selected option index changes based on rotation direction
- The scroll offset automatically adjusts to keep the selected item visible:
  - If scrolling up and the selected item would be above the visible area: scroll up
  - If scrolling down and the selected item would be below the visible area: scroll down
- Only visible items are rendered, improving performance

### Visual Example with 7 Items

**Initial State (item 0 selected):**
```
Scroll Offset: 0
Selected: 0
Visible Items: [0, 1, 2, 3]

Screen Display:
┌─────────────────┐
│ > Modules       │ ← Selected
│   Underglow     │
│   Clock         │
│   Pixel Flush   │
└─────────────────┘
```

**After scrolling down to item 4:**
```
Scroll Offset: 1
Selected: 4
Visible Items: [1, 2, 3, 4]

Screen Display:
┌─────────────────┐
│   Underglow     │
│   Clock         │
│   Pixel Flush   │
│ > IoT Link      │ ← Selected
└─────────────────┘
```

**After scrolling to the last item (item 6):**
```
Scroll Offset: 3
Selected: 6
Visible Items: [3, 4, 5, 6]

Screen Display:
┌─────────────────┐
│   Pixel Flush   │
│   IoT Link      │
│   Build         │
│ > Brightness    │ ← Selected
└─────────────────┘
```

## Adding New Menu Items

Adding new items to the settings menu is straightforward. Simply edit the `settingsMenuItems` array in `source/src/display/settingsScreen.cpp`:

```cpp
// Menu item structure for settings
struct SettingsMenuItem
{
    const char *text;        // Display text (use translation string)
    const uint8_t *icon;     // Optional icon (or nullptr)
    int iconWidth;           // Icon width in pixels
    int iconHeight;          // Icon height in pixels
};

// Define all settings menu items here - easy to add more!
const SettingsMenuItem settingsMenuItems[] = {
    {ui_modules, iconBleConnectedBig, 14, 22},
    {ui_underglow, iconLightBulb, 18, 23},
    {ui_clock, iconTranslation, 22, 22},
    {ui_pixel_flush, nullptr, 0, 0},
    {ui_your_new_item, iconYourIcon, 20, 20},  // <-- Add your item here!
};
```

### Steps to Add a Menu Item

1. **Define the translation string** in `source/include/translations.h` and `source/src/translations.cpp`:
   ```cpp
   // In translations.h
   extern const char *ui_your_new_item;
   
   // In translations.cpp
   const char *ui_your_new_item = "Your New Item";
   ```

2. **Add an icon (optional)** to `source/include/display/icons.h` if needed

3. **Add the menu item** to the `settingsMenuItems` array in `settingsScreen.cpp`

4. **No other changes needed!** The scrolling system automatically handles any number of items.

## Implementation Details

### Files Modified

- **source/src/display/settingsScreen.cpp**: Contains the menu items and rendering logic
- **source/src/tasks/displayHandler.cpp**: Handles scroll offset updates during navigation
- **source/include/tasks/displayHandler.h**: Exports the scroll offset variable
- **source/include/display/screens.h**: Exports the menu item count function

### Key Functions

- `getSettingsMenuItemCount()`: Returns the number of menu items
- `displaySettingsScreen()`: Renders only the visible menu items based on scroll offset

### Benefits

✅ **Scalable**: Add as many menu items as needed  
✅ **Efficient**: Only renders visible items  
✅ **Simple**: Just add items to an array  
✅ **Clean**: No hardcoded limits in the code  
✅ **User-friendly**: Smooth scrolling navigation
