# BLE (Bluetooth Low Energy) System Documentation

This document describes the BLE connectivity features in PsychOS keyboard firmware, including module communication, service architecture, and integration options.

## Overview

The BLE system enables wireless connectivity with external keyboard modules, allowing expansion of the keyboard with additional keys, controls, or sensors. The system supports up to 4 concurrent module connections with automatic discovery and management.

## Architecture

### BLE Service Configuration

```cpp
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
```

### Device Information

- **Device Name**: "Kibodo One"
- **Appearance**: HID Keyboard (0x03C1)
- **Connection Interval**: 15ms - 30ms
- **MTU**: 23 bytes (default)

## Module Communication Protocol

### Message Format

BLE messages use a simple binary protocol:

```
Byte 0: Key Code (0-255)
Byte 1: State (0=Release, 1=Press)
```

### Key Code Mapping

Modules send standard HID key codes (0x00-0xFF) that are translated to USB HID reports.

### Connection Management

#### Automatic Discovery

```cpp
BLEDevice central = BLE.central();
if (central && central.connected()) {
    // Handle new connection
    moduleConnectionStatus = true;
    // Register module statistics
}
```

#### Connection Monitoring

- **RSSI Tracking**: Signal strength monitoring
- **Activity Timeout**: 30-second inactivity disconnect
- **Reconnection**: Automatic advertising restart on disconnect

#### Module Statistics

```cpp
struct ModuleStat {
    String address;           // BLE MAC address
    int keyPresses;          // Total key presses
    unsigned long connectTime; // Connection timestamp
    int rssi;                // Signal strength (-127 to 0 dBm)
};
```

## Supported Message Types

### Key Events

Primary communication type for keyboard input:

```cpp
enum class BLEMessageType {
    KEY_EVENT,           // Key press/release
    CAPS_LOCK_STATUS,    // Caps lock state sync
    RGB_CONTROL,         // RGB lighting control
    SYSTEM_STATUS,       // System information
    CUSTOM_COMMAND       // Extensible commands
};
```

### System Integration

#### Host Communication Bridge

BLE key events are forwarded to the USB HID interface:

```cpp
HostMessage msg;
msg.type = isPressed ? KEY_PRESS : KEY_RELEASE;
msg.data = keyCode;
xQueueSend(hostMessageQueue, &msg, 0);
```

#### Stuck Key Prevention

The system tracks active keys and releases them on disconnection:

```cpp
for (int i = 0; i < activeKeyCount; i++) {
    HostMessage msg;
    msg.type = KEY_RELEASE;
    msg.data = activeModuleKeys[i];
    xQueueSend(hostMessageQueue, &msg, 0);
}
```

## Module Development

### BLE Module Requirements

1. **ESP32** or compatible microcontroller
2. **ArduinoBLE** library compatibility
3. **Matching Service UUIDs**
4. **HID key code knowledge**

### Sample Module Code

```cpp
#include <ArduinoBLE.h>

BLEService keyboardService(SERVICE_UUID);
BLECharacteristic keyboardChar(CHARACTERISTIC_UUID,
                              BLERead | BLEWrite | BLENotify, 20);

void setup() {
    BLE.begin();
    BLE.setLocalName("Keyboard Module");
    keyboardService.addCharacteristic(keyboardChar);
    BLE.addService(keyboardService);
    BLE.advertise();
}

void sendKeyPress(uint8_t keyCode) {
    uint8_t data[2] = {keyCode, 1}; // Press
    keyboardChar.writeValue(data, 2);
    delay(50);
    data[1] = 0; // Release
    keyboardChar.writeValue(data, 2);
}
```

### Key Code Reference

Common key codes for module development:

| Key | Code | Key | Code | Key | Code |
|-----|------|-----|------|-----|------|
| A | 0x04 | 1 | 0x1E | F1 | 0x3A |
| B | 0x05 | 2 | 0x1F | F2 | 0x3B |
| C | 0x06 | 3 | 0x20 | Space | 0x2C |
| Enter | 0x28 | ESC | 0x29 | Left Ctrl | 0xE0 |

