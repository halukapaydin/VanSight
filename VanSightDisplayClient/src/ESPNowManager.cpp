#include "ESPNowManager.h"

// Singleton instance
ESPNowManager& ESPNowManager::getInstance() {
    static ESPNowManager instance;
    return instance;
}

ESPNowManager::ESPNowManager()
    : _initialized(false),
      _relayStatusCallback(nullptr),
      _sensorDataCallback(nullptr),
      _allStatusCallback(nullptr) {
}

ESPNowManager::~ESPNowManager() {
    if (_initialized) {
        esp_now_deinit();
    }
}

bool ESPNowManager::init() {
    if (_initialized) {
        return true;
    }
    
    Serial.println("\n[ESP-NOW] Initializing...");
    
    // Set WiFi to STA mode (required for ESP-NOW)
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    Serial.print("[ESP-NOW] MAC Address: ");
    Serial.println(WiFi.macAddress());
    
    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[ESP-NOW] ✗ Initialization failed!");
        return false;
    }
    
    Serial.println("[ESP-NOW] ✓ Initialized");
    
    // Register callbacks
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataReceived);
    
    // Add VanSightHub as peer
    memset(&_peerInfo, 0, sizeof(_peerInfo));
    memcpy(_peerInfo.peer_addr, HUB_MAC_ADDRESS, 6);
    _peerInfo.channel = 0;
    _peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&_peerInfo) != ESP_OK) {
        Serial.println("[ESP-NOW] ✗ Failed to add peer!");
        return false;
    }
    
    Serial.printf("[ESP-NOW] ✓ Peer added: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  HUB_MAC_ADDRESS[0], HUB_MAC_ADDRESS[1], HUB_MAC_ADDRESS[2],
                  HUB_MAC_ADDRESS[3], HUB_MAC_ADDRESS[4], HUB_MAC_ADDRESS[5]);
    
    _initialized = true;
    return true;
}

void ESPNowManager::sendRelayToggle(uint8_t relayNum) {
    if (!_initialized) {
        Serial.println("[ESP-NOW] Not initialized!");
        return;
    }
    
    JsonDocument doc;
    doc["relay"] = relayNum;
    sendCommand("relay_toggle", doc);
}

void ESPNowManager::sendAllRelaysOff() {
    if (!_initialized) {
        Serial.println("[ESP-NOW] Not initialized!");
        return;
    }
    
    JsonDocument doc;
    sendCommand("all_relays_off", doc);
}

void ESPNowManager::requestAllStatus() {
    if (!_initialized) {
        Serial.println("[ESP-NOW] Not initialized!");
        return;
    }
    
    JsonDocument doc;
    sendCommand("all_status", doc);
}

void ESPNowManager::sendCommand(const char* cmd, JsonDocument& doc) {
    doc["cmd"] = cmd;
    
    char buffer[MAX_ESPNOW_PAYLOAD];
    size_t len = serializeJson(doc, buffer, sizeof(buffer));
    
    Serial.printf("[ESP-NOW TX] %s\n", buffer);
    
    esp_err_t result = esp_now_send(HUB_MAC_ADDRESS, (uint8_t*)buffer, len);
    
    if (result != ESP_OK) {
        Serial.printf("[ESP-NOW] ✗ Send failed: %d\n", result);
    }
}

void ESPNowManager::setRelayStatusCallback(RelayStatusCallback callback) {
    _relayStatusCallback = callback;
}

void ESPNowManager::setSensorDataCallback(SensorDataCallback callback) {
    _sensorDataCallback = callback;
}

void ESPNowManager::setAllStatusCallback(AllStatusCallback callback) {
    _allStatusCallback = callback;
}

// Static callbacks for ESP-NOW C API
void ESPNowManager::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.printf("[ESP-NOW] Send status: %s\n", 
                  status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

void ESPNowManager::onDataReceived(const uint8_t *mac, const uint8_t *data, int len) {
    Serial.printf("[ESP-NOW RX] %d bytes from %02X:%02X:%02X:%02X:%02X:%02X\n",
                  len, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    getInstance().handleResponse(data, len);
}

void ESPNowManager::handleResponse(const uint8_t* data, int len) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data, len);
    
    if (error) {
        Serial.printf("[ESP-NOW] JSON parse error: %s\n", error.c_str());
        return;
    }
    
    // Debug: Print received JSON
    Serial.print("[ESP-NOW RX] ");
    serializeJson(doc, Serial);
    Serial.println();
    
    const char* status = doc["status"];
    if (!status || strcmp(status, "ok") != 0) {
        Serial.println("[ESP-NOW] Response status not OK");
        return;
    }
    
    JsonObject dataObj = doc["data"];
    if (!dataObj) {
        Serial.println("[ESP-NOW] No data object in response");
        return;
    }
    
    // Check if this is a relay status response
    if (dataObj.containsKey("relay")) {
        parseRelayStatus(doc);
    }
    // Check if this is a sensor status response
    else if (dataObj.containsKey("sensor")) {
        parseSensorData(doc);
    }
    // Check if this is an all_status response
    else if (dataObj.containsKey("relays") && dataObj.containsKey("sensors")) {
        parseAllStatus(doc);
    }
}

void ESPNowManager::parseRelayStatus(JsonDocument& doc) {
    JsonObject data = doc["data"];
    uint8_t relayNum = data["relay"];
    const char* stateStr = data["state"];
    bool state = (strcmp(stateStr, "on") == 0);
    
    Serial.printf("[ESP-NOW] Relay %d: %s\n", relayNum, state ? "ON" : "OFF");
    
    if (_relayStatusCallback) {
        _relayStatusCallback(relayNum, state);
    }
}

void ESPNowManager::parseSensorData(JsonDocument& doc) {
    JsonObject data = doc["data"];
    uint8_t sensorNum = data["sensor"];
    int level = data["level"];
    
    Serial.printf("[ESP-NOW] Sensor %d: %d%%\n", sensorNum, level);
    
    if (_sensorDataCallback) {
        _sensorDataCallback(sensorNum, level);
    }
}

void ESPNowManager::parseAllStatus(JsonDocument& doc) {
    JsonObject data = doc["data"];
    JsonArray relays = data["relays"];
    JsonArray sensors = data["sensors"];
    
    bool relayStates[16] = {false};
    int sensorLevels[3] = {0};
    
    // Parse relay states
    int relayCount = min((int)relays.size(), 16);
    for (int i = 0; i < relayCount; i++) {
        relayStates[i] = (relays[i].as<int>() == 1);
        Serial.printf("[ESP-NOW] Relay %d: %s\n", i+1, relayStates[i] ? "ON" : "OFF");
    }
    
    // Parse sensor levels
    int sensorCount = min((int)sensors.size(), 3);
    for (int i = 0; i < sensorCount; i++) {
        JsonObject sensor = sensors[i];
        int id = sensor["id"];
        int level = sensor["level"];
        if (id >= 1 && id <= 3) {
            sensorLevels[id - 1] = level;
            Serial.printf("[ESP-NOW] Sensor %d: %d%%\n", id, level);
        }
    }
    
    if (_allStatusCallback) {
        _allStatusCallback(relayStates, sensorLevels);
    }
}
