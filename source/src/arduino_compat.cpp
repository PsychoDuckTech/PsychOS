#include "arduino_compat.h"
#include "soc/gpio_struct.h"

// Instantiate the Serial object
SerialClass Serial;

// GPIO register access
volatile uint32_t* REG_GPIO_BASE = (volatile uint32_t*)&GPIO;
