#include "arduino_compat.h"
#include "soc/gpio_struct.h"

// Instantiate the Serial object
SerialClass Serial;

// GPIO register access - point to the actual GPIO registers
// For ESP32-S3, use gpio_dev_t structure
#define GPIO_REG GPIO
