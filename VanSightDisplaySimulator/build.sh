#!/bin/bash
# VanSight Display Simulator - Build Script

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

echo "======================================"
echo "VanSight Display Simulator - Build"
echo "======================================"
echo ""

# Check if SDL2 is installed
if ! pkg-config --exists sdl2; then
    echo "❌ Error: SDL2 development libraries not found!"
    echo ""
    echo "Please install SDL2:"
    echo "  sudo apt-get install libsdl2-dev"
    echo ""
    exit 1
fi

echo "✓ SDL2 found: $(pkg-config --modversion sdl2)"
echo ""

# Clean build directory if requested
if [ "$1" == "clean" ]; then
    echo "🧹 Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "✓ Clean complete"
    echo ""
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "📁 Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Run CMake
echo "🔧 Running CMake..."
cd "$BUILD_DIR"
cmake .. || {
    echo ""
    echo "❌ CMake configuration failed!"
    exit 1
}

echo ""
echo "🔨 Building simulator..."
make -j$(nproc) || {
    echo ""
    echo "❌ Build failed!"
    exit 1
}

echo ""
echo "======================================"
echo "✅ Build successful!"
echo "======================================"
echo ""
echo "Executable: $BUILD_DIR/vansight_simulator"
echo "Size: $(du -h "$BUILD_DIR/vansight_simulator" | cut -f1)"
echo ""
echo "To run the simulator:"
echo "  ./run.sh"
echo "  or"
echo "  cd build && ./vansight_simulator"
echo ""
