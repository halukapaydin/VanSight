#include <Arduino.h>

#include "BleCommandHandler.h"
#include "PanelManager.h"
#include "UIStateManager.h"
#include "SleepManager.h"
#include "json/JsonCodec.h"
#include "protocol/protocol.h"


QueueHandle_t uiUpdateQueue;

void onDataReceivedOverBle(String data)
{
    Serial.println("received data : " + data);
    Response response{};
    JsonCodec::instance().deserializeResponse(data, response);
    
    // Send to queue for processing in main loop (avoids deadlock)
    if (xQueueSend(uiUpdateQueue, &response, 0) != pdTRUE) {
        Serial.println("Failed to send response to queue (full?)");
    }
}

void onBleClientStateChange(BleClientState state)
{
    // Ensure this is also safe or deferred if it touches UI. 
    // For now, assuming updateConnectionStatus is safe or we might need to queue this too.
    // However, the main deadlock is usually data reception.
    UIStateManager::getInstance().updateConnectionStatus(state);
    Serial.print("Ble client state : ");
    Serial.println(state);
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== VanSight DisplayClient ===\n");

    // Create queue for UI updates (depth 10 should be sufficient)
    uiUpdateQueue = xQueueCreate(10, sizeof(Response));
    if (uiUpdateQueue == nullptr) {
        Serial.println("Failed to create UI update queue!");
    }
    
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

    BleCommandHandler::instance().setDataReceivedCallback(onDataReceivedOverBle);
    BleCommandHandler::instance().setClientStateCallback(onBleClientStateChange);

    if (BleCommandHandler::instance().begin()) {
        Serial.println("BLE Client initialized successfully!");
    } else {
        Serial.println("Failed to initialize BLE Client!");
    }

    Serial.println("\n=== Setup Complete ===\n");
}

void loop()
{
    // Process UI updates from BLE queue
    Response response;
    while (xQueueReceive(uiUpdateQueue, &response, 0) == pdTRUE) {
        UIStateManager::getInstance().updateAllRelayStates(response.relays);
        UIStateManager::getInstance().updateAllSensorLevels(response.levels);
    }

    // Update sleep manager
    SleepManager::getInstance().update();
    // Non-blocking 5 second periodic status update
    static unsigned long lastStatusUpdate = 0;
    if (!SleepManager::getInstance().isSleeping() && millis() - lastStatusUpdate > 5000) {
        lastStatusUpdate = millis();
        // Only request updates if BLE is connected (optional check, but good for safety)
        BleCommandHandler::instance().allStatus();
    }

    delay(20); // Reduced delay for better responsiveness
}
