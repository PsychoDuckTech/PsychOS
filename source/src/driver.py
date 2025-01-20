import time
import serial
import os
from datetime import datetime

class ESP32Driver:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.caps_lock_status = self.get_caps_lock_status()
        self.connection_status = False
        self.last_time_update = None
        self.connect_to_serial()

    def connect_to_serial(self):
        while True:
            try:
                self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
                print("Serial connection established.")
                self.update_connection_status(True)
                self.update_time_from_system()  # Update time once during initialization
                return
            except serial.SerialException as e:
                print(f"Failed to connect to serial port: {e}. Retrying in 1 seconds...")
                time.sleep(1)

    def send_command(self, command):
        if self.ser and self.ser.is_open:
            try:
                self.ser.write((command + "\n").encode())
                response = self.ser.readline().decode().strip()
                return response
            except Exception as e:
                raise ConnectionError(f"Failed to send command: {e}")
        else:
            raise ConnectionError("Serial connection is not open.")

    def update_variable(self, variable, value):
        command = f"{variable} {value}"
        response = self.send_command(command)
        print(response)

    def query_variable(self, variable):
        command = f"{variable}?"
        response = self.send_command(command)
        print(response)
        return response.split()[-1]  # Extract the value from the response

    def update_caps_lock_status(self):
        new_status = self.get_caps_lock_status()
        if new_status != self.caps_lock_status:
            self.caps_lock_status = new_status
            self.update_variable("caps", int(self.caps_lock_status))

    def update_connection_status(self, status):
        self.connection_status = status
        self.update_variable("connectionStatus", int(status))

    def update_time_from_system(self):
        now = datetime.now()
        self.update_variable("time.hours", now.hour)
        self.update_variable("time.minutes", now.minute)
        self.update_variable("time.seconds", now.second)
        self.last_time_update = now

    def check_and_update_time(self):
        now = datetime.now()
        if self.last_time_update is None or now.hour != self.last_time_update.hour:
            print("System time has changed, updating ESP32 time...")
            self.update_time_from_system()

    @staticmethod
    def get_caps_lock_status():
        caps_lock_state = os.popen('xset q | grep Caps | awk \'{print $4}\'').read().strip()
        return caps_lock_state == "on"

    def handle_reconnection(self):
        try:
            connection_status = int(self.query_variable("connectionStatus"))
            if connection_status == 0:
                print("ESP32 connection lost. Reinitializing variables...")
                self.update_connection_status(True)
                self.update_time_from_system()
                self.update_caps_lock_status()
        except (ValueError, ConnectionError):
            print("Connection to ESP32 lost. Attempting to reconnect...")
            self.reconnect()

    def reconnect(self):
        if self.ser:
            self.ser.close()
        self.connect_to_serial()

    def close(self):
        self.update_connection_status(False)
        if self.ser and self.ser.is_open:
            self.ser.close()

    def run_forever(self):
        try:
            while True:
                self.handle_reconnection()
                self.check_and_update_time()
                self.update_caps_lock_status()
                #time.sleep(0.1)  # Check every .1 seconds
        except KeyboardInterrupt:
            print("Exiting driver loop...")
        finally:
            self.close()

# Example usage:
if __name__ == "__main__":
    esp = ESP32Driver(port="/dev/ttyACM0")
    esp.run_forever()