# PsychOS

A feature-rich firmware for custom mechanical keyboards, built on ESP32-S3.

## Supported Matrix Hardware Setups
|   Device    |           Status           |       Notes        |
| :---------: |  :----------------------:  |  :-------------:   |
| Prototype 0 |   ![alt text][supported]   |  Actively tweaked  |
| 2x2 Matrix  |  ![alt text][development]  | Temporarily halted |

[supported]: https://img.shields.io/badge/-supported-green "supported"
[preview]: https://img.shields.io/badge/-preview-orange "preview"
[development]: https://img.shields.io/badge/-unsupported-red "in development"

## Core Features

|              Feature              |     Status      |                    Notes                    |
| :------------------------------: | :-------------: | :-----------------------------------------: |
|       Matrix Key Scanning        |    Supported    |     High-performance scanning with debounce  |
|    USB HID Communication        |    Supported    |       Full keyboard and consumer input       |
|    Bluetooth Module Support     |    Supported    |   Wireless numpad module connectivity       |
|     Display Interface          |    Supported    |    ILI9341-based UI with custom screens    |
|     Rotary Encoder            |    Supported    |    Volume control with press detection     |
|     RGB Underglow             |    Supported    |    Customizable effects and colors        |
|     WPM Counter              |    Supported    |    Real-time typing speed monitoring      |
|     Multi-tasking            |    Supported    |    FreeRTOS-based task management        |
|     Settings System          |    Supported    |    On-device configuration menus         |
|     Audio Feedback           |    Supported    |    Programmable buzzer tones             |
|     Clock                    |    Supported    |    Real-time clock with sync            |
|     Layer System             |       WIP       |    Multiple keymap layer support         |
|     Macro Support            |   Planned    |    Custom macro programming              |

## Technical Features
- **Task Management**: FreeRTOS-based multitasking system
- **Communication**:
  - USB HID for keyboard and consumer controls
  - Bluetooth LE for wireless module support
  - Serial interface for configuration and debugging
- **Input Processing**:
  - Advanced matrix scanning with hardware multiplexing
  - Rotary encoder with acceleration and button detection
  - Real-time WPM calculation
- **Output Systems**:
  - TFT display with custom UI components
  - RGB LED control with multiple effects
  - Buzzer feedback system
- **Configuration**:
  - Non-volatile settings storage (not functional at the moment)
  - Real-time clock synchronization
  - On-device settings menu

## Serial Commands
The firmware supports the following commands:
- `connectionStatus [0/1/?]` - Get/set connection state
- `caps [0/1/?]` - Get/set caps lock status
- `time.hours [0-23]` - Set hours
- `time.minutes [0-59]` - Set minutes
- `time.seconds [0-59]` - Set seconds

## Installation

### Hardware Requirements
- ESP32-S3 microcontroller
- Key matrix setup (compatible with standard mechanical switches)
- ILI9341 display (optional)
- Rotary encoder (optional)
- RGB LEDs (optional)
- Buzzer (optional)

### Flashing Instructions
1. Download the latest `firmware.bin` from the releases page
2. Install esptool: `pip install esptool`
3. Put your ESP32-S3 into bootloader mode
4. Flash using esptool

## Development

### Adding Translations
Translations are managed through two main files:
- `source/include/translations.h`: Declares all translatable strings
- `source/src/translations.cpp`: Implements the translations

To add a new language:
1. Define a new language identifier (e.g., `USE_FR_FR` for French)
2. Add translations in `translations.cpp` under the new language condition
3. Keep the string declarations in `translations.h` unchanged

Example:
```cpp
#ifdef USE_FR_FR
// System messages
const char *task_clock_started = "Tâche d'horloge démarrée.";
// ... more translations ...
#endif
```

Contributions for new languages are welcome! Please maintain the existing structure when adding translations.

### Versioning
Following Semantic Versioning 2.0.0:
- Format: `MAJOR.MINOR.PATCH-PRERELEASE+BUILD`
- Pre-release stages: `alpha` < `beta` < `rc`
- Build metadata: Date (YYYYMMDD) and CI run number

## Project Activity
![Alt](https://repobeats.axiom.co/api/embed/41e28f23eb43e17ac67db3d966de3dd565079220.svg "Repobeats analytics image")

## License
Licensed under [LGPL-2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) unless otherwise noted.
Third-party components (e.g., FreeRTOS) maintain their respective licenses.

## Commercial Use
This firmware is provided "AS IS" without commercial support. You may:
- Distribute links to this firmware
- Ship hardware with this firmware pre-installed

Please link to this repository rather than redistributing the files to ensure users always get the latest version.
