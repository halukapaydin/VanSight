#include "ESPNowManager.h"

namespace VanSight {

// Static instance pointer
ESPNowManager* ESPNowManager::_instance = nullptr;

ESPNowManager::ESPNowManager(ESPNowRole role, const uint8_t* peerMac, uint8_t channel)
    : _role(role),
      _channel(channel),
      _retryCount(3),
      _verbose(true),
      _initialized(false),
      _peerCount(0),
      _commandCallback(nullptr),
      _responseCallback(nullptr)
{
    memset(_peerMac, 0, sizeof(_peerMac));
    memset(_lastSenderMac, 0, sizeof(_lastSenderMac));
    memset(_peerList, 0, sizeof(_peerList));
    
    if (peerMac) {
        memcpy(_peerMac, peerMac, 6);
    }
    
    _instance = this;
}

bool ESPNowManager::begin()
{
    if (_initialized) {
        return true;
    }
    
    log("[ESPNow] Initializing as %s...", _role == ESPNowRole::SERVER ? "SERVER" : "CLIENT");
    
    // Initialize WiFi
    if (!initWiFi()) {
        log("[ESPNow] WiFi initialization failed");
        return false;
    }
    
    // Initialize ESP-NOW
    if (!initESPNow()) {
        log("[ESPNow] ESP-NOW initialization failed");
        return false;
    }
    
    // Add peer if provided (Client mode)
    if (_role == ESPNowRole::CLIENT && _peerMac[0] != 0) {
        if (!addPeer(_peerMac)) {
            log("[ESPNow] Failed to add peer");
            return false;
        }
        log("[ESPNow] Peer added: %02X:%02X:%02X:%02X:%02X:%02X",
            _peerMac[0], _peerMac[1], _peerMac[2], _peerMac[3], _peerMac[4], _peerMac[5]);
    }
    
    _initialized = true;
    log("[ESPNow] Initialization complete");
    return true;
}

bool ESPNowManager::initWiFi()
{
    // Set WiFi to STA mode
    WiFi.mode(WIFI_STA);
    WiFi.begin();
    
    // Set WiFi channel
    esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE);
    
    // Disable power save
    esp_wifi_set_ps(WIFI_PS_NONE);
    
    log("[ESPNow] WiFi: STA mode, Channel %d, Power Save OFF", _channel);
    log("[ESPNow] MAC: %s", WiFi.macAddress().c_str());
    
    return true;
}

bool ESPNowManager::initESPNow()
{
    if (esp_now_init() != ESP_OK) {
        return false;
    }
    
    // Register callbacks
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);
    
    return true;
}

bool ESPNowManager::sendCommand(const Command& cmd)
{
    if (!_initialized) {
        log("[ESPNow] Not initialized");
        return false;
    }
    
    if (_role != ESPNowRole::CLIENT) {
        log("[ESPNow] sendCommand only available in CLIENT mode");
        return false;
    }
    
    // Build JSON from command
    JsonDocument doc;
    doc["cmd"] = commandTypeToString(cmd.type);
    
    switch (cmd.type) {
        case CMD_RELAY_TOGGLE:
            doc["relay"] = cmd.params.relay.relayNum;
            break;
        case CMD_SENSOR_READ:
            doc["sensor"] = cmd.params.sensor.sensorNum;
            break;
        case CMD_ALL_RELAYS_OFF:
        case CMD_ALL_STATUS:
            // No parameters
            break;
        default:
            log("[ESPNow] Unknown command type");
            return false;
    }
    
    char buffer[MAX_PAYLOAD_SIZE];
    size_t len = serializeJson(doc, buffer, sizeof(buffer));
    
    log("[ESPNow] TX Command: %s", buffer);
    
    return sendData((uint8_t*)buffer, len, _peerMac);
}

bool ESPNowManager::sendResponse(const Response& response, const uint8_t* targetMac)
{
    if (!_initialized) {
        log("[ESPNow] Not initialized");
        return false;
    }
    
    if (_role != ESPNowRole::SERVER) {
        log("[ESPNow] sendResponse only available in SERVER mode");
        return false;
    }
    
    // Use last sender if no target specified
    const uint8_t* target = targetMac ? targetMac : _lastSenderMac;
    
    // Build JSON from response
    char buffer[MAX_PAYLOAD_SIZE];
    size_t len = ResponseBuilder::build(response, buffer, sizeof(buffer));
    
    log("[ESPNow] TX Response: %s", buffer);
    
    // Add peer if not exists
    if (!esp_now_is_peer_exist(target)) {
        addPeer(target);
    }
    
    return sendData((uint8_t*)buffer, len, target);
}

