import serial
import time
import keyboard  # Install via `pip install keyboard`

# Update with the correct COM port for your ESP32
ser = serial.Serial('COM3', 9600)

while True:
    if keyboard.is_pressed('caps lock'):
        ser.write(b'CAPS_ON\n')
    else:
        ser.write(b'CAPS_OFF\n')
    time.sleep(0.1)
