What kind of issue is this?

- [ ] **PlatformIO Core**.
      If you’ve found a bug, please provide an information below.

------------------------------------------------------------------

### Configuration

**Operating system**: Ubuntu 24.10

**PlatformIO Version** (`platformio --version`): PlatformIO Core, version 6.1.16

### Description of problem
Including

#### Steps to Reproduce

1.
2.
3.

### Actual Results


### Expected Results


### If problems with PlatformIO Build System:

**The content of `platformio.ini`:**
```ini
Insert here...
```

**Source file to reproduce issue:**
```cpp
Insert here...
```

### Additional info






In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:92: warning: "CFG_TUH_ENABLED" redefined
 #define CFG_TUH_ENABLED 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:318: note: this is the location of the previous definition
   #define CFG_TUH_ENABLED     (TUH_RHPORT_MODE & OPT_MODE_HOST)
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:93: warning: "CFG_TUH_MAX_SPEED" redefined
 #define CFG_TUH_MAX_SPEED OPT_MODE_FULL_SPEED
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:323: note: this is the location of the previous definition
   #define CFG_TUH_MAX_SPEED   (TUH_RHPORT_MODE & OPT_MODE_SPEED_MASK)
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:94: warning: "CFG_TUH_MAX3421" redefined
 #define CFG_TUH_MAX3421 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:267: note: this is the location of the previous definition
   #define CFG_TUH_MAX3421  0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:104: warning: "CFG_TUH_HUB" redefined
 #define CFG_TUH_HUB 1
Compiling .pio/build/esp32-s3-devkitc-1/lib0b9/ArduinoBLE/local/BLELocalCharacteristic.cpp.o
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:512: note: this is the location of the previous definition
   #define CFG_TUH_HUB    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:113: warning: "CFG_TUH_MSC" redefined
 #define CFG_TUH_MSC 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:569: note: this is the location of the previous definition
   #define CFG_TUH_MSC    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:117: warning: "CFG_TUH_HID" redefined
 #define CFG_TUH_HID (3 * CFG_TUH_DEVICE_MAX)
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:561: note: this is the location of the previous definition
   #define CFG_TUH_HID    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:121: warning: "CFG_TUH_CDC" redefined
 #define CFG_TUH_CDC 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:516: note: this is the location of the previous definition
   #define CFG_TUH_CDC    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:122: warning: "CFG_TUH_CDC_FTDI" redefined
 #define CFG_TUH_CDC_FTDI 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:521: note: this is the location of the previous definition
   #define CFG_TUH_CDC_FTDI 0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:123: warning: "CFG_TUH_CDC_CP210X" redefined
 #define CFG_TUH_CDC_CP210X 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:534: note: this is the location of the previous definition
   #define CFG_TUH_CDC_CP210X 0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from include/tasks/matrixScan.cpp:2,
                 from src/main.cpp:4:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:124: warning: "CFG_TUH_CDC_CH34X" redefined
 #define CFG_TUH_CDC_CH34X 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/common/tusb_common.h:74,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/class/hid/hid.h:34,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.h:22,
                 from /home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHIDConsumerControl.h:16,
                 from include/tasks/hostCommunicationBridge.h:3,
Compiling .pio/build/esp32-s3-devkitc-1/lib0b9/ArduinoBLE/local/BLELocalDescriptor.cpp.o
                 from include/tasks/hostCommunicationBridge.cpp:1,
                 from src/config.h:6,
                 from src/main.cpp:3:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:545: note: this is the location of the previous definition
   #define CFG_TUH_CDC_CH34X 0





In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:92: warning: "CFG_TUH_ENABLED" redefined
 #define CFG_TUH_ENABLED 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:318: note: this is the location of the previous definition
   #define CFG_TUH_ENABLED     (TUH_RHPORT_MODE & OPT_MODE_HOST)
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:93: warning: "CFG_TUH_MAX_SPEED" redefined
 #define CFG_TUH_MAX_SPEED OPT_MODE_FULL_SPEED
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:323: note: this is the location of the previous definition
   #define CFG_TUH_MAX_SPEED   (TUH_RHPORT_MODE & OPT_MODE_SPEED_MASK)
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:94: warning: "CFG_TUH_MAX3421" redefined
 #define CFG_TUH_MAX3421 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:267: note: this is the location of the previous definition
   #define CFG_TUH_MAX3421  0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:104: warning: "CFG_TUH_HUB" redefined
 #define CFG_TUH_HUB 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:512: note: this is the location of the previous definition
   #define CFG_TUH_HUB    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:113: warning: "CFG_TUH_MSC" redefined
 #define CFG_TUH_MSC 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:569: note: this is the location of the previous definition
   #define CFG_TUH_MSC    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:117: warning: "CFG_TUH_HID" redefined
 #define CFG_TUH_HID (3 * CFG_TUH_DEVICE_MAX)
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:561: note: this is the location of the previous definition
   #define CFG_TUH_HID    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:121: warning: "CFG_TUH_CDC" redefined
 #define CFG_TUH_CDC 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:516: note: this is the location of the previous definition
   #define CFG_TUH_CDC    0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:122: warning: "CFG_TUH_CDC_FTDI" redefined
 #define CFG_TUH_CDC_FTDI 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:521: note: this is the location of the previous definition
   #define CFG_TUH_CDC_FTDI 0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:123: warning: "CFG_TUH_CDC_CP210X" redefined
 #define CFG_TUH_CDC_CP210X 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:534: note: this is the location of the previous definition
   #define CFG_TUH_CDC_CP210X 0
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/Adafruit_TinyUSB.h:30,
                 from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:29:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/ports/esp32/tusb_config_esp32.h:124: warning: "CFG_TUH_CDC_CH34X" redefined
 #define CFG_TUH_CDC_CH34X 1
 
