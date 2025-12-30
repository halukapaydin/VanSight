#include <Arduino.h>
#include <uart/VanSightUart.h>

// Initialize UART Manager on Serial1
UartManager uart(Serial1);

void onPacketReceived(String data) {
    Serial.print("Arduino Received: ");
    Serial.println(data);
}

void setup() {
    Serial.begin(115200);
    
    // Start UART on Serial1
    uart.begin(115200);
    
    // Register callback
    uart.onDataReceived(onPacketReceived);
    
    Serial.println("Arduino UART Node Started");
}

void loop() {
    // Process incoming data
    uart.loop();
    
    // Example: send PING every 3 seconds
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 3000) {
        lastUpdate = millis();
        Serial.println("Sending PING to ESP32...");
        uart.sendData("PING");
    }
}
