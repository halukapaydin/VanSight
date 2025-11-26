/*
 * CommandManager Example - Hub (Server)
 * 
 * Demonstrates using CommandManager with simple high-level API
 */

#include <VanSightLib.h>

using namespace VanSight;

// Simulated relay states
bool relayStates[MAX_RELAYS] = {false};

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== CommandManager Server Example ===\n");
    
    // Initialize as Server
    if (!CommandManager::getInstance().beginServer()) {
        Serial.println("Failed to initialize CommandManager");
        return;
    }
    
    // Register relay toggle handler
    CommandManager::getInstance().onToggleRelay([](uint8_t relayNum) -> bool {
        Serial.printf("Toggling relay %d\n", relayNum);
        
        if (relayNum >= 1 && relayNum <= MAX_RELAYS) {
            relayStates[relayNum - 1] = !relayStates[relayNum - 1];
            return relayStates[relayNum - 1];
        }
        return false;
    });
    
    // Register all relays off handler
    CommandManager::getInstance().onAllRelaysOff([]() {
        Serial.println("Turning off all relays");
        for (int i = 0; i < MAX_RELAYS; i++) {
            relayStates[i] = false;
        }
    });
    
    // Register status request handler
    CommandManager::getInstance().onStatusRequest([]() -> AllStatusData {
        Serial.println("Status requested");
        
        AllStatusData data;
        memcpy(data.relayStates, relayStates, sizeof(relayStates));
        
        // Simulated sensor data
        for (int i = 0; i < MAX_SENSORS; i++) {
            data.sensorLevels[i] = 50 + (i * 10);
        }
        
        return data;
    });
    
    Serial.println("Server ready!\n");
}

void loop() {
    delay(100);
}
