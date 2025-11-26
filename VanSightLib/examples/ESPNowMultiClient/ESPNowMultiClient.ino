/*
 * ESP-NOW Multi-Client Broadcast Example
 * 
 * Demonstrates using ESPNowManager in SERVER mode with multiple clients.
 * Shows how to broadcast responses to all connected clients.
 */

#include <VanSightLib.h>

using namespace VanSight;

// Create ESP-NOW manager in SERVER mode
ESPNowManager espnow(ESPNowRole::SERVER);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== ESP-NOW Multi-Client Server ===\n");
    
    // Initialize ESP-NOW
    if (!espnow.begin()) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }
    
    // Register command callback
    espnow.onCommandReceived([](const Command& cmd, const uint8_t* senderMac) {
        Serial.printf("Command from %02X:%02X:%02X:%02X:%02X:%02X\n",
                      senderMac[0], senderMac[1], senderMac[2], 
                      senderMac[3], senderMac[4], senderMac[5]);
        
        Response response;
        response.status = STATUS_OK;
        
        switch (cmd.type) {
            case CMD_RELAY_TOGGLE:
                Serial.printf("Toggle relay %d\n", cmd.params.relay.relayNum);
                response.data.relay.relayNum = cmd.params.relay.relayNum;
                response.data.relay.state = true;
                
                // BROADCAST to all clients (DisplayClient, ButtonClient, etc.)
                int sent = espnow.broadcastResponse(response);
                Serial.printf("Broadcasted to %d clients\n", sent);
                break;
                
            case CMD_ALL_STATUS:
                Serial.println("Status request - broadcasting to all");
                
                // Fill status data
                for (int i = 0; i < MAX_RELAYS; i++) {
                    response.data.allStatus.relayStates[i] = (i % 2 == 0);
                }
                for (int i = 0; i < MAX_SENSORS; i++) {
                    response.data.allStatus.sensorLevels[i] = 50 + (i * 10);
                }
                
                // Broadcast to all clients
                espnow.broadcastResponse(response);
                break;
                
            default:
                // For other commands, just reply to sender
                espnow.sendResponse(response, senderMac);
                break;
        }
    });
    
    Serial.println("Server ready for multiple clients...");
    Serial.printf("Max clients: %d\n\n", ESP_NOW_MAX_TOTAL_PEER_NUM);
}

void loop() {
    static unsigned long lastStatus = 0;
    unsigned long now = millis();
    
    // Broadcast status update every 30 seconds
    if (now - lastStatus >= 30000) {
        lastStatus = now;
        
        if (espnow.getPeerCount() > 0) {
            Serial.println("Broadcasting periodic status update...");
            
            Response response;
            response.status = STATUS_OK;
            
            // Fill with current status
            for (int i = 0; i < MAX_RELAYS; i++) {
                response.data.allStatus.relayStates[i] = false;
            }
            for (int i = 0; i < MAX_SENSORS; i++) {
                response.data.allStatus.sensorLevels[i] = random(0, 100);
            }
            
            int sent = espnow.broadcastResponse(response);
            Serial.printf("Periodic update sent to %d clients\n", sent);
        }
    }
    
    delay(100);
}
