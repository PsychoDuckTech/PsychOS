import time
import serial
import os
from datetime import datetime
import subprocess

# Try to import dbus for media player detection
HAS_DBUS = False
try:
    import dbus
    HAS_DBUS = True
except ImportError:
    print("Warning: python-dbus not installed. Media player detection will be limited.")
    print("To install: sudo apt-get install python3-dbus")

class ESP32Driver:
    SLEEP_DURATION = 0.4  # General loop interval
    MEDIA_CHECK_INTERVAL = 5  # Check media every 5 seconds

    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.caps_lock_status = self.get_caps_lock_status()
        self.connection_status = False
        self.last_time_update = None
        self.last_media_check = time.time()
        self.current_media = None  # Cache for current media
        self.connect_to_serial()

    def connect_to_serial(self):
        while True:
            try:
                self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
                print("Serial connection established.")
                self.update_connection_status(True)
                self.update_time_from_system()
                return
            except serial.SerialException as e:
                print(f"Failed to connect to serial port: {e}. Retrying in 1 second...")
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
        return response.split()[-1]

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

    def update_song_title(self, title):
        try:
            max_title_length = 30
            if len(title) > max_title_length:
                title = title[:max_title_length-3] + "..."
            command = f"songTitle {title}"
            response = self.send_command(command)
            print(f"Updated song title: '{title}'")
            return response
        except Exception as e:
            print(f"Failed to update song title: {e}")

    def get_playing_media(self):
        if HAS_DBUS:
            try:
                bus = dbus.SessionBus()
                players = [s for s in bus.list_names() if s.startswith('org.mpris.MediaPlayer2.')]
                print(f"Found players: {players}")
                for player in players:
                    print(f"Checking player: {player}")
                    try:
                        proxy = bus.get_object(player, "/org/mpris/MediaPlayer2")
                        properties = dbus.Interface(proxy, "org.freedesktop.DBus.Properties")
                        playback_status = properties.Get("org.mpris.MediaPlayer2.Player", "PlaybackStatus")
                        print(f"Playback status for {player}: {playback_status}")
                        if playback_status != "Playing":
                            continue
                        metadata = properties.Get("org.mpris.MediaPlayer2.Player", "Metadata")
                        print(f"Metadata for {player}: {metadata}")
                        title = str(metadata.get("xesam:title", "Unknown Title"))
                        artist_value = metadata.get("xesam:artist", ["Unknown Artist"])
                        artist = str(artist_value[0]) if isinstance(artist_value, list) and artist_value else str(artist_value)
                        media_info = f"{title} - {artist}"
                        print(f"Found playing media: {media_info}")
                        return media_info
                    except Exception as e:
                        print(f"Error getting info from {player}: {e}")
                        continue
            except Exception as e:
                print(f"Error with dbus media detection: {e}")

        # Fallback to playerctl
        try:
            result = subprocess.run(['which', 'playerctl'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            if result.returncode == 0:
                status = subprocess.run(['playerctl', 'status'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                if status.returncode == 0 and status.stdout.strip() == "Playing":
                    title = subprocess.run(['playerctl', 'metadata', 'title'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    artist = subprocess.run(['playerctl', 'metadata', 'artist'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    if title.returncode == 0 and title.stdout.strip():
                        title_text = title.stdout.strip()
                        artist_text = artist.stdout.strip() if artist.returncode == 0 else "Unknown Artist"
                        media_info = f"{title_text} - {artist_text}"
                        print(f"Found playing media via playerctl: {media_info}")
                        return media_info
        except Exception as e:
            print(f"Error with playerctl fallback: {e}")
        print("No active media players found")
        return None

    def update_media_information(self):
        try:
            media_info = self.get_playing_media()
            if media_info != self.current_media:
                self.current_media = media_info
                self.update_song_title(media_info if media_info else "No media playing")
        except Exception as e:
            print(f"Error updating media information: {e}")

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
                current_time = time.time()
                if current_time - self.last_media_check >= self.MEDIA_CHECK_INTERVAL:
                    self.update_media_information()
                    self.last_media_check = current_time
                self.handle_reconnection()
                self.check_and_update_time()
                self.update_caps_lock_status()
                time.sleep(self.SLEEP_DURATION)
        except KeyboardInterrupt:
            print("Exiting driver loop...")
        finally:
            self.close()

if __name__ == "__main__":
    esp = ESP32Driver(port="/dev/ttyACM0")
    esp.run_forever()