#define ASYNCWEBSERVER_REGEX 1
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "RelayController.h"
#include "SensorController.h"
#include "CommandHandler.h"
#include "HTTPApiHandler.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
// Hardware controllers
RelayController relayController(RELAY_PINS, 16);
SensorController sensorController(3);

// Command handler
CommandHandler commandHandler(relayController, sensorController);

// Web server and HTTP API
AsyncWebServer server(80);
HTTPApiHandler httpApiHandler(server, commandHandler);

// ESP-NOW
uint8_t senderMacAddress[6];

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================
void initSensors();
void initWiFi();
void initESPNow();
void sendESPNowResponse(JsonDocument& response);
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("VanSightHub - Dual Protocol");
  Serial.println("ESP-NOW + HTTP API");
  Serial.println("=================================\n");
  
  // Initialize relays
  Serial.println("Initializing relays...");
  relayController.begin();
  Serial.printf("✓ %d relays initialized\n\n", relayController.getCount());
  
  // Initialize sensors
  // Initialize sensors
  initSensors();
  
  // Initialize WiFi (for both ESP-NOW and HTTP)
  initWiFi();
  
  // Initialize ESP-NOW
  initESPNow();
  
  // Initialize HTTP API
  httpApiHandler.begin();
  server.begin();
  Serial.println("✓ Web Server started\n");
  
  Serial.println("\n=================================");
  Serial.println("System ready!");
  Serial.println("=================================");
  Serial.println("ESP-NOW: Listening for commands");
  Serial.print("HTTP API: http://");
  Serial.println(WiFi.localIP());
  Serial.println("=================================\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  delay(10);
}

// ============================================================================
// SENSOR INITIALIZATION
// ============================================================================
void initSensors() {
  Serial.println("Initializing sensors...");
  
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  sensorController.addSensor(0, SENSOR_PINS[0], SENSOR1_MIN_RESISTANCE, SENSOR1_MAX_RESISTANCE, REFERENCE_RESISTOR);
  sensorController.addSensor(1, SENSOR_PINS[1], SENSOR23_MIN_RESISTANCE, SENSOR23_MAX_RESISTANCE, REFERENCE_RESISTOR);
  sensorController.addSensor(2, SENSOR_PINS[2], SENSOR23_MIN_RESISTANCE, SENSOR23_MAX_RESISTANCE, REFERENCE_RESISTOR);
  
  sensorController.begin();
  
  Serial.printf("✓ %d level sensors initialized\n\n", sensorController.getCount());
}

// ============================================================================
// WiFi INITIALIZATION
// ============================================================================
void initWiFi() {
  Serial.println("Initializing WiFi...");
  
  if (WIFI_AP_MODE) {
    // Access Point mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    
    Serial.println("✓ WiFi AP Mode");
    Serial.print("  SSID: ");
    Serial.println(WIFI_SSID);
    Serial.print("  IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    // Station mode
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);
    
    Serial.print("Connecting to WiFi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✓ WiFi STA Mode");
      Serial.print("  IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\n✗ WiFi connection failed!");
    }
  }
  
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println();
}

// ============================================================================
// ESP-NOW FUNCTIONS
// ============================================================================
void initESPNow() {
  Serial.println("Initializing ESP-NOW...");
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("✗ ESP-NOW initialization failed!");
    return;
  }
  
  Serial.println("✓ ESP-NOW initialized");
  
  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onDataSent);
  
  Serial.println("✓ ESP-NOW callbacks registered\n");
}

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(senderMacAddress, mac, 6);
  
  Serial.printf("\n[ESP-NOW RX] %d bytes from %02X:%02X:%02X:%02X:%02X:%02X\n",
                len, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  
  JsonDocument response;
  commandHandler.processCommand(incomingData, len, response);
  
  sendESPNowResponse(response);
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.printf("[ESP-NOW TX] Status: %s\n", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

void sendESPNowResponse(JsonDocument& response) {
  Serial.println("\n[ESP-NOW Response]:");
  serializeJsonPretty(response, Serial);
  Serial.println("\n");
  
  char buffer[MAX_ESPNOW_PAYLOAD];
  size_t len = serializeJson(response, buffer, sizeof(buffer));
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, senderMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (!esp_now_is_peer_exist(senderMacAddress)) {
    esp_now_add_peer(&peerInfo);
  }
  
  esp_now_send(senderMacAddress, (uint8_t*)buffer, len);
}
