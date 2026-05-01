import serial
import subprocess
import time

# Arduino 
PORT = '/dev/ttyACM0' 
BAUD = 9600

def connect_serial():
    print(f"Attempting to connect to {PORT}...")
    try:
        return serial.Serial(PORT, BAUD, timeout=1)
    except serial.SerialException:
        print("Waiting for Arduino...")
        time.sleep(2)
        return None


ser = None
while ser is None:
    ser = connect_serial()

print("Connected! Listening for gestures...")

try:
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            print(f"Received: {line}")

            # --- NAVIGATION MODE ---
            if line == "NAV:LEFT":
                subprocess.run(["hyprctl", "dispatch", "workspace", "m+1"])
            elif line == "NAV:RIGHT":
                subprocess.run(["hyprctl", "dispatch", "workspace", "m-1"])
            elif line == "NAV:UP":
                subprocess.run(["hyprctl", "dispatch", "togglespecialworkspace"])
            elif line == "NAV:DOWN":
                subprocess.run(["hyprctl", "dispatch", "movetoworkspace", "special"])
            elif line == "NAV:FORWARD":
                subprocess.Popen(["kitty"])
            elif line == "NAV:BACKWARD":
                subprocess.run(["caelestia", "shell", "lock", "lock"])
            elif line == "NAV:CLOCKWISE":
                subprocess.run(["hyprctl", "dispatch", "cyclenext"])
            elif line == "NAV:ANTI-CW":
                subprocess.run(["hyprctl", "dispatch", "cyclenext", "prev"])
            elif line == "NAV:WAVE":
                subprocess.run(["hyprctl", "dispatch", "togglefloating"])
            
            # --- MEDIA MODE ---
            # A media player (Spotify, YouTube, VLC) must be open for these to work!
            elif line == "MEDIA:LEFT":
                subprocess.run(["playerctl", "next"])
            elif line == "MEDIA:RIGHT":
                subprocess.run(["playerctl", "previous"])
            elif line == "MEDIA:UP":
                subprocess.run(["wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "10%+"])
            elif line == "MEDIA:DOWN":
                subprocess.run(["wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "10%-"])
            elif line == "MEDIA:FORWARD":
                subprocess.run(["playerctl", "play-pause"])
            elif line == "MEDIA:BACKWARD":
                subprocess.run(["playerctl", "position", "0"])
            elif line == "MEDIA:CLOCKWISE":
                subprocess.run(["playerctl", "position", "10+"])
            elif line == "MEDIA:ANTI-CW":
                subprocess.run(["playerctl", "position", "10-"])
            elif line == "MEDIA:WAVE":
                subprocess.run(["wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle"])

            # --- READING MODE ---
            elif line == "READING:LEFT":
                subprocess.run(["wtype", "-k", "Page_Up"])
            elif line == "READING:RIGHT":
                subprocess.run(["wtype", "-k", "Page_Down"])
            elif line == "READING:UP":
                subprocess.run(["wtype", "-k", "down"])
            elif line == "READING:DOWN":
                subprocess.run(["wtype", "-k", "up"])
            elif line == "READING:FORWARD":
                subprocess.run(["wtype", "-M", "ctrl", "-k", "minus"])
            elif line == "READING:BACKWARD":
                subprocess.run(["wtype", "-M", "ctrl", "-k", "plus"])
            elif line == "READING:CLOCKWISE":
                subprocess.run(["wtype", "-k", "End"])
            elif line == "READING:ANTI-CW":
                subprocess.run(["wtype", "-k", "Home"])
            elif line == "READING:WAVE":
                subprocess.run(["wtype", "-k", "F11"])

except KeyboardInterrupt:
    print("\nShutting down listener.")
finally:
    if ser:
        ser.close()
