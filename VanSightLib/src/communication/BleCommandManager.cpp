#include "BleCommandManager.h"
#include "../protocol/CommandParser.h"
#include "../protocol/ResponseBuilder.h"
#include <ArduinoJson.h>

namespace VanSight {

BleCommandManager& BleCommandManager::getInstance()
{
    static BleCommandManager instance;
    return instance;
}

BleCommandManager::BleCommandManager()
    : _ble(nullptr),
      _role(BleRole::CLIENT),
      _initialized(false),
      _dataReceivedCallback(nullptr),
      _relayChangedCallback(nullptr),
      _connectionCallback(nullptr),
      _toggleRelayHandler(nullptr),
      _allRelaysOffHandler(nullptr),
      _statusRequestHandler(nullptr)
{
}

BleCommandManager::~BleCommandManager()
{
    if (_ble) {
        delete _ble;
    }
}

bool BleCommandManager::beginServer(const char* deviceName)
{
    if (_initialized) {
        return true;
    }
    
    _role = BleRole::SERVER;
    _ble = new BleManager(BleRole::SERVER, deviceName);
    
    if (!_ble || !_ble->begin()) {
        return false;
    }
    
    // Register data callback
    _ble->onDataReceived([this](const uint8_t* data, size_t len) {
        handleData(data, len);
    });
    
    // Register connection callback
    _ble->onConnectionChanged([this](bool connected) {
        if (_connectionCallback) {
            _connectionCallback(connected);
        }
    });
    
    _initialized = true;
    return true;
}

bool BleCommandManager::beginClient(const char* serverName)
{
    if (_initialized) {
        return true;
    }
    
    _role = BleRole::CLIENT;
    _ble = new BleManager(BleRole::CLIENT, serverName);
    
    if (!_ble || !_ble->begin()) {
        return false;
    }
    
    // Register data callback
    _ble->onDataReceived([this](const uint8_t* data, size_t len) {
        handleData(data, len);
    });
    
    // Register connection callback
    _ble->onConnectionChanged([this](bool connected) {
        if (_connectionCallback) {
            _connectionCallback(connected);
        }
    });
    
    _initialized = true;
    return true;
}

bool BleCommandManager::isInitialized() const
{
    return _initialized && _ble && _ble->isInitialized();
}

bool BleCommandManager::isConnected() const
{
    return _ble && _ble->isConnected();
}

// ============================================================================
// CLIENT MODE - SEND COMMANDS
// ============================================================================

void BleCommandManager::toggleRelay(uint8_t relayNum)
{
    if (!_ble || _role != BleRole::CLIENT) {
        return;
    }
    
    Command cmd;
    cmd.type = CMD_RELAY_TOGGLE;
    cmd.params.relay.relayNum = relayNum;
    
    // Build JSON
    JsonDocument doc;
    doc["cmd"] = "relay_toggle";
    doc["relay"] = relayNum;
    
    char buffer[256];
    size_t len = serializeJson(doc, buffer);
    
    // Append newline as delimiter
    if (len < sizeof(buffer) - 1) {
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
        len++;
    }
    
    _ble->sendData((uint8_t*)buffer, len);
}

void BleCommandManager::allRelaysOff()
{
    if (!_ble || _role != BleRole::CLIENT) {
        return;
    }
    
    JsonDocument doc;
    doc["cmd"] = "all_relays_off";
    
    char buffer[256];
    size_t len = serializeJson(doc, buffer);
    
    // Append newline as delimiter
    if (len < sizeof(buffer) - 1) {
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
        len++;
    }
    
    _ble->sendData((uint8_t*)buffer, len);
}

void BleCommandManager::requestStatus()
{
    if (!_ble || _role != BleRole::CLIENT) {
        return;
    }
    
    JsonDocument doc;
    doc["cmd"] = "all_status";
    
    char buffer[256];
    size_t len = serializeJson(doc, buffer);
    
    // Append newline as delimiter
    if (len < sizeof(buffer) - 1) {
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
        len++;
    }
    
    _ble->sendData((uint8_t*)buffer, len);
}

// ============================================================================
// CLIENT MODE - RECEIVE DATA
// ============================================================================

void BleCommandManager::onDataReceived(std::function<void(const AllStatusData&)> callback)
{
    _dataReceivedCallback = callback;
}

void BleCommandManager::onRelayChanged(std::function<void(uint8_t, bool)> callback)
{
    _relayChangedCallback = callback;
}

void BleCommandManager::onConnectionChanged(std::function<void(bool)> callback)
{
    _connectionCallback = callback;
}

// ============================================================================
// SERVER MODE - HANDLE COMMANDS
// ============================================================================

void BleCommandManager::onToggleRelay(std::function<bool(uint8_t)> handler)
{
    _toggleRelayHandler = handler;
}

void BleCommandManager::onAllRelaysOff(std::function<void()> handler)
{
    _allRelaysOffHandler = handler;
}

void BleCommandManager::onStatusRequest(std::function<AllStatusData()> handler)
{
    _statusRequestHandler = handler;
}

// ============================================================================
// SERVER MODE - SEND UPDATES
// ============================================================================

void BleCommandManager::sendRelayState(uint8_t relayNum, bool state)
{
    if (!_ble || _role != BleRole::SERVER) {
        return;
    }
    
    // Build JSON directly for BLE
    JsonDocument doc;
    doc["status"] = "ok";
    
    JsonObject data = doc.createNestedObject("data");
    data["relay"] = relayNum;
    data["state"] = state;
    
    char buffer[256];
    size_t len = serializeJson(doc, buffer);
    
    // Append newline as delimiter
    if (len < sizeof(buffer) - 1) {
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
        len++;
    }
    
    Serial.printf("[BleCmd] Sending relay state: %d bytes\n", len);
    
    _ble->sendData((uint8_t*)buffer, len);
}

void BleCommandManager::sendAllStatus(const int relayStates[MAX_RELAYS], const int sensorLevels[MAX_SENSORS])
{
    if (!_ble || _role != BleRole::SERVER) {
        return;
    }
    
    // Build JSON directly for BLE
    JsonDocument doc;
    doc["status"] = "ok";
    
    JsonObject data = doc.createNestedObject("data");
    
    // Add relays array
    JsonArray relays = data.createNestedArray("relays");
    for (int i = 0; i < MAX_RELAYS; i++) {
        relays.add(relayStates[i]);
    }
    
    // Add sensors array
    JsonArray sensors = data.createNestedArray("sensors");
    for (int i = 0; i < MAX_SENSORS; i++) {
        JsonObject sensor = sensors.createNestedObject();
        sensor["id"] = i + 1;
        sensor["level"] = sensorLevels[i];
    }
    
    char buffer[512];
    size_t len = serializeJson(doc, buffer);
    
    // Append newline as delimiter
    if (len < sizeof(buffer) - 1) {
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
        len++;
    }
    
    Serial.printf("[BleCmd] Sending status: %d bytes\n", len);
    Serial.printf("[BleCmd] JSON: %s\n", buffer);
    
    _ble->sendData((uint8_t*)buffer, len);
}

// ============================================================================
// INTERNAL HANDLERS
// ============================================================================

void BleCommandManager::handleData(const uint8_t* data, size_t len)
{
    // Debug: Print received data
    Serial.printf("[BleCmd] RX chunk: %d bytes\n", len);
    
    // Append to buffer
    for (size_t i = 0; i < len; i++) {
        _rxBuffer += (char)data[i];
    }
    
    // Process complete messages
    int newlinePos;
    while ((newlinePos = _rxBuffer.indexOf('\n')) != -1) {
        // Extract message
        String message = _rxBuffer.substring(0, newlinePos);
        
        // Remove from buffer (including newline)
        _rxBuffer = _rxBuffer.substring(newlinePos + 1);
        
        // Trim whitespace
        message.trim();
        
        if (message.length() == 0) {
            continue;
        }
        
        Serial.printf("[BleCmd] Full Message: %s\n", message.c_str());
        
        // Parse JSON
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, message);
        
        if (error) {
            Serial.printf("[BleCmd] JSON parse error: %s\n", error.c_str());
            continue;
        }
        
        if (_role == BleRole::SERVER) {
            // Server receives commands
            Command cmd;
            // Note: CommandParser expects raw bytes, but we have a JSON object now
            // We need to adapt CommandParser or parse manually here
            // Since we switched to JSON commands, let's parse manually for now
            
            if (doc.containsKey("cmd")) {
                const char* cmdType = doc["cmd"];
                
                if (strcmp(cmdType, "relay_toggle") == 0) {
                    cmd.type = CMD_RELAY_TOGGLE;
                    cmd.params.relay.relayNum = doc["relay"];
                    handleCommand(cmd);
                }
                else if (strcmp(cmdType, "all_relays_off") == 0) {
                    cmd.type = CMD_ALL_RELAYS_OFF;
                    handleCommand(cmd);
                }
                else if (strcmp(cmdType, "all_status") == 0) {
                    cmd.type = CMD_ALL_STATUS;
                    handleCommand(cmd);
                }
            }
        } else {
            // Client receives responses
            const char* status = doc["status"];
            if (status && strcmp(status, "ok") == 0) {
                // Parse response data
                if (doc.containsKey("data")) {
                    JsonObject dataObj = doc["data"];
                    
                    // Check if it's all status
                    if (dataObj.containsKey("relays") && dataObj.containsKey("sensors")) {
                        AllStatusData allData;
                        
                        JsonArray relays = dataObj["relays"];
                        for (size_t i = 0; i < MAX_RELAYS && i < relays.size(); i++) {
                            allData.relayStates[i] = relays[i];
                        }
                        
                        JsonArray sensors = dataObj["sensors"];
                        for (size_t i = 0; i < MAX_SENSORS && i < sensors.size(); i++) {
                            JsonObject sensor = sensors[i];
                            allData.sensorLevels[i] = sensor["level"];
                        }
                        
                        if (_dataReceivedCallback) {
                            _dataReceivedCallback(allData);
                        }
                    }
                    // Check if it's single relay
                    else if (dataObj.containsKey("relay") && dataObj.containsKey("state")) {
                        uint8_t relayNum = dataObj["relay"];
                        bool state = dataObj["state"];
                        
                        if (_relayChangedCallback) {
                            _relayChangedCallback(relayNum, state);
                        }
                    }
                }
            }
        }
    }
}

