# PsychOS Keyboard Firmware

PsychOS is a custom keyboard firmware for the Kibodo One keyboard, built on ESP32-S3 with advanced features including BLE connectivity, RGB lighting, display integration, and comprehensive media controls.

## Features

- **6x16 Matrix Keyboard** with multiplexer support
- **USB HID Interface** for standard keyboard functionality
- **BLE Connectivity** for wireless modules and connections
- **RGB LED Backlighting** with customizable effects
- **TFT Display** with multiple screens (clock, settings, media, etc.)
- **Rotary Encoder** with volume control and navigation
- **Buzzer** for audio feedback
- **Real-time Clock** with NTP synchronization
- **Media Controls** with MPRIS integration
- **Serial Command Interface** for configuration
- **Web-based Configuration** via Web Serial API
- **Words Per Minute (WPM) Counter**
- **Benchmarking Tools** for performance monitoring

## Hardware Requirements

- ESP32-S3 DevKitC-1
- 320x240 ILI9341 TFT Display
- 16-channel analog multiplexer (CD74HC4067)
- Rotary encoder with switch
- Buzzer
- NeoPixel LED strip
- 6x16 keyboard matrix

## Pin Configuration

| Component | Pins |
|-----------|------|
| Matrix Rows | GPIO 21, 4, 14, 10, 11, 9 |
| Multiplexer | S0:5, S1:6, S2:7, S3:15, SIG:35 |
| Display | CS:13, RST:17, RS:16, MOSI:12, SCK:18 |
| Rotary Encoder | CLK:36, DT:2, SW:0 |
| Buzzer | GPIO 8 |
| NeoPixel | GPIO 3 (default) |

## Framework

**Note**: This firmware has been migrated from Arduino Framework to **ESP-IDF** for better performance and native ESP32-S3 support. See [ESP-IDF_MIGRATION.md](ESP-IDF_MIGRATION.md) for details.

## Installation

### Prerequisites

1. Install PlatformIO Core or PlatformIO IDE
2. ESP-IDF will be automatically installed by PlatformIO

### Building and Flashing

1. Clone the repository:
```bash
git clone <repository-url>
cd PsychOS/source
```

2. Connect your ESP32-S3 DevKitC-1

3. Build and upload:
```bash
pio run -t upload
```

4. Monitor serial output:
```bash
pio device monitor
```

### Testing

Run the unit tests:
```bash
pio test
```

## Configuration

### Key Mapping

The keyboard uses a single layer (Layer 0) with the following layout:

