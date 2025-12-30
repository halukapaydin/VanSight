#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ARDUINO MEGA RELAY PIN DEFINITIONS (16 channels)
// Using digital pins that are safe and easily accessible
// ============================================================================
const int RELAY_PINS[16] = {
    22, 23, 24, 25, 26,  // Relays 1-5
    27, 28, 29, 30, 31,  // Relays 6-10
    32, 33, 34, 35, 36, 37 // Relays 11-16
};

// ============================================================================
// LEVEL SENSOR PIN DEFINITIONS (Arduino Mega Analog Pins)
// ============================================================================
static const int SENSOR_PINS[3] = {
    A0,  // Sensor 1
    A1,  // Sensor 2
    A2   // Sensor 3
};

// ============================================================================
// SENSOR CALIBRATION CONSTANTS
// ============================================================================
// Sensor 1: 0-190Ω range
static const float SENSOR1_MIN_RESISTANCE = 0.0;
static const float SENSOR1_MAX_RESISTANCE = 190.0;

// Sensor 2: 0-190Ω range
static const float SENSOR2_MIN_RESISTANCE = 30.0;
static const float SENSOR2_MAX_RESISTANCE = 240.0;

// Sensor 3: 0-190Ω range
static const float SENSOR3_MIN_RESISTANCE = 30.0;
static const float SENSOR3_MAX_RESISTANCE = 270.0;

// Common Settings
static const float REFERENCE_RESISTOR = 1000.0; // Measured resistance
static const float SUPPLY_VOLTAGE = 5.0;        // Arduino Mega uses 5V logic
static const int ADC_RESOLUTION = 1024;         // Arduino Mega has 10-bit ADC (0-1023)


// ============================================================================
// PROTOCOL CONSTANTS
// ============================================================================
const int MAX_RELAYS = 16;
const int MAX_SENSORS = 3;

// Serial buffer size
const int SERIAL_BUFFER_SIZE = 256;

// ============================================================================
// BUZZER CONFIGURATION
// ============================================================================
const int BUZZER_PIN = 8;  // Digital pin for buzzer

#define I2C_HUB_ADDR 0x20
#define I2C_SERVER_ADDR 0x10

#endif // CONFIG_H