void BleCommandManager::handleCommand(const Command& cmd)
{
    Response response;
    response.status = STATUS_OK;
    
    switch (cmd.type) {
        case CMD_RELAY_TOGGLE:
            if (_toggleRelayHandler) {
                bool newState = _toggleRelayHandler(cmd.params.relay.relayNum);
                sendRelayState(cmd.params.relay.relayNum, newState);
            }
            break;
            
        case CMD_ALL_RELAYS_OFF:
            if (_allRelaysOffHandler) {
                _allRelaysOffHandler();
            }
            break;
            
        case CMD_ALL_STATUS:
            if (_statusRequestHandler) {
                AllStatusData data = _statusRequestHandler();
                sendAllStatus(data.relayStates, data.sensorLevels);
            }
            break;
            
        default:
            break;
    }
}

// ============================================================================
// HELPER METHODS
// ============================================================================

Response BleCommandManager::createRelayResponse(uint8_t relayNum, bool state)
{
    Response response;
    response.status = STATUS_OK;
    response.data.relay.relayNum = relayNum;
    response.data.relay.state = state;
    return response;
}

Response BleCommandManager::createAllStatusResponse(const AllStatusData& data)
{
    Response response;
    response.status = STATUS_OK;
    memcpy(response.data.allStatus.relayStates, data.relayStates, sizeof(data.relayStates));
    memcpy(response.data.allStatus.sensorLevels, data.sensorLevels, sizeof(data.sensorLevels));
    return response;
}

} // namespace VanSight