int ESPNowManager::broadcastResponse(const Response& response)
{
    if (!_initialized) {
        log("[ESPNow] Not initialized");
        return 0;
    }
    
    if (_role != ESPNowRole::SERVER) {
        log("[ESPNow] broadcastResponse only available in SERVER mode");
        return 0;
    }
    
    // Build JSON from response
    char buffer[MAX_PAYLOAD_SIZE];
    size_t len = ResponseBuilder::build(response, buffer, sizeof(buffer));
    
    log("[ESPNow] Broadcasting to %d clients: %s", _peerCount, buffer);
    
    int successCount = 0;
    
    // Send to all registered peers
    for (int i = 0; i < _peerCount; i++) {
        if (sendData((uint8_t*)buffer, len, _peerList[i])) {
            successCount++;
        }
    }
    
    log("[ESPNow] Broadcast sent to %d/%d clients", successCount, _peerCount);
    return successCount;
}

bool ESPNowManager::sendData(const uint8_t* data, size_t len, const uint8_t* targetMac)
{
    esp_err_t result = esp_now_send(targetMac, data, len);
    
    if (result != ESP_OK) {
        log("[ESPNow] Send failed: %d", result);
        return false;
    }
    
    return true;
}

void ESPNowManager::onCommandReceived(std::function<void(const Command&, const uint8_t*)> callback)
{
    _commandCallback = callback;
}

void ESPNowManager::onResponseReceived(std::function<void(const Response&)> callback)
{
    _responseCallback = callback;
}

bool ESPNowManager::addPeer(const uint8_t* mac)
{
    if (esp_now_is_peer_exist(mac)) {
        return true; // Already exists
    }
    
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = _channel;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        // Add to peer list if there's space
        if (_peerCount < ESP_NOW_MAX_TOTAL_PEER_NUM) {
            memcpy(_peerList[_peerCount], mac, 6);
            _peerCount++;
            log("[ESPNow] Peer added (%d total): %02X:%02X:%02X:%02X:%02X:%02X",
                _peerCount, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
        return true;
    }
    
    return false;
}

bool ESPNowManager::removePeer(const uint8_t* mac)
{
    if (esp_now_del_peer(mac) == ESP_OK) {
        // Remove from peer list
        for (int i = 0; i < _peerCount; i++) {
            if (memcmp(_peerList[i], mac, 6) == 0) {
                // Shift remaining peers
                for (int j = i; j < _peerCount - 1; j++) {
                    memcpy(_peerList[j], _peerList[j + 1], 6);
                }
                _peerCount--;
                log("[ESPNow] Peer removed (%d remaining)", _peerCount);
                break;
            }
        }
        return true;
    }
    return false;
}

void ESPNowManager::setChannel(uint8_t channel)
{
    _channel = channel;
    if (_initialized) {
        esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE);
    }
}

void ESPNowManager::setRetryCount(uint8_t count)
{
    _retryCount = count;
}

// Static callbacks
void ESPNowManager::onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    if (_instance) {
        _instance->handleDataSent(mac_addr, status);
    }
}

void ESPNowManager::onDataRecv(const uint8_t* mac, const uint8_t* data, int len)
{
    if (_instance) {
        _instance->handleDataRecv(mac, data, len);
    }
}

// Internal handlers
void ESPNowManager::handleDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    if (_verbose) {
        log("[ESPNow] Send status: %s", status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAILED");
    }
}

void ESPNowManager::handleDataRecv(const uint8_t* mac, const uint8_t* data, int len)
{
    log("[ESPNow] RX %d bytes from %02X:%02X:%02X:%02X:%02X:%02X",
        len, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    // Store sender MAC
    memcpy(_lastSenderMac, mac, 6);
    
    // Server mode: receive commands
    if (_role == ESPNowRole::SERVER) {
        Command cmd;
        if (CommandParser::parse(data, len, cmd)) {
            log("[ESPNow] Command received: %s", commandTypeToString(cmd.type));
            if (_commandCallback) {
                _commandCallback(cmd, mac);
            }
        } else {
            log("[ESPNow] Failed to parse command");
        }
    }
    // Client mode: receive responses
    else if (_role == ESPNowRole::CLIENT) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);
        
        if (error) {
            log("[ESPNow] JSON parse error: %s", error.c_str());
            return;
        }
        
        // Parse response (simplified - you may want to create ResponseParser)
        Response response;
        const char* status = doc["status"];
        response.status = (status && strcmp(status, "ok") == 0) ? STATUS_OK : STATUS_ERROR;
        
        if (_responseCallback) {
            _responseCallback(response);
        }
    }
}

void ESPNowManager::log(const char* format, ...)
{
    if (!_verbose) return;
    
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Serial.println(buffer);
}

} // namespace VanSight
