#!/bin/bash

# ==============================================================================
# Dependency Installation Script for Hyprland Gesture Conductor
# ==============================================================================
# This script installs the necessary dependencies for the project.
# It defaults to using pacman (for Arch Linux and derivatives like EndeavourOS).
#
# If you are on a Debian-based distribution (like Ubuntu), please comment out
# the pacman command and uncomment the apt commands.

# --- Check for root privileges ---
if [ "$EUID" -ne 0 ]; then
  echo "Please run this script with sudo: sudo ./install_dependencies.sh"
  exit 1
fi


# --- ARCH LINUX (pacman) ---
# ---------------------------
echo "INFO: Installing dependencies for Arch Linux..."
# Note: The terminal (kitty) and lock screen command can be changed in `script.py`.
pacman -Syu --noconfirm python-pyserial playerctl wtype rofi kitty


echo "=================================================="
echo "✅ Dependency installation complete."
echo "--------------------------------------------------"
echo "Next Steps:"
echo "1. Upload 'along_script.ino' to your Arduino."
echo "2. Connect the Arduino to your computer."
echo "3. Run the main listener: python script.py"
echo "=================================================="
