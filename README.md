# Hyprland Gesture Conductor

> Control your Hyprland desktop with a wave of your hand.

A project that combines an Arduino UNO and a PAJ7620 gesture sensor to provide intuitive, touchless control over the Hyprland window manager and other Linux applications.

---

## 🌟 Features

The Conductor operates in three distinct modes, cycled by pressing a hardware button. Each mode maps 9 unique gestures to a specific set of actions.

| Gesture           | Navigation Mode        | Media Mode           | Reading Mode        |
| :---------------- | :--------------------- | :------------------- | :------------------ |
| 👋 **Left**       | Next Workspace         | Next Track           | Page Up             |
| 👋 **Right**      | Previous Workspace     | Previous Track       | Page Down           |
| 👋 **Up**         | Toggle Scratchpad      | Volume Up (10%)      | Scroll Down         |
| 👋 **Down**       | Send to Scratchpad     | Volume Down (10%)    | Scroll Up           |
| 👉 **Forward**    | Open Terminal\*        | Play/Pause           | Zoom Out            |
| 👈 **Backward**   | Lock Screen\*          | Restart Track        | Zoom In             |
| clockwise 🔄      | Cycle Window Focus     | Seek Forward (+10s)  | Go to End of Page   |
| anti-clockwise 🔄 | Cycle Focus (Prev)     | Seek Backward (-10s) | Go to Start of Page |
| 🌊 **Wave**       | Toggle Floating Window | Toggle Mute          | Toggle Fullscreen   |

_\*User-configurable in `script.py`. See the Customization section below._

## 🛠️ Architecture (Script-Driven Approach)

The current implementation uses a simple and effective distributed architecture where each component has a clearly defined role.

1.  **PAJ7620 Sensor**: Detects the raw hand gesture.
2.  **Arduino UNO (`along_script.ino`)**:
    - Reads the final gesture data from the sensor (e.g., `GES_FORWARD`).
    - Manages the current mode (Nav, Media, Reading).
    - Sends a formatted string over the USB serial port (e.g., `"NAV:FORWARD"`).
    - Provides feedback to the user via an LCD screen.
3.  **Python Script (`script.py`)**:
    - Runs continuously on the host computer, listening to the serial port.
    - Parses the incoming string to understand the mode and gesture.
    - Executes the corresponding shell command using `subprocess`.

