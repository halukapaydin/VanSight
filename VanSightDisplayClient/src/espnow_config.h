#ifndef ESPNOW_CONFIG_H
#define ESPNOW_CONFIG_H

#include <Arduino.h>
#include <lvgl.h>

// ============================================================================
// VanSightHub MAC Address
// ============================================================================
// TODO: Update this with your VanSightHub MAC address
// You can find it in VanSightHub serial output on startup
const uint8_t HUB_MAC_ADDRESS[6] = {0x08, 0xB6, 0x1F, 0xBE, 0x12, 0x24};  // Hub AP MAC Address  // WiFi AP MAC
// const uint8_t HUB_MAC_ADDRESS[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Hub AP MAC Address  // WiFi AP MAC

// ============================================================================
// Relay to Button Mapping
// ============================================================================
// Maps button index to relay number (1-16)
enum ButtonIndex {
    BTN_HOME = 0,
    BTN_LEFT_CHAIR,
    BTN_RIGHT_CHAIR,
    BTN_KITCHEN,
    BTN_TOILET,
    BTN_BATH,
    BTN_BED,
    BTN_BALKONY_UP,
    BTN_BALKONY_DOWN,
    BTN_PROJECTOR,
    BTN_RELOAD_INFO,
    BTN_CLOSE_ALL,
    BTN_COUNT
};

const uint8_t BUTTON_TO_RELAY_MAP[BTN_COUNT] = {
    1,   // BTN_HOME → Relay 1
    2,   // BTN_LEFT_CHAIR → Relay 2
    3,   // BTN_RIGHT_CHAIR → Relay 3
    4,   // BTN_KITCHEN → Relay 4
    5,   // BTN_TOILET → Relay 5
    6,   // BTN_BATH → Relay 6
    7,   // BTN_BED → Relay 7
    8,   // BTN_BALKONY_UP → Relay 8
    9,   // BTN_BALKONY_DOWN → Relay 9
    10,  // BTN_PROJECTOR → Relay 10
    0,   // BTN_RELOAD_INFO → Special (request status)
    0    // BTN_CLOSE_ALL → Special (all OFF)
};

// ============================================================================
// Sensor to Bar Mapping
// ============================================================================
enum SensorIndex {
    SENSOR_CLEAN_WATER = 1,
    SENSOR_GRAY_WATER = 2,
    SENSOR_BLACK_WATER = 3
};

// ============================================================================
// UI Colors
// ============================================================================
const lv_color_t COLOR_BUTTON_ACTIVE = lv_color_hex(0xFFD700);   // Gold/Yellow
const lv_color_t COLOR_BUTTON_INACTIVE = lv_color_hex(0x2E2E2E); // Dark gray
const lv_color_t COLOR_CONNECTION_LOST = lv_color_hex(0x8073E6); // Dark gray
const lv_color_t COLOR_CONNECTION_ACTIVE = lv_color_hex(0xFFD700); // Dark gray

// ============================================================================
// ESP-NOW Configuration
// ============================================================================
const int MAX_ESPNOW_PAYLOAD = 250;
const int ESPNOW_TIMEOUT_MS = 2000;
const int STATUS_UPDATE_INTERVAL_MS = 5000;  // Request status every 5 seconds

#endif // ESPNOW_CONFIG_H
