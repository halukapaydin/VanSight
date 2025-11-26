/*
 * ESP-NOW Client Example (DisplayClient)
 * 
 * Demonstrates using ESPNowManager in CLIENT mode to send commands
 * and receive responses.
 */

#include <VanSightLib.h>

using namespace VanSight;

// Hub MAC address - UPDATE THIS WITH YOUR HUB'S MAC
const uint8_t HUB_MAC[] = {0x08, 0xB6, 0x1F, 0xBE, 0x12, 0x24};

// Create ESP-NOW manager in CLIENT mode
ESPNowManager espnow(ESPNowRole::CLIENT, HUB_MAC);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== ESP-NOW Client (DisplayClient) ===\n");
    
    // Initialize ESP-NOW
    if (!espnow.begin()) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }
    
    // Register response callback
    espnow.onResponseReceived([](const Response& response) {
        if (response.status == STATUS_OK) {
            Serial.println("Response received: OK");
            // Handle response data
        } else {
            Serial.println("Response received: ERROR");
        }
    });
    
    Serial.println("Client ready!\n");
}

void loop() {
    static unsigned long lastSend = 0;
    unsigned long now = millis();
    
    // Send command every 5 seconds
    if (now - lastSend >= 5000) {
        lastSend = now;
        
        // Create and send relay toggle command
        Command cmd;
        cmd.type = CMD_RELAY_TOGGLE;
        cmd.params.relay.relayNum = 1;
        
        Serial.println("Sending relay toggle command...");
        if (espnow.sendCommand(cmd)) {
            Serial.println("Command sent successfully");
        } else {
            Serial.println("Failed to send command");
        }
    }
    
    delay(100);
}
