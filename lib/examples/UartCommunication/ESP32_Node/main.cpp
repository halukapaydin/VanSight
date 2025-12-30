#include <Arduino.h>
#include <uart/VanSightUart.h>

// Initialize UART Manager on Serial2 with pins (RX=16, TX=17 usually)
UartManager uart(16, 17);

void onPacketReceived(String data) {
    Serial.print("ESP32 Received: ");
    Serial.println(data);
    
    // Echo back or process
    if (data == "PING") {
        uart.sendData("PONG");
    }
}

void setup() {
    Serial.begin(115200);
    
    // Start UART on Serial2 with 115200 baud
    // Pins are set in constructor: uart(16, 17)
    uart.begin(115200);
    
    // Register callback
    uart.onDataReceived(onPacketReceived);
    
    Serial.println("ESP32 UART Node Started");
}

void loop() {
    // Process incoming data
    uart.loop();
    
    // Example: Send heartbeat every 5 seconds
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 5000) {
        lastUpdate = millis();
        Serial.println("Sending status to Arduino...");
        uart.sendData("STATUS_OK");
    }
}
