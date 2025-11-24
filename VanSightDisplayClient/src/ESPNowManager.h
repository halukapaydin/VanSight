#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "espnow_config.h"

// Forward declarations
typedef void (*RelayStatusCallback)(uint8_t relayNum, bool state);
typedef void (*SensorDataCallback)(uint8_t sensorNum, int level);
typedef void (*AllStatusCallback)(bool relayStates[], int sensorLevels[]);

/**
 * @brief ESP-NOW Manager for bidirectional communication with VanSightHub
 * 
 * Singleton class that handles all ESP-NOW communication
 */
class ESPNowManager {
public:
    /**
     * @brief Get singleton instance
     */
    static ESPNowManager& getInstance();
    
    /**
     * @brief Initialize ESP-NOW
     * @return true if successful
     */
    bool init();
    
    /**
     * @brief Send relay toggle command
     * @param relayNum Relay number (1-16)
     */
    void sendRelayToggle(uint8_t relayNum);
    
    /**
     * @brief Send all relays OFF command
     */
    void sendAllRelaysOff();
    
    /**
     * @brief Request complete status update
     */
    void requestAllStatus();
    
    /**
     * @brief Register callback for relay status updates
     */
    void setRelayStatusCallback(RelayStatusCallback callback);
    
    /**
     * @brief Register callback for sensor data updates
     */
    void setSensorDataCallback(SensorDataCallback callback);
    
    /**
     * @brief Register callback for all status updates
     */
    void setAllStatusCallback(AllStatusCallback callback);
    
    /**
     * @brief Check if initialized
     */
    bool isInitialized() const { return _initialized; }

private:
    ESPNowManager();
    ~ESPNowManager();
    
    // Prevent copying
    ESPNowManager(const ESPNowManager&) = delete;
    ESPNowManager& operator=(const ESPNowManager&) = delete;
    
    bool _initialized;
    esp_now_peer_info_t _peerInfo;
    
    // Callbacks
    RelayStatusCallback _relayStatusCallback;
    SensorDataCallback _sensorDataCallback;
    AllStatusCallback _allStatusCallback;
    
    // ESP-NOW callbacks (static for C API)
    static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void onDataReceived(const uint8_t *mac, const uint8_t *data, int len);
    
    // Helper methods
    void sendCommand(const char* cmd, JsonDocument& doc);
    void handleResponse(const uint8_t* data, int len);
    void parseRelayStatus(JsonDocument& doc);
    void parseSensorData(JsonDocument& doc);
    void parseAllStatus(JsonDocument& doc);
};

#endif // ESPNOW_MANAGER_H
