import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import gi
gi.require_version('GLib', '2.0')
from gi.repository import GLib
import threading

class MediaPlayerSignalHandler:
    """
    A class to handle DBUS signal events from media players
    """
    def __init__(self, media_callback):
        """
        Initialize the signal handler with a callback function
        
        Args:
            media_callback: A function that will be called when media status changes
                            The callback should accept a string parameter with media info
        """
        self.media_callback = media_callback
        self.mainloop = None
        self.loop_thread = None
        self.active_players = {}
        self.should_run = True

    def start(self):
        """Start the event loop in a separate thread"""
        # Set up the DBUS main loop for signal handling
        DBusGMainLoop(set_as_default=True)
        self.mainloop = GLib.MainLoop()
        
        # Create a separate thread for the main loop
        self.loop_thread = threading.Thread(target=self._run_loop, daemon=True)
        self.loop_thread.start()
        
        # Register for player signals on the bus
        self._register_signal_handlers()
        
        print("Media signal handler started")
        return True

    def _run_loop(self):
        """Run the GLib main loop in a thread"""
        while self.should_run:
            try:
                self.mainloop.run()
            except Exception as e:
                print(f"Error in media signal loop: {e}")
                # Brief pause to avoid consuming CPU in case of errors
                import time
                time.sleep(0.5)

    def stop(self):
        """Stop the signal handler and its thread"""
        self.should_run = False
        if self.mainloop and self.mainloop.is_running():
            self.mainloop.quit()
        if self.loop_thread and self.loop_thread.is_alive():
            self.loop_thread.join(timeout=1.0)

    def _register_signal_handlers(self):
        """Register for DBUS signals from media players"""
        try:
            bus = dbus.SessionBus()
            
            # Listen for new services appearing on the bus
            bus.add_signal_receiver(
                self._name_owner_changed,
                signal_name='NameOwnerChanged',
                dbus_interface='org.freedesktop.DBus'
            )
            
            # Register with already running players
            self._find_and_register_players()
            
        except Exception as e:
            print(f"Error setting up DBUS signal handlers: {e}")
            return False
        
        return True

    def _find_and_register_players(self):
        """Find existing media players and register for their signals"""
        try:
            bus = dbus.SessionBus()
            player_names = [s for s in bus.list_names() if s.startswith('org.mpris.MediaPlayer2.')]
            
            for player_name in player_names:
                self._register_player(player_name)
                
        except Exception as e:
            print(f"Error finding media players: {e}")

    def _name_owner_changed(self, name, old_owner, new_owner):
        """Handle services appearing/disappearing from the bus"""
        if name.startswith('org.mpris.MediaPlayer2.'):
            if new_owner and not old_owner:
                # New media player appeared
                print(f"Media player appeared: {name}")
                self._register_player(name)
            elif old_owner and not new_owner:
                # Media player disappeared
                print(f"Media player disappeared: {name}")
                if name in self.active_players:
                    del self.active_players[name]
                # If the current player disappeared, send an update
                self._check_current_media()

    def _register_player(self, player_name):
        """Register for signals from a specific media player"""
        try:
            bus = dbus.SessionBus()
            player_obj = bus.get_object(player_name, '/org/mpris/MediaPlayer2')
            
            # Register for PropertiesChanged signal to catch media updates
            player_obj.connect_to_signal(
                'PropertiesChanged',
                lambda interface, changed_props, invalidated_props: 
                    self._on_properties_changed(player_name, interface, changed_props),
                dbus_interface='org.freedesktop.DBus.Properties'
            )
            
            # Remember this player
            self.active_players[player_name] = player_obj
            
            # Check initial state
            self._check_player_media(player_name)
            
        except Exception as e:
            print(f"Error registering for {player_name} signals: {e}")

    def _on_properties_changed(self, player_name, interface, changed_props):
        """Handle property changes from media players"""
        if interface != 'org.mpris.MediaPlayer2.Player':
            return
            
        if 'PlaybackStatus' in changed_props:
            status = str(changed_props['PlaybackStatus'])
            print(f"Player {player_name} status changed to: {status}")
            # If playback started or stopped, check media
            self._check_current_media()
        
        if 'Metadata' in changed_props:
            # Media metadata changed, check and notify
            print(f"Player {player_name} metadata changed")
            self._check_current_media()

    def _check_current_media(self):
        """Check all players for currently playing media"""
        try:
            for player_name in list(self.active_players.keys()):
                media_info = self._check_player_media(player_name)
                if media_info:
                    # Found playing media, notify via callback
                    self.media_callback(media_info)
                    return
                    
            # No media playing found, notify with empty string
            self.media_callback(None)
            
        except Exception as e:
            print(f"Error checking current media: {e}")

    def _check_player_media(self, player_name):
        """Check if a specific player is playing and get its media info"""
        try:
            if player_name not in self.active_players:
                return None
                
            player_obj = self.active_players[player_name]
            properties = dbus.Interface(player_obj, "org.freedesktop.DBus.Properties")
            
            # Check if player is playing
            status = properties.Get("org.mpris.MediaPlayer2.Player", "PlaybackStatus")
            if status != "Playing":
                return None
                
            # Get metadata for playing media
            metadata = properties.Get("org.mpris.MediaPlayer2.Player", "Metadata")
            
            # Extract title and artist
            title = str(metadata.get("xesam:title", "Unknown Title"))
            artist_value = metadata.get("xesam:artist", ["Unknown Artist"])
            artist = str(artist_value[0]) if isinstance(artist_value, list) and artist_value else str(artist_value)
            
            # Format media info string
            media_info = f"{title} - {artist}"
            print(f"Found playing media: {media_info}")
            return media_info
            
        except Exception as e:
            print(f"Error checking player {player_name}: {e}")
            return None
