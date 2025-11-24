cmake_minimum_required(VERSION 3.10)
project(VanSightSimulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# LVGL configuration
add_definitions(-DLV_CONF_INCLUDE_SIMPLE)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/lv_drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/src
)

# Collect LVGL source files
file(GLOB_RECURSE LVGL_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/*.c
)

# Collect lv_drivers source files (SDL only)
file(GLOB LV_DRIVERS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lv_drivers/sdl/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lv_drivers/*.c
)

# Collect UI source files
file(GLOB_RECURSE UI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/src/*.c
)

# Main executable
add_executable(vansight_simulator
    main.c
    ui_event_handlers.c
    ${LVGL_SOURCES}
    ${LV_DRIVERS_SOURCES}
    ${UI_SOURCES}
)

# Link SDL2
target_link_libraries(vansight_simulator ${SDL2_LIBRARIES} m pthread)

# Compiler flags
target_compile_options(vansight_simulator PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -O2
)
