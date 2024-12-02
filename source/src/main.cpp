#include <Arduino.h>
#include <TinyUSB.h>
#include <SPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ILI9341_display.h"

// Include TinyUSB Keyboard and Mouse
#include "TinyUSB_Mouse_and_Keyboard.h"

#define MATRIX_ROWS 6
#define MATRIX_COLS 16

// Define key mapping (fill with appropriate HID key codes)
const uint8_t key_map[MATRIX_ROWS][MATRIX_COLS] = {
    // Row 0
    { HID_KEY_A, HID_KEY_B, HID_KEY_C, HID_KEY_D, HID_KEY_E, HID_KEY_F,
      HID_KEY_G, HID_KEY_H, HID_KEY_I, HID_KEY_J, HID_KEY_K, HID_KEY_L,
      HID_KEY_M, HID_KEY_N, HID_KEY_O, HID_KEY_P },
    // Add additional rows...
};

class KeyMatrix {
private:
    uint8_t row_pins[MATRIX_ROWS] = {1, 2, 3, 4, 5, 6};
    uint8_t col_pins[MATRIX_COLS] = {7, 8, 9, 10, 11, 12, 13, 14,
                                     15, 16, 17, 18, 19, 20, 21, 22};
    bool matrix_state[MATRIX_ROWS][MATRIX_COLS] = {0};

    void scan_matrix() {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            // Set current row high
            digitalWrite(row_pins[row], HIGH);

            for (int col = 0; col < MATRIX_COLS; col++) {
                bool current_state = digitalRead(col_pins[col]);

                if (current_state != matrix_state[row][col]) {
                    matrix_state[row][col] = current_state;
                    uint8_t keycode = key_map[row][col];

                    if (current_state) {
                        // Key pressed
                        Keyboard.press(keycode);
                    } else {
                        // Key released
                        Keyboard.release(keycode);
                    }
                }
            }

            // Set current row low
            digitalWrite(row_pins[row], LOW);
        }
    }

public:
    void begin() {
        // Configure row pins as outputs
        for (int i = 0; i < MATRIX_ROWS; i++) {
            pinMode(row_pins[i], OUTPUT);
            digitalWrite(row_pins[i], LOW);
        }

        // Configure column pins as inputs with pull-down
        for (int j = 0; j < MATRIX_COLS; j++) {
            pinMode(col_pins[j], INPUT_PULLDOWN);
        }

        // Initialize TinyUSB devices
        TinyUSBDevice.begin();
        Keyboard.begin();
    }

    void task() {
        scan_matrix();
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Debounce delay
    }
};

// Define display pin connections
#define TFT_MOSI 23
#define TFT_SCLK 24
#define TFT_CS   25
#define TFT_DC   26
#define TFT_RST  27

// Create instances
KeyMatrix keyMatrix;
ILI9341Display tft(TFT_MOSI, TFT_SCLK, TFT_CS, TFT_DC, TFT_RST);

void keyMatrixTask(void *pvParameters) {
    while (true) {
        keyMatrix.task();
    }
}

void displayTask(void *pvParameters) {
    while (true) {
        tft.fill_screen(ILI9341Display::color565(255, 0, 0));  // Red
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        tft.fill_screen(ILI9341Display::color565(0, 255, 0));  // Green
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    // Initialize Serial over USB CDC
    Serial.begin(115200);

    // Wait for Serial port to connect (necessary on some systems)
    while (!Serial) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    Serial.println("USB CDC Initialized");

    // Initialize components
    keyMatrix.begin();
    tft.initialize();

    // Create FreeRTOS tasks
    xTaskCreatePinnedToCore(
        keyMatrixTask, "KeyMatrixTask", 4096, NULL, 1, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(
        displayTask, "DisplayTask", 4096, NULL, 1, NULL, APP_CPU_NUM);
}

void loop() {
    // Handle USB CDC communication
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(input);
        // Process input data (e.g., configuration commands)
    }

    // Yield to FreeRTOS
    vTaskDelay(1 / portTICK_PERIOD_MS);
}