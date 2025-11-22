#!/bin/bash
# VanSight Display Simulator - Run Script

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
EXECUTABLE="$BUILD_DIR/vansight_simulator"

echo "======================================"
echo "VanSight Display Simulator"
echo "======================================"
echo ""

# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo "❌ Error: Simulator not built yet!"
    echo ""
    echo "Please build first:"
    echo "  ./build.sh"
    echo ""
    exit 1
fi

# Check if DISPLAY is set (for X11)
if [ -z "$DISPLAY" ]; then
    echo "⚠️  Warning: DISPLAY environment variable not set"
    echo "   SDL2 may not be able to open a window"
    echo ""
fi

# Get resolution from lv_drv_conf.h
HOR_RES=$(grep "SDL_HOR_RES" "$SCRIPT_DIR/lv_drv_conf.h" | grep -v "//" | awk '{print $3}')
VER_RES=$(grep "SDL_VER_RES" "$SCRIPT_DIR/lv_drv_conf.h" | grep -v "//" | awk '{print $3}')
ZOOM=$(grep "SDL_ZOOM" "$SCRIPT_DIR/lv_drv_conf.h" | grep -v "//" | awk '{print $3}')

echo "Configuration:"
echo "  Resolution: ${HOR_RES}x${VER_RES}"
echo "  Zoom: ${ZOOM}x"
echo "  Window size: $((HOR_RES * ZOOM))x$((VER_RES * ZOOM))"
echo ""
echo "Controls:"
echo "  Mouse: Simulates touchscreen"
echo "  Close window or Ctrl+C to exit"
echo ""
echo "Starting simulator..."
echo "======================================"
echo ""

# Run the simulator
cd "$BUILD_DIR"
exec ./vansight_simulator
