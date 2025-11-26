/*
 * ESP-NOW Server Example (Hub)
 * 
 * Demonstrates using ESPNowManager in SERVER mode to receive commands
 * and send responses.
 */

#include <VanSightLib.h>

using namespace VanSight;

// Create ESP-NOW manager in SERVER mode
ESPNowManager espnow(ESPNowRole::SERVER);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== ESP-NOW Server (Hub) ===\n");
    
    // Initialize ESP-NOW
    if (!espnow.begin()) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }
    
    // Register command callback
    espnow.onCommandReceived([](const Command& cmd, const uint8_t* senderMac) {
        Serial.printf("Command received from %02X:%02X:%02X:%02X:%02X:%02X\n",
                      senderMac[0], senderMac[1], senderMac[2], 
                      senderMac[3], senderMac[4], senderMac[5]);
        
        // Create response
        Response response;
        response.status = STATUS_OK;
        
        // Handle command
        switch (cmd.type) {
            case CMD_RELAY_TOGGLE:
                Serial.printf("Toggle relay %d\n", cmd.params.relay.relayNum);
                response.data.relay.relayNum = cmd.params.relay.relayNum;
                response.data.relay.state = true; // Simulated state
                break;
                
            case CMD_ALL_RELAYS_OFF:
                Serial.println("All relays OFF");
                break;
                
            case CMD_ALL_STATUS:
                Serial.println("Status request");
                // Fill all status data
                for (int i = 0; i < MAX_RELAYS; i++) {
                    response.data.allStatus.relayStates[i] = false;
                }
                for (int i = 0; i < MAX_SENSORS; i++) {
                    response.data.allStatus.sensorLevels[i] = 50; // 50%
                }
                break;
                
            default:
                Serial.println("Unknown command");
                response.status = STATUS_INVALID_COMMAND;
                break;
        }
        
        // Send response back to sender
        espnow.sendResponse(response, senderMac);
    });
    
    Serial.println("Server ready, waiting for commands...\n");
}

void loop() {
    // Nothing to do - callbacks handle everything
    delay(100);
}
