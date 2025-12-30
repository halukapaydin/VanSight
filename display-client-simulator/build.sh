#!/bin/bash
# VanSight Display Simulator - Build Script

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
UI_SOURCE_DIR="$SCRIPT_DIR/../display-clint/lib/ui"
UI_TARGET_DIR="$SCRIPT_DIR/ui/src"

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

# Sync UI files from VanSightDisplayClient (unless --no-sync is passed)
if [ "$1" != "--no-sync" ] && [ "$2" != "--no-sync" ]; then
    if [ -d "$UI_SOURCE_DIR" ]; then
        echo "📋 Syncing UI files from VanSightDisplayClient..."
        
        # Create target directory if it doesn't exist
        mkdir -p "$UI_TARGET_DIR"
        
        # Copy UI C and H files (excluding subdirectories like src/)
        cp "$UI_SOURCE_DIR"/*.c "$UI_TARGET_DIR/" 2>/dev/null || true
        cp "$UI_SOURCE_DIR"/*.h "$UI_TARGET_DIR/" 2>/dev/null || true
        
        # Count copied files
        C_FILES=$(ls "$UI_SOURCE_DIR"/*.c 2>/dev/null | wc -l)
        H_FILES=$(ls "$UI_SOURCE_DIR"/*.h 2>/dev/null | wc -l)
        
        if [ $C_FILES -gt 0 ] || [ $H_FILES -gt 0 ]; then
            echo "✓ UI files synced ($C_FILES .c files, $H_FILES .h files)"
        else
            echo "⚠️  Warning: No UI files found to sync"
        fi
        echo ""
    else
        echo "⚠️  Warning: UI source directory not found: $UI_SOURCE_DIR"
        echo "   Continuing with existing UI files..."
        echo ""
    fi
else
    echo "⏭️  Skipping UI sync (--no-sync flag)"
    echo ""
fi

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
echo "UI files synced from: VanSightDisplayClient/lib/ui/src"
echo ""
echo "To run the simulator:"
echo "  ./run.sh"
echo ""
echo "Build options:"
echo "  ./build.sh clean      - Clean build from scratch"
echo "  ./build.sh --no-sync  - Build without syncing UI files"
echo ""
