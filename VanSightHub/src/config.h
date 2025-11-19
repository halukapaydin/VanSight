#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// RELAY PIN DEFINITIONS (10 channels)
// ============================================================================
const int RELAY_PINS[16] = {
    32, 33, 23, 22, 21,  // Relays 1-5
    19, 18, 5, 17, 16 // Relays 6-10
};

// ============================================================================
// LEVEL SENSOR PIN DEFINITIONS (ADC inputs)
// ============================================================================
const int SENSOR_PINS[3] = {
    34,  // Sensor 1: 0-190Ω (ADC1_CH6)
    35,  // Sensor 2: 30-240Ω (ADC1_CH7)
    36   // Sensor 3: 30-240Ω (ADC1_CH0)
};

// ============================================================================
// SENSOR CALIBRATION CONSTANTS
// ============================================================================
// Sensor 1: 0-190Ω range
const float SENSOR1_MIN_RESISTANCE = 0.0;
const float SENSOR1_MAX_RESISTANCE = 10000.0;

// Sensors 2 & 3: 30-240Ω range
const float SENSOR23_MIN_RESISTANCE = 0.0;
const float SENSOR23_MAX_RESISTANCE = 10000.0;

// Sensor 1: 0-190Ω range
//const float SENSOR1_MIN_RESISTANCE = 0.0;
//const float SENSOR1_MAX_RESISTANCE = 190.0;

// Sensors 2 & 3: 30-240Ω range
//const float SENSOR23_MIN_RESISTANCE = 30.0;
//const float SENSOR23_MAX_RESISTANCE = 240.0;



// Voltage divider reference resistor (adjust based on your circuit)
const float REFERENCE_RESISTOR = 1000.0;  // 1kΩ reference resistor
const float SUPPLY_VOLTAGE = 3.3;         // ESP32 ADC reference voltage

// ADC resolution
const int ADC_RESOLUTION = 4095;  // 12-bit ADC

// ============================================================================
// ESP-NOW CONFIGURATION
// ============================================================================
// Maximum payload size for ESP-NOW
const int MAX_ESPNOW_PAYLOAD = 250;

// Broadcast address (for receiving from any device)
// Set to your specific sender MAC address if needed
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ============================================================================
// WiFi CONFIGURATION
// ============================================================================
// WiFi credentials (change these to your network)
const char* WIFI_SSID = "VanSightHub";      // AP mode SSID
const char* WIFI_PASSWORD = "12345678";      // AP mode password (min 8 chars)

// Set to true for AP mode, false for STA mode
const bool WIFI_AP_MODE = true;

// If STA mode, set your router credentials here
const char* WIFI_STA_SSID = "YourRouterSSID";
const char* WIFI_STA_PASSWORD = "YourRouterPassword";

#endif // CONFIG_H
