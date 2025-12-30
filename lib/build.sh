#!/bin/bash

# VanSightLib Build Script
# This script cleans and builds the VanSightLib library

set -e  # Exit on error

echo "======================================"
echo "VanSightLib Build Script"
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
echo "Building VanSightLib..."
pio run -d "$SCRIPT_DIR"

echo ""
echo "======================================"
echo "✓ Build completed successfully!"
echo "======================================"
