#include <Arduino.h>
#include <VanSightLib.h>
#include "RelayController.h"
#include "SensorController.h"
#include "CommandHandler.h"
#include "BuzzerManager.h"
#include "config.h"

using namespace VanSight;
RelayController relayController(RELAY_PINS, 16);
SensorController sensorController(3);
CommandHandler commandHandler(relayController, sensorController);

void initSensors();

AllStatusData readAllStatus();

void setup()
{
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== VanSightHub - ESP-NOW Server ===\n");
    
    // Initialize Buzzer
    BuzzerManager::getInstance().begin(4); // Pin 4
    BuzzerManager::getInstance().beep(100); // Startup beep
    
    // Initialize relays
    Serial.println("Initializing relays...");
    relayController.begin();
    Serial.printf("✓ %d relays initialized\n\n", relayController.getCount());
    
    // Initialize sensors
    initSensors();
    
    // Initialize CommandManager as Server (BLE)
    if (!BleCommandManager::getInstance().beginServer("VanSightHub")) {
        Serial.println("BleCommandManager initialization failed!");
        return;
    }
    
    // Register relay toggle handler
    BleCommandManager::getInstance().onToggleRelay([](uint8_t relayNum) -> bool {
        Serial.printf("Toggle relay %d\n", relayNum);
        
        // Beep on command received
        BuzzerManager::getInstance().beep(50);
        
        // Toggle relay
        relayController.toggle(relayNum);
        bool newState = relayController.getState(relayNum);
        
        // Broadcast to all clients
        BleCommandManager::getInstance().sendRelayState(relayNum, newState);
        
        return newState;
    });
    
    // Register all relays off handler
    BleCommandManager::getInstance().onAllRelaysOff([]() {
        Serial.println("All relays OFF");
        
        // Beep on command received
        BuzzerManager::getInstance().beepPattern(2, 50, 100);
        
        // Turn off all relays
        for (int i = 1; i <= relayController.getCount(); i++) {
            relayController.turnOff(i);
        }
        auto status = readAllStatus();
        BleCommandManager::getInstance().sendAllStatus(status.relayStates, status.sensorLevels);
    });
    
    // Register status request handler
    BleCommandManager::getInstance().onStatusRequest([]() -> AllStatusData {
        Serial.println("Status requested");
        
        // Beep on command received
        BuzzerManager::getInstance().beep(30);
        
        return readAllStatus();
    });
    
    Serial.println("\\n=== System Ready ===");
    Serial.printf("Waiting for clients (max %d)...\\n\\n", ESP_NOW_MAX_TOTAL_PEER_NUM);
}

AllStatusData readAllStatus()
{
    AllStatusData data;

    // Get relay states
    for (int i = 0; i < VanSight::MAX_RELAYS; i++) {
        data.relayStates[i] = relayController.getState(i + 1);
    }

    // Get sensor levels
    for (int i = 0; i < VanSight::MAX_SENSORS; i++) {
        data.sensorLevels[i] = sensorController.readLevel(i);
    }

    return data;
}


void loop()
{
    static unsigned long lastSensorCheck = 0;
    static int lastSensorLevels[VanSight::MAX_SENSORS] = {-1, -1, -1}; // Initialize with invalid values
    unsigned long now = millis();

    // Check sensors every 5 seconds
    if (now - lastSensorCheck >= 5000) {
        lastSensorCheck = now;
        
        if (BleCommandManager::getInstance().isConnected()) {
            bool sensorChanged = false;
            int currentSensorLevels[VanSight::MAX_SENSORS];
            
            // Read current sensor levels
            for (int i = 0; i < VanSight::MAX_SENSORS; i++) {
                currentSensorLevels[i] = sensorController.readLevel(i);
                
                // Check if sensor value changed
                if (currentSensorLevels[i] != lastSensorLevels[i]) {
                    sensorChanged = true;
                    Serial.printf("[Sensor] Sensor %d changed: %d -> %d\n", 
                                  i, lastSensorLevels[i], currentSensorLevels[i]);
                }
            }
            
            // If any sensor changed, send update to clients
            if (sensorChanged) {
                Serial.println("[Sensor] Sending sensor update to clients...");
                
                // Get current relay states
                bool relayStates[VanSight::MAX_RELAYS];
                for (int i = 0; i < VanSight::MAX_RELAYS; i++) {
                    relayStates[i] = relayController.getState(i + 1);
                }
                
                // Send all status with updated sensor levels
                BleCommandManager::getInstance().sendAllStatus(relayStates, currentSensorLevels);
                
                // Update last known sensor levels
                for (int i = 0; i < VanSight::MAX_SENSORS; i++) {
                    lastSensorLevels[i] = currentSensorLevels[i];
                }
            }
        }
    }
    
    delay(100);
}

void initSensors()
{
    Serial.println("Initializing sensors...");
    
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    
    sensorController.addSensor(0, SENSOR_PINS[0], SENSOR1_MIN_RESISTANCE, SENSOR1_MAX_RESISTANCE, REFERENCE_RESISTOR);
    sensorController.addSensor(1, SENSOR_PINS[1], SENSOR23_MIN_RESISTANCE, SENSOR23_MAX_RESISTANCE, REFERENCE_RESISTOR);
    sensorController.addSensor(2, SENSOR_PINS[2], SENSOR23_MIN_RESISTANCE, SENSOR23_MAX_RESISTANCE, REFERENCE_RESISTOR);
    
    sensorController.begin();
    
    Serial.printf("✓ %d level sensors initialized\\n\\n", sensorController.getCount());
}