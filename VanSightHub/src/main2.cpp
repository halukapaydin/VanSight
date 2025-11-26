//
//
// #include <Arduino.h>
// #include <WiFi.h>
// #include <ArduinoJson.h>
//
// #include "config.h"
// #include "RelayController.h"
// #include "SensorController.h"
// #include "CommandHandler.h"
// #include "BLEManager.h"
//
// RelayController relayController(RELAY_PINS, 16);
// SensorController sensorController(3);
//
// CommandHandler commandHandler(relayController, sensorController);
//
// // ============================================================================
// // FUNCTION DECLARATIONS
// // ============================================================================
// void initSensors();
//
// // ============================================================================
// // SETUP
// // ============================================================================
// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//
//   Serial.println("\n=================================");
//   Serial.println("VanSightHub - Dual Protocol");
//   Serial.println("VanSightHub - BLE Protocol");
//   Serial.println("=================================\n");
//
//   // Initialize relays
//   Serial.println("Initializing relays...");
//   relayController.begin();
//   Serial.printf("✓ %d relays initialized\n\n", relayController.getCount());
//
//   // Initialize sensors
//   // Initialize sensors
//   initSensors();
//
//
//   // Initialize BLE
//   bleManager.init();
//
//   Serial.println("\n=================================");
//   Serial.println("System ready!");
//   Serial.println("=================================");
//   Serial.println("ESP-NOW: Listening for commands");
//   Serial.println("BLE: Waiting for client connection");
//   Serial.println("=================================\n");
// }
//
// // ============================================================================
// // MAIN LOOP
// // ============================================================================
// void loop() {
//   // Update BLE connection state
//   bleManager.update();
//
//   delay(10);
// }
//
// // ============================================================================
// // SENSOR INITIALIZATION
// // ============================================================================
// void initSensors() {
//   Serial.println("Initializing sensors...");
//
//   analogReadResolution(12);
//   analogSetAttenuation(ADC_11db);
//
//   sensorController.addSensor(0, SENSOR_PINS[0], SENSOR1_MIN_RESISTANCE, SENSOR1_MAX_RESISTANCE, REFERENCE_RESISTOR);
//   sensorController.addSensor(1, SENSOR_PINS[1], SENSOR23_MIN_RESISTANCE, SENSOR23_MAX_RESISTANCE, REFERENCE_RESISTOR);
//   sensorController.addSensor(2, SENSOR_PINS[2], SENSOR23_MIN_RESISTANCE, SENSOR23_MAX_RESISTANCE, REFERENCE_RESISTOR);
//
//   sensorController.begin();
//
//   Serial.printf("✓ %d level sensors initialized\n\n", sensorController.getCount());
// }
//
//
//
