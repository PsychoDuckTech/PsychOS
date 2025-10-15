# Serial Command Reference

This document provides comprehensive documentation for all available serial commands in PsychOS keyboard firmware.

## Command Format

Commands are sent via serial interface at 115200 baud rate. Format:
```
<command> <value>
<command>?
```

- `<command>`: Command name (case-sensitive)
- `<value>`: Parameter value (integer for numeric commands, string for text)
- `?`: Query suffix to get current value

## Available Commands

### Time Commands

#### `hours`
Set or get the current hour.

**Parameters:**
- Set: `hours <0-23>`
- Get: `hours?`

**Examples:**
```
hours 14      # Set to 2 PM
hours?        # Returns: hours is: 14
```

#### `minutes`
Set or get the current minute.

**Parameters:**
- Set: `minutes <0-59>`
- Get: `minutes?`

**Examples:**
```
minutes 30    # Set to 30 minutes past hour
minutes?      # Returns: minutes is: 30
```

#### `seconds`
Set or get the current second.

**Parameters:**
- Set: `seconds <0-59>`
- Get: `seconds?`

**Examples:**
```
seconds 45    # Set to 45 seconds
seconds?      # Returns: seconds is: 45
```

### Status Commands

#### `caps`
Set or get the caps lock status.

**Parameters:**
- Set: `caps <0-1>` (0=off, 1=on)
- Get: `caps?`

**Examples:**
```
caps 1        # Enable caps lock
caps 0        # Disable caps lock
caps?         # Returns: caps is: 1
```

### Media Commands

#### `songTitle`
Set or get the current song title for display.

**Parameters:**
- Set: `songTitle <text>` (any string)
- Get: `songTitle?`

**Examples:**
```
songTitle My Favorite Song
songTitle?    # Returns: Current song title: My Favorite Song
```

**Notes:**
- Song titles can contain spaces
- Maximum length: 128 characters
- Used by media integration features

## Command Response Format

### Successful Set Commands
Commands that set values return no response on success.

### Query Commands
Query commands return: `<command> is: <value>`

### Error Responses

#### Invalid Format
```
Invalid command format. Use: <command> <value> or <command>?
```

#### Invalid Value
```
Invalid value for <command>. Must be between <min> and <max>.
```

#### Unknown Command
```
Unknown command: <command>
```

## Command Table Reference

| Command | Type | Min Value | Max Value | Description |
|---------|------|-----------|-----------|-------------|
| hours | Time | 0 | 23 | Clock hours (24-hour format) |
| minutes | Time | 0 | 59 | Clock minutes |
| seconds | Time | 0 | 59 | Clock seconds |
| caps | Status | 0 | 1 | Caps lock state |
| songTitle | Media | - | - | Current song title |

## Usage Examples

### Setting Time
```
hours 9
minutes 30
seconds 0
```

### Checking Status
```
caps?
hours?
minutes?
```

### Media Integration
```
songTitle Now Playing - Artist Name
songTitle?
```

## Implementation Details

### Command Processing
- Commands are processed in the `commandProcessor` FreeRTOS task
- Input is buffered and parsed line-by-line
- Commands are case-sensitive
- Leading/trailing whitespace is trimmed

### Thread Safety
- All commands are processed in a single task
- No concurrent access issues with global state
- Responses are sent immediately

### Error Handling
- Invalid formats are rejected with descriptive messages
- Value bounds checking prevents invalid states
- Unknown commands are logged with command name

## Python Driver Integration

The Python driver (`driver.py`) provides high-level interface:

```python
from driver import ESP32Driver

driver = ESP32Driver('/dev/ttyUSB0')
driver.connect()

# Set time
driver.send_command('hours 12')
driver.send_command('minutes 30')

# Get status
response = driver.send_command('caps?')
```

## Web Interface Commands

The web configuration interface (`index.html`) supports all serial commands through Web Serial API.

## Debugging

Enable command debugging by modifying `commandProcessor.cpp`:

```cpp
#define COMMAND_DEBUG true
```

This will log all command processing steps to serial output.

## Future Commands

Planned commands for future versions:
- `rgb <effect>` - Set RGB effect
- `brightness <0-100>` - Set display brightness
- `layer <0-3>` - Switch keymap layer (when implemented)
- `macro <id>` - Execute macro (when implemented)
- `eeprom save` - Save configuration to EEPROM
- `eeprom load` - Load configuration from EEPROM</content>
<parameter name="filePath">c:\Users\Gabii\Documents\PsychoDuck\PsychOS\source\SERIAL_COMMANDS.md