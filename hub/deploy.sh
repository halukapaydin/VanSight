#!/bin/bash

# VanSightHub Deploy Script (USBtinyISP)
# This script uploads firmware using USBtinyISP programmer

set -e  # Exit on error

echo "======================================"
echo "VanSightHub Deploy Script (USBtinyISP)"
echo "======================================"

# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Cleaning .pio directory..."
if [ -d "$SCRIPT_DIR/.pio" ]; then
    rm -rf "$SCRIPT_DIR/.pio"
    echo "✓ .pio directory removed"
else
    echo "✓ .pio directory does not exist"
fi

echo ""
echo "Note: Uploading using USBtinyISP programmer."
echo "Ensure the programmer is connected to the ICSP header."

# USBtiny doesn't use a serial port for upload, so we don't need to find /dev/tty*
# However, we still might want to find a serial port for monitoring later

echo ""
echo "Uploading firmware..."
# We remove the --upload-port argument as USBtiny is found via USB PID/VID
pio run -d "$SCRIPT_DIR" -t upload -v

echo ""
echo "======================================"
echo "✓ Deploy completed successfully!"
echo "======================================"
echo ""
echo "To monitor serial output (if USB serial is connected), run: ./monitor.sh"
