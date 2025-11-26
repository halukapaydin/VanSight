#ifndef BLE_COMMAND_MANAGER_H
#define BLE_COMMAND_MANAGER_H

#include <Arduino.h>
#include <functional>
#include "BleManager.h"
#include "../protocol/VanSightProtocol.h"
#include "../config/VanSightConfig.h"

namespace VanSight {

/**
 * @brief BLE Command Manager - High-level API for BLE communication
 * 
 * Provides simple methods like toggleRelay(), allRelaysOff(), etc.
 * Uses BleManager for BLE communication.
 */
class BleCommandManager {
public:
    /**
     * @brief Get singleton instance
     */
    static BleCommandManager& getInstance();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * @brief Initialize as Server (Hub)
     * @param deviceName BLE device name
     * @return true if successful
     */
    bool beginServer(const char* deviceName = "VanSightHub");
    
    /**
     * @brief Initialize as Client (DisplayClient)
     * @param serverName Server device name to connect to
     * @return true if successful
     */
    bool beginClient(const char* serverName = "VanSightHub");
    
    /**
     * @brief Check if initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Check if connected
     */
    bool isConnected() const;
    
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
    
    /**
     * @brief Register callback for connection state
     */
    void onConnectionChanged(std::function<void(bool connected)> callback);
    
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
    // SERVER MODE - SEND UPDATES
    // ========================================================================
    
    /**
     * @brief Send relay state to client
     */
    void sendRelayState(uint8_t relayNum, bool state);
    
    /**
     * @brief Send all status to client
     */
    void sendAllStatus(const bool relayStates[MAX_RELAYS], const int sensorLevels[MAX_SENSORS]);

private:
    BleCommandManager();
    ~BleCommandManager();
    
    // Prevent copying
    BleCommandManager(const BleCommandManager&) = delete;
    BleCommandManager& operator=(const BleCommandManager&) = delete;
    
    BleManager* _ble;
    BleRole _role;
    bool _initialized;
    
    // Client callbacks
    std::function<void(const AllStatusData&)> _dataReceivedCallback;
    std::function<void(uint8_t, bool)> _relayChangedCallback;
    std::function<void(bool)> _connectionCallback;
    
    // Buffer for packet reassembly
    String _rxBuffer;
    
    // Server handlers
    std::function<bool(uint8_t)> _toggleRelayHandler;
    std::function<void()> _allRelaysOffHandler;
    std::function<AllStatusData()> _statusRequestHandler;
    
    // Internal handlers
    void handleCommand(const Command& cmd);
    void handleResponse(const Response& response);
    void handleData(const uint8_t* data, size_t len);
    
    // Helper methods
    Response createRelayResponse(uint8_t relayNum, bool state);
    Response createAllStatusResponse(const AllStatusData& data);
};

} // namespace VanSight

#endif // BLE_COMMAND_MANAGER_H
