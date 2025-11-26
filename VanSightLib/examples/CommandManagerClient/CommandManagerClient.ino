/*
 * CommandManager Example - Client (DisplayClient)
 * 
 * Demonstrates using CommandManager with simple high-level API
 */

#include <VanSightLib.h>

using namespace VanSight;

// Hub MAC address - UPDATE THIS
const uint8_t HUB_MAC[] = {0x08, 0xB6, 0x1F, 0xBE, 0x12, 0x24};

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== CommandManager Client Example ===\n");
    
    // Initialize as Client
    if (!CommandManager::getInstance().beginClient(HUB_MAC)) {
        Serial.println("Failed to initialize CommandManager");
        return;
    }
    
    // Register data received callback
    CommandManager::getInstance().onDataReceived([](const AllStatusData& data) {
        Serial.println("All status received:");
        
        Serial.print("Relays: ");
        for (int i = 0; i < MAX_RELAYS; i++) {
            Serial.printf("%d:%s ", i+1, data.relayStates[i] ? "ON" : "OFF");
        }
        Serial.println();
        
        Serial.print("Sensors: ");
        for (int i = 0; i < MAX_SENSORS; i++) {
            Serial.printf("%d:%d%% ", i+1, data.sensorLevels[i]);
        }
        Serial.println("\n");
    });
    
    // Register relay changed callback
    CommandManager::getInstance().onRelayChanged([](uint8_t relayNum, bool state) {
        Serial.printf("Relay %d changed to %s\n", relayNum, state ? "ON" : "OFF");
    });
    
    Serial.println("Client ready!\n");
    
    // Request initial status
    delay(1000);
    CommandManager::getInstance().requestStatus();
}

void loop() {
    static unsigned long lastToggle = 0;
    unsigned long now = millis();
    
    // Toggle relay 1 every 5 seconds
    if (now - lastToggle >= 5000) {
        lastToggle = now;
        
        Serial.println("Toggling relay 1...");
        CommandManager::getInstance().toggleRelay(1);
    }
    
    delay(100);
}
