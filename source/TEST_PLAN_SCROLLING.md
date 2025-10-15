# Test Plan for Scrollable Settings Menu

## Test Scenarios

### Test 1: Menu with 4 Items (No Scrolling Needed)
**Setup**: Settings menu with exactly 4 items (original configuration)
**Expected Behavior**:
- All 4 items should be visible at once
- No scrolling should occur when navigating
- Selection should move from item 0 to item 3 and vice versa

### Test 2: Menu with 7 Items (Scrolling Required)
**Setup**: Settings menu with 7 items (current test configuration)
**Expected Behavior**:

#### Navigation from Top to Bottom:
1. Initial state: Items 0-3 visible, item 0 selected
2. Scroll down once: Items 0-3 visible, item 1 selected
3. Scroll down twice more: Items 0-3 visible, item 3 selected
4. Scroll down once: Items 1-4 visible, item 4 selected (scroll occurs!)
5. Scroll down once: Items 2-5 visible, item 5 selected (scroll occurs!)
6. Scroll down once: Items 3-6 visible, item 6 selected (scroll occurs!)

#### Navigation from Bottom to Top:
1. Starting at item 6: Items 3-6 visible
2. Scroll up once: Items 3-6 visible, item 5 selected
3. Scroll up twice more: Items 3-6 visible, item 3 selected
4. Scroll up once: Items 2-5 visible, item 2 selected (scroll occurs!)
5. Scroll up once: Items 1-4 visible, item 1 selected (scroll occurs!)
6. Scroll up once: Items 0-3 visible, item 0 selected (scroll occurs!)

### Test 3: Edge Cases
**Tests**:
- Trying to scroll up when at item 0 (should stay at 0)
- Trying to scroll down when at last item (should stay at last)
- Fast scrolling through the menu
- Switching to settings screen should reset to item 0 with scroll offset 0

## Manual Testing Checklist

- [ ] Menu displays correctly with 4 items (backward compatibility)
- [ ] Menu displays correctly with 7 items
- [ ] Scrolling down works smoothly
- [ ] Scrolling up works smoothly
- [ ] Selected item is always visible
- [ ] Screen resets to top when entering settings
- [ ] No visual glitches during scrolling
- [ ] Button press on each item still works (if implemented)
- [ ] Performance is acceptable (no lag)

## Code Review Checklist

- [x] settingsScrollOffset is properly initialized to 0
- [x] settingsScrollOffset is reset when entering settings screen
- [x] Scroll offset calculation prevents out-of-bounds access
- [x] Only visible items are rendered
- [x] Selected item visibility check is correct
- [x] Menu item count is calculated dynamically
- [x] No hardcoded item limits remain in the code

## Known Limitations

1. Currently tested with 7 items - should work with any number
2. MAX_VISIBLE_ITEMS is set to 4 based on screen space
3. Menu items are defined in settingsScreen.cpp - no runtime modification
4. Each item takes ~54px vertically (ITEM_SPACING)

## Future Enhancements

- Add visual indicators (arrows) showing more items above/below
- Implement smooth scrolling animation
- Add page-based scrolling for long lists
- Support dynamic menu items (runtime addition/removal)
