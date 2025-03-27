<div align="center">
  <h1>PsychOS</h1>
</div>

## Supported Matrix Hardware Setups
|   Device    |           Status           |       Notes        |
| :---------: |  :----------------------:  |  :-------------:   |
| Prototype 0 |   ![alt text][supported]   |  Actively tweaked  |
| 2x2 Matrix  |  ![alt text][development]  | Temporarily halted |

[supported]: https://img.shields.io/badge/-supported-green "supported"
[preview]: https://img.shields.io/badge/-preview-orange "preview"
[development]: https://img.shields.io/badge/-unsupported-red "in development"

## Features
|                                Feature                                |     Status      |                    Notes                    |
| :-------------------------------------------------------------------: | :-------------: | :-----------------------------------------: |
|                  Keystroke Handler & Matrix Scanning                  |    Supported    |        TinyUSB library issues fixed         |
|                       Host Communication Bridge                       | Partial Support | Keyboard communication to the host machine. |
|                       Modifiable Configuration                        |   Unsupported   |          On-the-fly configuration           |
| [Web Configurator](https://github.com/PsychoDuckTech/WebConfigurator) |   Unsupported   |  Web-based configurator for the firmware.   |
|                            Rotary Encoder                             |    Supported    |            Rotary Knob support.             |
|                             Macro Support                             |   Unsupported   |  Macro support for the main board matrix.   |
|                                Modules                                |    Supported    |      Bluetooth Numpad Modules support.      |
|                                Display                                |    Supported    |                  Functional                 |
|                                Layers                                 |       WIP       |     Multiple toggle-able keymap layers.     |
|                                  WPM                                  |    Supported    |                  WPM counter                |
|                                  RGB                                  |    Supported    |        RGB Underglow on the keyboard        |


## Current Features
- USB HID Communication
- Volume Control via Rotary Encoder
  - Smooth increment/decrement
  - Mute function via encoder press
- FreeRTOS Task Management
- Matrix Scanning
- WPM counter
- Serial Commands System
- RGB underglow system
- Buzzer tones
- Clock
- Builtin settings menus

## Serial Commands and Syntax
- connectionStatus [0/1/?]
- caps [0/1/?]
- time.seconds [time 0 to 60]
- time.minutes [time 0 to 60]
- time.hours [time 0 to 23]

## Flashing the Firmware
Once the compilation finishes, go to the releases tab and find the latest release. Download the `firmware.bin` file.
Set your microcontroller to bootloader mode, then use the `esptool` command to flash the firmware.

If you don't have the [esptool](https://github.com/espressif/esptool) installed, install it using the command: `pip install esptool`.

## Translations
Is your preferred language missing localisation of some of the text?
Translations are stored as `.h` files in the `source/include/translations` folder.
_Pull requests_ are loved and accepted to enhance the firmware. <3

## Versioning Scheme
We follow [Semantic Versioning 2.0.0](https://semver.org/) with:
- Format: `MAJOR.MINOR.PATCH-PRERELEASE+BUILD`
- Pre-release stages: `alpha` < `beta` < `rc`
- Build metadata: Date (YYYYMMDD) and CI run number

## About
This firmware was tested only on the ESP32-S3.

Developed by PsychoDuck Tech.

## License
This code is covered by the [LGPL-2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) license **unless noted elsewhere**.
Other components such as _FreeRTOS_ have their own licenses.

## Commercial Use
This software is provided "AS IS", so we cannot provide any commercial support for the firmware.
However, you are more than welcome to distribute links to the firmware or provide hardware with this firmware.
**Please do not re-host the files, but rather link to this page, so that there are no old versions of the firmware scattered around**.
