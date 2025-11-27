#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <Arduino.h>
#include <functional>
#include "ESPNowManager.h"
#include "../protocol/VanSightProtocol.h"
#include "../config/VanSightConfig.h"

namespace VanSight {

/**
 * @brief Relay state data structure
 */
struct RelayStateData {
    uint8_t relayNum;
    bool state;
};

/**
 * @brief Command Manager - High-level API for ESP-NOW communication
 * 
 * Provides simple methods like toggleRelay(), allRelaysOff(), etc.
 * Hides all ESP-NOW and protocol complexity.
 */
class CommandManager {
public:
    /**
     * @brief Get singleton instance
     */
    static CommandManager& getInstance();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * @brief Initialize as Server (Hub)
     * @param channel WiFi channel (default: 1)
     * @return true if successful
     */
    bool beginServer(uint8_t channel = 1);
    
    /**
     * @brief Initialize as Client (DisplayClient)
     * @param hubMac Hub MAC address
     * @param channel WiFi channel (default: 1)
     * @return true if successful
     */
    bool beginClient(const uint8_t* hubMac, uint8_t channel = 1);
    
    /**
     * @brief Check if initialized
     */
    bool isInitialized() const;
    
    // ========================================================================
    // CLIENT MODE - SEND COMMANDS
    // ========================================================================
    
    /**
     * @brief Toggle a relay
     * @param relayNum Relay number (1-16)
     */
    void toggleRelay(uint8_t relayNum);
    
    /**
     * @brief Turn off all relays
     */
    void allRelaysOff();
    
    /**
     * @brief Request status update
     */
    void requestStatus();
    
    // ========================================================================
    // CLIENT MODE - RECEIVE DATA
    // ========================================================================
    
    /**
     * @brief Register callback for all status data
     */
    void onDataReceived(std::function<void(const AllStatusData&)> callback);
    
    /**
     * @brief Register callback for relay state changes
     */
    void onRelayChanged(std::function<void(uint8_t relayNum, bool state)> callback);
    
    // ========================================================================
    // SERVER MODE - HANDLE COMMANDS
    // ========================================================================
    
    /**
     * @brief Register handler for relay toggle command
     * @param handler Function that toggles relay and returns new state
     */
    void onToggleRelay(std::function<bool(uint8_t relayNum)> handler);
    
    /**
     * @brief Register handler for all relays off command
     */
    void onAllRelaysOff(std::function<void()> handler);
    
    /**
     * @brief Register handler for status request
     * @param handler Function that returns current status
     */
    void onStatusRequest(std::function<AllStatusData()> handler);
    
    // ========================================================================
    // SERVER MODE - BROADCAST UPDATES
    // ========================================================================
    
    /**
     * @brief Broadcast relay state to all clients
     */
    void broadcastRelayState(uint8_t relayNum, bool state);
    
    /**
     * @brief Broadcast all status to all clients
     */
    void broadcastAllStatus(const int relayStates[MAX_RELAYS], const int sensorLevels[MAX_SENSORS]);
    
    /**
     * @brief Get number of connected clients (Server mode)
     */
    int getClientCount() const;

private:
    CommandManager();
    ~CommandManager();
    
    // Prevent copying
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    
    ESPNowManager* _espnow;
    ESPNowRole _role;
    bool _initialized;
    
    // Client callbacks
    std::function<void(const AllStatusData&)> _dataReceivedCallback;
    std::function<void(uint8_t, bool)> _relayChangedCallback;
    
    // Server handlers
    std::function<bool(uint8_t)> _toggleRelayHandler;
    std::function<void()> _allRelaysOffHandler;
    std::function<AllStatusData()> _statusRequestHandler;
    
    // Internal handlers
    void handleCommand(const Command& cmd, const uint8_t* senderMac);
    void handleResponse(const Response& response);
    
    // Helper methods
    Response createRelayResponse(uint8_t relayNum, bool state);
    Response createAllStatusResponse(const AllStatusData& data);
};

} // namespace VanSight

#endif // COMMAND_MANAGER_H