![Architecture Diagram](https://i.imgur.com/your-diagram-placeholder.png)  
_(Placeholder for a future architecture diagram)_

## 🚀 Getting Started

Follow these steps to get the project up and running on your machine.

### 1. Prerequisites (Hardware)

- An Arduino UNO (or compatible board).
- A PAJ7620 Gesture Sensor module.
- An I2C 16x2 LCD Display (optional, but recommended for feedback).
- A momentary push button for mode switching.
- Jumper wires for connections.

### 2. Dependencies (Software)

The Python script relies on several command-line tools to control the desktop environment. A helper script has been provided to install these.

First, make the installation script executable:

```sh
chmod +x install_dependencies.sh
```

Then, run it with `sudo`:

```sh
sudo ./install_dependencies.sh
```

This will install the core dependencies required for the project: `python-pyserial`, `playerctl`, `wtype`, `rofi`, and `kitty`. The terminal and lock screen applications can be configured in `script.py`.

### 3. Installation & Running

1.  **Upload the Sketch**: Open `along_script/along_script.ino` in the Arduino IDE and upload it to your Arduino board.
2.  **Connect Hardware**: Connect the Arduino to your computer via USB.
3.  **Run the Listener**: Open a terminal and run the main Python script.

        ```sh
        python script.py
        ```

    You should see a "Connected! Listening for gestures..." message. The project is now active.

## 🎨 Customization

This project is designed to be easily adapted to your personal workflow and installed applications. You can change the commands that are executed by editing the `script.py` file.

### Example 1: Changing the Lock Screen

The default lock screen command is set to `["caelestia", "shell", "lock", "lock"]`. If you use a different lock screen, like `hyprlock`, you can change it as follows.

Find this line in `script.py`:

```python
            elif line == "NAV:BACKWARD":
                subprocess.run(["caelestia", "shell", "lock", "lock"])
```

And change it to use your preferred locker:

```python
            elif line == "NAV:BACKWARD":
                subprocess.run(["hyprlock"])
```

### Example 2: Changing the Terminal

Similarly, the default terminal is `kitty`. To change it to `wezterm`, for example, find this line:

```python
            elif line == "NAV:FORWARD":
                subprocess.Popen(["kitty"])
```

And change it to:

```python
            elif line == "NAV:FORWARD":
                subprocess.Popen(["wezterm"])
```

You can apply this principle to any of the commands in `script.py` to tailor the project to your exact needs.

## 🧠 Code Explained

### `along_script.ino` (The Sensor Hub)

This code is the brain of the hardware. Its key responsibilities are:

- **Initialization**: Sets up the gesture sensor, LCD, and serial communication.
- **Mode Switching**: Uses a hardware interrupt on a button press to cycle through `NAV`, `MEDIA`, and `READING` modes.
- **Gesture Polling**: In the main `loop()`, it continuously reads the sensor to clear its internal buffer.
- **Cooldown Logic**: A non-blocking `millis()` timer ensures that gestures are only processed after a `COOLDOWN_PERIOD` has passed. This prevents a single hand motion from triggering multiple unwanted gestures.
- **Serial Communication**: When a valid gesture is detected outside the cooldown period, `routeGesture()` is called, which sends the formatted string (e.g., `"MEDIA:UP"`) to the connected computer.

### `script.py` (The Command Executor)

This script acts as the bridge between the Arduino and the operating system.

- **Serial Connection**: It automatically detects and connects to the Arduino on the `/dev/ttyACM0` port.
- **Main Loop**: It runs a `while True:` loop to constantly listen for incoming lines from the serial port.
- **Command Parsing**: It uses a series of `if/elif` statements to match the received string (e.g., `"NAV:FORWARD"`) to a specific action.
- **Process Management**: It uses `subprocess.run()` for instantaneous commands (like `playerctl` or `hyprctl`) and `subprocess.Popen()` for long-running graphical applications (like `kitty`). This prevents the script from freezing while waiting for a GUI application to be closed.

---

## 🔌 The Plug-and-Play Vision (HID Approach)

While the script-driven approach is flexible, a more advanced version of this project would eliminate the Python script entirely by turning the Arduino into a native **Human Interface Device (HID)**, just like a real keyboard.

### How It Would Work

Instead of sending strings, the Arduino would send actual keyboard shortcuts. This requires either an Arduino with native USB capabilities (like a Leonardo or Pro Micro) or a dedicated USB HID chip (like CH9328).

The architecture would be simpler:

1.  **Arduino**: Reads gesture (`GES_FORWARD`).
2.  **Arduino**: Looks up the corresponding keyboard shortcut for the current mode (e.g., `SUPER + SHIFT + T`).
3.  **Arduino**: Sends the keystroke combination over USB.
4.  **Hyprland (`hyprland.conf`)**: Receives the keyboard shortcut and executes the desired command.

### Example: Replicating a Feature

Let's replicate the "Open Terminal" feature (`NAV:FORWARD`).

1.  **Arduino Code (HID Version):**

    ```cpp
    // In the NAV mode logic...
    if (gesture == GES_FORWARD) {
      // Send a complex, unused key combination
      Keyboard.press(KEY_LEFT_GUI);    // Super/Win key
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('T');
      delay(100);
      Keyboard.releaseAll();
    }
    ```

2.  **Hyprland Config (`~/.config/hypr/hyprland.conf`):**
    ```ini
    # Bind the shortcut from the Arduino to the desired action
    bind = SUPER SHIFT, T, exec, kitty
    ```

### Advantages of the HID Approach

- **Zero Software Setup**: Truly plug-and-play. It would work on any Hyprland (or other) machine without needing Python, Pyserial, or any scripts.
- **Lower Latency**: By removing the Python middleman, the time between gesture and action is reduced.
- **Increased Robustness**: The system is simpler with fewer points of failure.

## 📄 License

This project is open source. Feel free to use, modify, and distribute it as you see fit. It is recommended to license it under the **MIT License**.
