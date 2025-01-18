import serial
import os
import time
import subprocess

# Find available USB ports
# List available ports
# Replace 'ttyUSB0' with the actual port if necessary
ports = [f'/dev/{port}' for port in ['ttyUSB0', 'ttyUSB1', 'ttyACM0'] if os.path.exists(f'/dev/{port}')]
if ports:
    ser = serial.Serial(ports[0], 115200)
    print(f"Connected to {ports[0]}")
else:
    print("No USB device found")
    exit()

def send_data(caps_lock_status, hour, minute):
    data = f"{caps_lock_status},{hour},{minute}\n"
    ser.write(data.encode())
    print(f"Sent: {data.strip()}")
    
def get_caps_lock_status():
    try:
        output = subprocess.check_output(['xset', 'q'])
        output = output.decode('utf-8')
        for line in output.splitlines():
            if 'Caps Lock:' in line:
                return 'on' in line.split()[3]
    except Exception as e:
        print(f"Error: {e}")
        return 0

# Initial values
caps_lock_status = "false"
hour = 12
minute = 34

try:
    toggle_interval = 5  # Toggle caps_lock_status every 5 seconds
    last_toggle_time = time.time()
    
    while True:
        start_time = time.time()
        caps_lock_status = get_caps_lock_status()
        
        # Update caps_lock_status every toggle_interval seconds
        if time.time() - last_toggle_time >= toggle_interval:
            caps_lock_status = not caps_lock_status
            last_toggle_time = time.time()
        
        # Update hour and minute every minute
        current_time = time.localtime()
        hour = current_time.tm_hour
        minute = current_time.tm_min
        
        send_data(caps_lock_status, hour, minute)
        
        # Calculate the time taken to send the data
        elapsed_time = time.time() - start_time
        
        # Sleep for the remaining time to maintain 100 Hz (10 ms interval)
        sleep_time = max(0, 0.01 - elapsed_time)
        time.sleep(sleep_time)
except KeyboardInterrupt:
    print("Exiting...")
finally:
    # Close the serial connection
    ser.close()