```
ESC F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 SYSRQ INS DEL
 \  1  2  3  4  5  6  7  8  9  0  '     BKSP     HOME
TAB Q  W  E  R  T  Y  U  I  O  P  =  `        END
CPS A  S  D  F  G  H  J  K  L  Ç  º  ~  ENTER    PGUP
LSH    Z  X  C  V  B  N  M  ,  .  -  RSH    UP   PGDN
LCTL LWI    LALT       SPC          RALT    RCTL LEFT DOWN RIGHT
```

### Serial Commands

Configure the keyboard via serial interface:

| Command | Description | Range | Example |
|---------|-------------|-------|---------|
| `hours <value>` | Set clock hours | 0-23 | `hours 14` |
| `minutes <value>` | Set clock minutes | 0-59 | `minutes 30` |
| `seconds <value>` | Set clock seconds | 0-59 | `seconds 45` |
| `caps <value>` | Set/get caps lock status | 0-1 | `caps 1` |
| `songTitle <text>` | Set current song title | - | `songTitle My Song` |
| `<command>?` | Query current value | - | `hours?` |

### RGB Configuration

Configure RGB lighting effects:

```cpp
RGBConfig config;
config.effect = RGB_EFFECT_RUNNER;  // Effect type
config.colors[0] = "#FFA500";       // Orange
config.colors[1] = "#FF69B4";       // Hot Pink
config.colors[2] = "#800080";       // Purple
config.numColors = 3;
config.speed = 20;                  // 1-20 scale
config.brightness = 43;             // 0-100 scale
uRGB.configure(config);
```

Available effects:
- `RGB_EFFECT_RUNNER`
- `RGB_EFFECT_STATIC`
- `RGB_EFFECT_BREATHE`
- `RGB_EFFECT_WAVE`

### BLE Configuration

The firmware supports BLE connectivity for external modules. Configure service UUIDs in `globals.h`:

```cpp
#define SERVICE_UUID "your-service-uuid"
#define CHARACTERISTIC_UUID "your-characteristic-uuid"
```

### Display Configuration

Multiple display screens are available:
- **Main Screen**: Current layer, WPM, connection status
- **Clock Screen**: Time display with NTP sync
- **Settings Screen**: Configuration options
- **Media Screen**: Current track information
- **RGB Screen**: Lighting configuration
- **Modules Screen**: Connected BLE modules

### Web Configuration

Use the web interface for easy configuration:

1. Open `index.html` in a Web Serial API compatible browser
2. Click "Connect to Keyboard"
3. Use the interface to configure settings

## Customization

### Adding New Commands

1. Add command to `commandTable` in `commandProcessor.cpp`
2. Implement update and get functions
3. Define min/max values

### Modifying Keymap

Edit `keyMapL0` and `keyNameL0` arrays in `main.cpp`

### Adding RGB Effects

1. Define new effect in RGB handler
2. Add to effect enumeration
3. Implement effect logic

### Display Screens

1. Create new screen function in appropriate display file
2. Add to screen rotation in display handler
3. Update UI components as needed

## BLE Module Integration

### Connecting Modules

1. Power on BLE module
2. Keyboard automatically scans for modules
3. Connection established when compatible module found

### Module Communication

Modules communicate via custom BLE service with key press/release events and configuration data.

## Media Integration

### MPRIS Support

The firmware integrates with MPRIS-compatible media players:

1. Automatically detects running media players
2. Displays current track information
3. Provides media control keys

### Supported Players

- VLC
- Spotify
- Rhythmbox
- Any MPRIS-compatible player

## Troubleshooting

### Common Issues

1. **Keyboard not recognized**
   - Check USB connection
   - Verify driver installation
   - Check serial output for errors

2. **BLE connection fails**
   - Ensure modules are powered
   - Check BLE service UUIDs match
   - Verify antenna placement

3. **Display not working**
   - Check wiring connections
   - Verify SPI pins
   - Check display power supply

4. **RGB not working**
   - Verify NeoPixel data pin
   - Check power supply
   - Confirm LED count

### Debug Information

Enable debug output by modifying `globals.h` defines and rebuilding.

## Development

### Project Structure

```
src/
├── main.cpp                 # Main firmware entry point
├── globals.h               # Global definitions and constants
├── translations.cpp        # UI text translations
├── index.html              # Web configuration interface
├── driver.py               # Python configuration driver
├── dbus_media_listener.py  # Media player integration
├── tasks/                  # FreeRTOS tasks
│   ├── matrixScan.cpp      # Key matrix scanning
│   ├── commandProcessor.cpp# Serial command processing
│   ├── BLEHandler.cpp      # BLE connectivity
│   ├── displayHandler.cpp  # Display management
│   ├── hostCommunicationBridge.cpp # USB HID interface
│   ├── rgbHandler.cpp      # RGB lighting
│   ├── buzzer.cpp          # Audio feedback
│   ├── clock.cpp           # Real-time clock
│   ├── mediaHandler.cpp    # Media controls
│   ├── knobHandler.cpp     # Rotary encoder
│   ├── serialHandler.cpp   # Serial communication
│   └── wpmCounter.cpp      # WPM tracking
├── display/                # Display screens
│   ├── mainScreen.cpp
│   ├── clockScreen.cpp
│   ├── settingsScreen.cpp
│   ├── mediaScreen.cpp
│   ├── rgbScreen.cpp
│   ├── modulesScreen.cpp
│   └── components/
├── drivers/                # Hardware drivers
├── fonts/                  # Custom fonts
├── utils/                  # Utility functions
└── lib/                    # External libraries
```

### Adding New Features

1. Create new task in `tasks/` directory
2. Add task initialization in `main.cpp`
3. Update headers and globals as needed
4. Add configuration options if required

### Testing

Run unit tests with:
```bash
pio test
```

Tests cover:
- Matrix scanning logic
- Command processing
- Key debouncing
- Key mapping validation

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## Support

For support and questions:
- Check the troubleshooting section
- Review serial debug output
- Open an issue on GitHub

## Version History

- **0.4.16b**: Current version
  - BLE module support
  - Web configuration interface
  - Media integration
  - RGB effects
  - Multiple display screens</content>
<parameter name="filePath">c:\Users\Gabii\Documents\PsychoDuck\PsychOS\source\README.md