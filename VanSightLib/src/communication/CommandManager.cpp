#include "CommandManager.h"

namespace VanSight {

CommandManager& CommandManager::getInstance()
{
    static CommandManager instance;
    return instance;
}

CommandManager::CommandManager()
    : _espnow(nullptr),
      _role(ESPNowRole::CLIENT),
      _initialized(false),
      _dataReceivedCallback(nullptr),
      _relayChangedCallback(nullptr),
      _toggleRelayHandler(nullptr),
      _allRelaysOffHandler(nullptr),
      _statusRequestHandler(nullptr)
{
}

CommandManager::~CommandManager()
{
    if (_espnow) {
        delete _espnow;
    }
}

bool CommandManager::beginServer(uint8_t channel)
{
    if (_initialized) {
        return true;
    }
    
    _role = ESPNowRole::SERVER;
    _espnow = new ESPNowManager(ESPNowRole::SERVER, nullptr, channel);
    
    if (!_espnow || !_espnow->begin()) {
        return false;
    }
    
    // Register command callback
    _espnow->onCommandReceived([this](const Command& cmd, const uint8_t* mac) {
        handleCommand(cmd, mac);
    });
    
    _initialized = true;
    return true;
}

bool CommandManager::beginClient(const uint8_t* hubMac, uint8_t channel)
{
    if (_initialized) {
        return true;
    }
    
    _role = ESPNowRole::CLIENT;
    _espnow = new ESPNowManager(ESPNowRole::CLIENT, hubMac, channel);
    
    if (!_espnow || !_espnow->begin()) {
        return false;
    }
    
    // Register response callback
    _espnow->onResponseReceived([this](const Response& response) {
        handleResponse(response);
    });
    
    _initialized = true;
    return true;
}

bool CommandManager::isInitialized() const
{
    return _initialized && _espnow && _espnow->isInitialized();
}

// ============================================================================
// CLIENT MODE - SEND COMMANDS
// ============================================================================

void CommandManager::toggleRelay(uint8_t relayNum)
{
    if (!_espnow || _role != ESPNowRole::CLIENT) {
        return;
    }
    
    Command cmd;
    cmd.type = CMD_RELAY_TOGGLE;
    cmd.params.relay.relayNum = relayNum;
    
    _espnow->sendCommand(cmd);
}

void CommandManager::allRelaysOff()
{
    if (!_espnow || _role != ESPNowRole::CLIENT) {
        return;
    }
    
    Command cmd;
    cmd.type = CMD_ALL_RELAYS_OFF;
    
    _espnow->sendCommand(cmd);
}

void CommandManager::requestStatus()
{
    if (!_espnow || _role != ESPNowRole::CLIENT) {
        return;
    }
    
    Command cmd;
    cmd.type = CMD_ALL_STATUS;
    
    _espnow->sendCommand(cmd);
}

// ============================================================================
// CLIENT MODE - RECEIVE DATA
// ============================================================================

void CommandManager::onDataReceived(std::function<void(const AllStatusData&)> callback)
{
    _dataReceivedCallback = callback;
}

void CommandManager::onRelayChanged(std::function<void(uint8_t, bool)> callback)
{
    _relayChangedCallback = callback;
}

// ============================================================================
// SERVER MODE - HANDLE COMMANDS
// ============================================================================

void CommandManager::onToggleRelay(std::function<bool(uint8_t)> handler)
{
    _toggleRelayHandler = handler;
}

void CommandManager::onAllRelaysOff(std::function<void()> handler)
{
    _allRelaysOffHandler = handler;
}

void CommandManager::onStatusRequest(std::function<AllStatusData()> handler)
{
    _statusRequestHandler = handler;
}

// ============================================================================
// SERVER MODE - BROADCAST UPDATES
// ============================================================================

void CommandManager::broadcastRelayState(uint8_t relayNum, bool state)
{
    if (!_espnow || _role != ESPNowRole::SERVER) {
        return;
    }
    
    Response response = createRelayResponse(relayNum, state);
    _espnow->broadcastResponse(response);
}

void CommandManager::broadcastAllStatus(const int relayStates[MAX_RELAYS], const int sensorLevels[MAX_SENSORS])
{
    if (!_espnow || _role != ESPNowRole::SERVER) {
        return;
    }
    
    AllStatusData data;
    memcpy(data.relayStates, relayStates, sizeof(data.relayStates));
    memcpy(data.sensorLevels, sensorLevels, sizeof(data.sensorLevels));
    
    Response response = createAllStatusResponse(data);
    _espnow->broadcastResponse(response);
}

int CommandManager::getClientCount() const
{
    if (!_espnow || _role != ESPNowRole::SERVER) {
        return 0;
    }
    return _espnow->getPeerCount();
}

// ============================================================================
// INTERNAL HANDLERS
// ============================================================================

void CommandManager::handleCommand(const Command& cmd, const uint8_t* senderMac)
{
    Response response;
    response.status = STATUS_OK;
    
    switch (cmd.type) {
        case CMD_RELAY_TOGGLE:
            if (_toggleRelayHandler) {
                bool newState = _toggleRelayHandler(cmd.params.relay.relayNum);
                response = createRelayResponse(cmd.params.relay.relayNum, newState);
                // Broadcast to all clients
                _espnow->broadcastResponse(response);
            }
            break;
            
        case CMD_ALL_RELAYS_OFF:
            if (_allRelaysOffHandler) {
                _allRelaysOffHandler();
                // Send acknowledgment
                _espnow->sendResponse(response, senderMac);
            }
            break;
            
        case CMD_ALL_STATUS:
            if (_statusRequestHandler) {
                AllStatusData data = _statusRequestHandler();
                response = createAllStatusResponse(data);
                _espnow->sendResponse(response, senderMac);
            }
            break;
            
        default:
            response.status = STATUS_INVALID_COMMAND;
            _espnow->sendResponse(response, senderMac);
            break;
    }
}

void CommandManager::handleResponse(const Response& response)
{
    if (response.status != STATUS_OK) {
        return;
    }
    
    // Check if this is a relay state update
    if (_relayChangedCallback) {
        // Try to extract relay data (simplified - needs proper response type detection)
        _relayChangedCallback(response.data.relay.relayNum, response.data.relay.state);
    }
    
    // Check if this is all status data
    if (_dataReceivedCallback) {
        AllStatusData data;
        memcpy(data.relayStates, response.data.allStatus.relayStates, sizeof(data.relayStates));
        memcpy(data.sensorLevels, response.data.allStatus.sensorLevels, sizeof(data.sensorLevels));
        _dataReceivedCallback(data);
    }
}

// ============================================================================
// HELPER METHODS
// ============================================================================

Response CommandManager::createRelayResponse(uint8_t relayNum, bool state)
{
    Response response;
    response.status = STATUS_OK;
    response.data.relay.relayNum = relayNum;
    response.data.relay.state = state;
    return response;
}

Response CommandManager::createAllStatusResponse(const AllStatusData& data)
{
    Response response;
    response.status = STATUS_OK;
    memcpy(response.data.allStatus.relayStates, data.relayStates, sizeof(data.relayStates));
    memcpy(response.data.allStatus.sensorLevels, data.sensorLevels, sizeof(data.sensorLevels));
    return response;
}

} // namespace VanSight
