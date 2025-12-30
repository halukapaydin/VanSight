#include <Arduino.h>
#include <VanSightBLE.h>

BleServerManager bleServer;
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 5000; // Send data every 5 seconds
int messageCounter = 0;

// Callback function when data is received from client
void onDataReceived(String data) {
    Serial.println("=================================");
    Serial.print("Callback - Received from client: ");
    Serial.println(data);
    Serial.println("=================================");
    
    // Echo back to client
    String response = "Server received: " + data;
    bleServer.sendData(response);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Server...");
    
    // Initialize BLE Server
    if (bleServer.begin()) {
        Serial.println("BLE Server initialized successfully!");
    } else {
        Serial.println("Failed to initialize BLE Server!");
        return;
    }
    
    // Set callback for received data
    bleServer.setDataReceivedCallback(onDataReceived);
    
    Serial.println("Server is ready and advertising...");
}

void loop() {
    // Send periodic messages to connected clients
    if (bleServer.isClientConnected()) {
        unsigned long now = millis();
        if (now - lastSendTime >= SEND_INTERVAL) {
            lastSendTime = now;
            
            String message = "Server message #" + String(messageCounter++);
            Serial.print("Sending to client: ");
            Serial.println(message);
            
            if (bleServer.sendData(message)) {
                Serial.println("Data sent successfully");
            } else {
                Serial.println("Failed to send data");
            }
        }
    }
    
    delay(100);
}
