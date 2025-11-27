#!/bin/bash

# VanSightHub Monitor Script
# This script finds ttyUSB device and opens serial monitor

set -e  # Exit on error

echo "======================================"
echo "VanSightHub Monitor Script"
echo "======================================"

# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Finding ttyUSB device..."
DEVICE=$(ls /dev/ttyUSB* 2>/dev/null | head -n 1)

if [ -z "$DEVICE" ]; then
    echo "✗ Error: No ttyUSB device found!"
    exit 1
fi

echo "✓ Found device: $DEVICE"
echo ""
echo "Opening serial monitor on $DEVICE..."
echo "Press Ctrl+C to exit"
echo "======================================"
echo ""

pio device monitor -d "$SCRIPT_DIR" --port "$DEVICE"
