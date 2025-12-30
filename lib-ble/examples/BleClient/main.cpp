#include <Arduino.h>
#include <VanSightBLE.h>

BleClientManager bleClient;
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 7000; // Send data every 7 seconds
int messageCounter = 0;

// Callback function when data is received from server
void onDataReceived(String data) {
    Serial.println("=================================");
    Serial.print("Callback - Received from server: ");
    Serial.println(data);
    Serial.println("=================================");
}

void printState(BleClientState state) {
    Serial.print("Current state: ");
    switch (state) {
        case BLE_DISCONNECTED:
            Serial.println("DISCONNECTED");
            break;
        case BLE_SCANNING:
            Serial.println("SCANNING");
            break;
        case BLE_CONNECTING:
            Serial.println("CONNECTING");
            break;
        case BLE_CONNECTED:
            Serial.println("CONNECTED");
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Client...");
    
    // Initialize BLE Client
    if (bleClient.begin()) {
        Serial.println("BLE Client initialized successfully!");
    } else {
        Serial.println("Failed to initialize BLE Client!");
    }
    
    // Set callback for received data
    bleClient.setDataReceivedCallback(onDataReceived);
    
    Serial.println("Client will auto-connect to server...");
}

void loop() {
    // IMPORTANT: Must call loop() for auto-reconnection to work
    bleClient.loop();
    
    // Print state changes
    static BleClientState lastState = BLE_DISCONNECTED;
    BleClientState currentState = bleClient.getState();
    if (currentState != lastState) {
        printState(currentState);
        lastState = currentState;
    }
    
    // Send periodic messages when connected
    if (bleClient.isConnected()) {
        unsigned long now = millis();
        if (now - lastSendTime >= SEND_INTERVAL) {
            lastSendTime = now;
            
            String message = "Client message #" + String(messageCounter++);
            Serial.print("Sending to server: ");
            Serial.println(message);
            
            if (bleClient.sendData(message)) {
                Serial.println("Data sent successfully");
            } else {
                Serial.println("Failed to send data");
            }
        }
    }
    
    delay(100);
}