## Performance Characteristics

### Latency
- **Connection**: <100ms
- **Key Press**: 15-30ms
- **Reconnection**: <200ms

### Power Consumption
- **Advertising**: ~20mA
- **Connected Idle**: ~15mA
- **Active Transmission**: ~25mA

### Range
- **Line-of-sight**: 10-15 meters
- **Through walls**: 5-8 meters
- **Interference sensitivity**: High (2.4GHz WiFi)

## Troubleshooting

### Connection Issues

1. **Module not discovered**
   - Verify BLE is enabled on both devices
   - Check service UUIDs match exactly
   - Ensure modules are in advertising mode

2. **Unstable connection**
   - Check RSSI values (> -70 dBm recommended)
   - Reduce interference (move away from WiFi routers)
   - Update to latest ArduinoBLE library

3. **Key presses not registering**
   - Verify key codes are valid HID codes
   - Check message format (2 bytes: code + state)
   - Monitor serial debug output

### Debug Information

Enable BLE debugging in `BLEHandler.cpp`:

```cpp
#define BLE_DEBUG true
```

This provides detailed connection logging and error reporting.

## Security Considerations

### Current Implementation
- **No encryption**: Open BLE service
- **No authentication**: Any device can connect
- **No pairing**: Automatic connection acceptance

### Recommended Security (Future)
- **BLE pairing**: Secure device pairing
- **Service encryption**: Encrypted characteristic access
- **Connection filtering**: MAC address whitelisting

## Advanced Features

### Multi-Module Coordination

```cpp
// Track multiple modules
for (int i = 0; i < numModules; i++) {
    if (moduleStats[i].address == targetAddress) {
        // Module-specific handling
    }
}
```

### RSSI-Based Features

```cpp
int rssi = central.rssi();
if (rssi > -50) {
    // Strong signal - enable advanced features
} else if (rssi < -80) {
    // Weak signal - reduce update frequency
}
```

### Connection Quality Monitoring

```cpp
unsigned long now = millis();
if (now - lastDataReceivedTime > 30000) {
    // No data for 30 seconds - consider disconnected
    handleDisconnection();
}
```

## Integration Examples

### Python Control Script

```python
import asyncio
from bleak import BleakClient

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def send_key(client, key_code):
    data = bytes([key_code, 1])  # Press
    await client.write_gatt_char(CHAR_UUID, data)
    await asyncio.sleep(0.05)
    data = bytes([key_code, 0])  # Release
    await client.write_gatt_char(CHAR_UUID, data)

async def main():
    async with BleakClient("DEVICE_ADDRESS") as client:
        await send_key(client, 0x04)  # Send 'A' key
```

### Web Bluetooth API

```javascript
const SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

async function connectModule() {
    const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: [SERVICE_UUID] }]
    });

    const server = await device.gatt.connect();
    const service = await server.getPrimaryService(SERVICE_UUID);
    const characteristic = await service.getCharacteristic(CHAR_UUID);

    // Send key press
    await characteristic.writeValue(new Uint8Array([0x04, 1]));
}
```

## Future Enhancements

### Planned Features
- **BLE HID Profile**: Standard HID over BLE
- **Battery reporting**: Module battery level monitoring
- **Firmware updates**: OTA updates for modules
- **Mesh networking**: Multi-hop module communication
- **Security features**: Pairing and encryption
- **Advanced key mapping**: Per-module key remapping

### Extended Protocol
- **Larger MTU**: Support for 512-byte packets
- **Bulk transfers**: Configuration data transfer
- **Real-time sync**: Clock and settings synchronization
- **Sensor data**: Accelerometer, gyroscope integration</content>
<parameter name="filePath">c:\Users\Gabii\Documents\PsychoDuck\PsychOS\source\BLE_DOCUMENTATION.md