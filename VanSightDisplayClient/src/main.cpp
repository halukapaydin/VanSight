#include <Arduino.h>
#include <VanSightLib.h>

#include "espnow_config.h"
#include "PanelManager.h"
#include "UIStateManager.h"
#include "SleepManager.h"

using namespace VanSight;

bool bleInitialized = false;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\\n=== VanSight DisplayClient ===\\n");
    
    // Initialize Panel (LVGL, Display, Touch, IO Expander)
    if (!PanelManager::getInstance().begin()) {
        Serial.println("Panel initialization failed!");
        return;
    }
    
    // Initialize UI State Manager
    UIStateManager::getInstance().init(
        PanelManager::getInstance().getLockFunction(),
        PanelManager::getInstance().getUnlockFunction()
    );
    
    // Initialize Sleep Manager
    SleepManager::getInstance().init(
        PanelManager::getInstance().getPanel(),
        PanelManager::getInstance().getExpander()
    );
    
    Serial.println("\\n=== Setup Complete ===\\n");
}

void loop()
{
    // Initialize BLE after UI is ready (one-time initialization)
    if (!bleInitialized) {
        Serial.println("[BLE] Starting BLE initialization...");
        
        // Initialize BleCommandManager as Client
        if (BleCommandManager::getInstance().beginClient("VanSightHub")) {
            Serial.println("[BLE] BleCommandManager initialized!");

            // BleCommandManager::getInstance().onConnectionChanged([](bool connected) {
            //                 Serial.printf("[BLE] Connection changed to %s\\n", connected? "ON" : "OFF");
            //                 UIStateManager::getInstance().updateConnectionStatus(connected);
            //             });

            // Register data received callback
            BleCommandManager::getInstance().onDataReceived([](const AllStatusData& data) {
                Serial.println("[BLE] Status data received from Hub");
                
                // Create non-const copies for UIStateManager
                bool relayStates[MAX_RELAYS];
                int sensorLevels[MAX_SENSORS];
                
                memcpy(relayStates, data.relayStates, sizeof(relayStates));
                memcpy(sensorLevels, data.sensorLevels, sizeof(sensorLevels));
                
                // Update UI with relay states
                UIStateManager::getInstance().updateAllRelayStates(relayStates);
                
                // Update UI with sensor levels
                UIStateManager::getInstance().updateAllSensorLevels(sensorLevels);
            });
            
            // Register relay changed callback
            BleCommandManager::getInstance().onRelayChanged([](uint8_t relayNum, bool state) {
                Serial.printf("[BLE] Relay %d changed to %s\\n", relayNum, state ? "ON" : "OFF");
                UIStateManager::getInstance().updateRelayButton(relayNum, state);
            });
            
            // Register connection status callback to update title color
//            BleCommandManager::getInstance().onConnectionChanged([](bool connected) {
//               if (connected) {
//                    // Yellow when connected
//                    ui_setTitleColor(COLOR_CONNECTION_ACTIVE);
//                    Serial.println("[UI] Title color: YELLOW (connected)");
//                } else {
//                    // Gray when disconnected
//                    ui_setTitleColor(COLOR_CONNECTION_LOST);
//                    Serial.println("[UI] Title color: GRAY (disconnected)");
//                }
//            });
//
            // Set blue color while connecting
//            ui_setTitleColor(COLOR_CONNECTION_ACTIVE);
//            Serial.println("[UI] Title color: BLUE (connecting)");
            
            bleInitialized = true;
            
            // Request initial status after connection
            delay(2000); // Wait for BLE connection
            BleCommandManager::getInstance().requestStatus();


        } else {
            Serial.println("[BLE] BleCommandManager initialization failed!");
            bleInitialized = true; // Don't retry
        }
    }
    
    // Update sleep manager
    SleepManager::getInstance().update();
    delay(100);
}
