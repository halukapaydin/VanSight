#!/bin/bash

# VanSightDisplayClient Deploy Script
# This script cleans, finds ttyACM device, stops any running monitor, and uploads firmware

set -e  # Exit on error

echo "======================================"
echo "VanSightDisplayClient Deploy Script"
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
echo "Finding ttyACM device..."
DEVICE=$(ls /dev/ttyACM* 2>/dev/null | head -n 1)

if [ -z "$DEVICE" ]; then
    echo "✗ Error: No ttyACM device found!"
    exit 1
fi

echo "✓ Found device: $DEVICE"

echo ""
echo "Checking for running serial monitors..."
# Kill any existing pio device monitor processes for this device
pkill -f "pio.*device monitor.*$DEVICE" 2>/dev/null && echo "✓ Stopped existing monitor" || echo "✓ No monitor running"

# Give it a moment to release the port
sleep 1

echo ""
echo "Uploading firmware to $DEVICE..."
pio run -d "$SCRIPT_DIR" -t upload --upload-port "$DEVICE"

echo ""
echo "======================================"
echo "✓ Deploy completed successfully!"
echo "======================================"
echo ""
echo "To monitor serial output, run: ./monitor.sh"