In file included from .pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/Adafruit_TinyUSB_API.cpp:25:
.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb_option.h:545: note: this is the location of the previous definition
   #define CFG_TUH_CDC_CH34X 0










/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: .pio/build/esp32-s3-devkitc-1/lib2b5/libUSB.a(USBHID.cpp.o): in function `tud_hid_descriptor_report_cb':
/home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.cpp:223: multiple definition of `tud_hid_descriptor_report_cb'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(Adafruit_USBD_HID.cpp.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/hid/Adafruit_USBD_HID.cpp:188: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: .pio/build/esp32-s3-devkitc-1/lib2b5/libUSB.a(USBHID.cpp.o): in function `tud_hid_get_report_cb':
/home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.cpp:256: multiple definition of `tud_hid_get_report_cb'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(Adafruit_USBD_HID.cpp.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/hid/Adafruit_USBD_HID.cpp:203: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: .pio/build/esp32-s3-devkitc-1/lib2b5/libUSB.a(USBHID.cpp.o): in function `tud_hid_set_report_cb':
/home/gabi/.platformio/packages/framework-arduinoespressif32/libraries/USB/src/USBHID.cpp:266: multiple definition of `tud_hid_set_report_cb'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(Adafruit_USBD_HID.cpp.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/arduino/hid/Adafruit_USBD_HID.cpp:217: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tusb_inited':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:62: multiple definition of `tusb_inited'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:102: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_desc_find':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:81: multiple definition of `tu_desc_find'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:137: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_desc_find2':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:91: multiple definition of `tu_desc_find2'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:145: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_desc_find3':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:101: multiple definition of `tu_desc_find3'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:153: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_claim':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:115: multiple definition of `tu_edpt_claim'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:165: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_release':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:133: multiple definition of `tu_edpt_release'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:182: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_validate':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:150: multiple definition of `tu_edpt_validate'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:196: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_bind_driver':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:189: multiple definition of `tu_edpt_bind_driver'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:231: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_desc_get_interface_total_len':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:208: multiple definition of `tu_desc_get_interface_total_len'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:245: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_stream_init':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:243: multiple definition of `tu_edpt_stream_init'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:277: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_stream_write_zlp_if_needed':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:317: multiple definition of `tu_edpt_stream_write_zlp_if_needed'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:347: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_stream_write_xfer':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:328: multiple definition of `tu_edpt_stream_write_xfer'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:356: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_stream_write':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:352: multiple definition of `tu_edpt_stream_write'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:376: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_stream_read_xfer':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:372: multiple definition of `tu_edpt_stream_read_xfer'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:420: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(tusb.c.obj): in function `tu_edpt_stream_read':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/tusb.c:405: multiple definition of `tu_edpt_stream_read'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(tusb.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/tusb.c:455: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_speed_get':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:333: multiple definition of `tud_speed_get'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:412: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_connected':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:337: multiple definition of `tud_connected'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:416: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_mounted':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:341: multiple definition of `tud_mounted'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:420: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_suspended':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:345: multiple definition of `tud_suspended'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:424: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_remote_wakeup':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:349: multiple definition of `tud_remote_wakeup'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:428: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_disconnect':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:356: multiple definition of `tud_disconnect'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:435: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_connect':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:362: multiple definition of `tud_connect'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:440: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_inited':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:371: multiple definition of `tud_inited'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:452: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj):/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:270: multiple definition of `_usbd_qdef'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:339: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_task_event_ready':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:442: multiple definition of `tud_task_event_ready'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:557: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `dcd_event_handler':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1072: multiple definition of `dcd_event_handler'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1162: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_int_set':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1138: multiple definition of `usbd_int_set'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1236: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_defer_func':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1173: multiple definition of `usbd_defer_func'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1271: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_open':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1189: multiple definition of `usbd_edpt_open'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1286: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_open_edpt_pair':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1150: multiple definition of `usbd_open_edpt_pair'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1248: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_claim':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1199: multiple definition of `usbd_edpt_claim'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1295: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_release':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1213: multiple definition of `usbd_edpt_release'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1308: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_xfer':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1224: multiple definition of `usbd_edpt_xfer'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1318: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_xfer_fifo':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1261: multiple definition of `usbd_edpt_xfer_fifo'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1357: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_busy':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1292: multiple definition of `usbd_edpt_busy'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1385: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_stall':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1302: multiple definition of `usbd_edpt_stall'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1394: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_clear_stall':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1319: multiple definition of `usbd_edpt_clear_stall'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1407: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `tud_task_ext':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:468: multiple definition of `tud_task_ext'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:577: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_stalled':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1336: multiple definition of `usbd_edpt_stalled'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1420: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_close':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1352: multiple definition of `usbd_edpt_close'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1433: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_sof_enable':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1370: multiple definition of `usbd_sof_enable'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1454: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_iso_alloc':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1379: multiple definition of `usbd_edpt_iso_alloc'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1471: first defined here
/home/gabi/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/gabi/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libarduino_tinyusb.a(usbd.c.obj): in function `usbd_edpt_iso_activate':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/components/arduino_tinyusb/tinyusb/src/device/usbd.c:1389: multiple definition of `usbd_edpt_iso_activate'; .pio/build/esp32-s3-devkitc-1/lib4de/libAdafruit TinyUSB Library.a(usbd.c.o):/home/gabi/Documents/GitHub/PsychoDuck/PsychOS/source/.pio/libdeps/esp32-s3-devkitc-1/Adafruit TinyUSB Library/src/device/usbd.c:1483: first defined here
collect2: error: ld returned 1 exit status
*** [.pio/build/esp32-s3-devkitc-1/firmware.elf] Error 1
======================================================================= [FAILED] Took 10.89 seconds =======================================================================