#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <functional>
#include "../protocol/VanSightProtocol.h"
#include "../protocol/CommandParser.h"
#include "../protocol/ResponseBuilder.h"
#include "../config/VanSightConfig.h"

namespace VanSight
{
    /**
     * @brief ESP-NOW role configuration
     */
    enum class ESPNowRole
    {
        SERVER, // Receives commands, sends responses (Hub)
        CLIENT // Sends commands, receives responses (DisplayClient)
    };

    /**
     * @brief Universal ESP-NOW Manager
     *
     * Handles all ESP-NOW communication for both Server and Client roles.
     * Automatically configures WiFi, manages peers, and handles message routing.
     */
    class ESPNowManager
    {
    public:
        /**
         * @brief Constructor
         * @param role Server or Client mode
         * @param peerMac Peer MAC address (required for Client, optional for Server)
         * @param channel WiFi channel (default: 1)
         */
        ESPNowManager(ESPNowRole role, const uint8_t* peerMac = nullptr, uint8_t channel = 1);

        /**
         * @brief Initialize ESP-NOW
         * @return true if successful
         */
        bool begin();

        /**
         * @brief Check if initialized
         */
        bool isInitialized() const { return _initialized; }

        /**
         * @brief Send command (Client mode)
         * @param cmd Command to send
         * @return true if sent successfully
         */
        bool sendCommand(const Command& cmd);

        /**
         * @brief Send response (Server mode)
         * @param response Response to send
         * @param targetMac Target MAC address (uses last sender if nullptr)
         * @return true if sent successfully
         */
        bool sendResponse(const Response& response, const uint8_t* targetMac = nullptr);

        /**
         * @brief Broadcast response to all registered clients (Server mode)
         * @param response Response to broadcast
         * @return Number of clients successfully sent to
         */
        int broadcastResponse(const Response& response);

        /**
         * @brief Register callback for received commands (Server mode)
         * @param callback Function to call when command received
         */
        void onCommandReceived(std::function<void(const Command &, const uint8_t *)> callback);

        /**
         * @brief Register callback for received responses (Client mode)
         * @param callback Function to call when response received
         */
        void onResponseReceived(std::function<void(const Response &)> callback);

        /**
         * @brief Add a peer
         * @param mac Peer MAC address
         * @return true if added successfully
         */
        bool addPeer(const uint8_t* mac);

        /**
         * @brief Get number of registered peers
         */
        int getPeerCount() const { return _peerCount; }

        /**
         * @brief Remove a peer
         * @param mac Peer MAC address
         * @return true if removed successfully
         */
        bool removePeer(const uint8_t* mac);

        /**
         * @brief Set WiFi channel
         * @param channel Channel number (1-13)
         */
        void setChannel(uint8_t channel);

        /**
         * @brief Set retry count for failed sends
         * @param count Number of retries (default: 3)
         */
        void setRetryCount(uint8_t count);

        /**
         * @brief Enable/disable verbose logging
         */
        void setVerbose(bool verbose) { _verbose = verbose; }

    private:
        // Configuration
        ESPNowRole _role;
        uint8_t _channel;
        uint8_t _retryCount;
        bool _verbose;
        bool _initialized;

        // Peer management
        uint8_t _peerMac[6];
        uint8_t _lastSenderMac[6];
        uint8_t _peerList[ESP_NOW_MAX_TOTAL_PEER_NUM][6]; // List of all registered peers
        int _peerCount;

        // Callbacks
        std::function<void(const Command &, const uint8_t *)> _commandCallback;
        std::function<void(const Response &)> _responseCallback;

        // ESP-NOW callbacks (static for C API)
        static void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
        static void onDataRecv(const uint8_t* mac, const uint8_t* data, int len);

        // Singleton instance for callbacks
        static ESPNowManager* _instance;

        // Internal methods
        void handleDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
        void handleDataRecv(const uint8_t* mac, const uint8_t* data, int len);
        bool initWiFi();
        bool initESPNow();
        bool sendData(const uint8_t* data, size_t len, const uint8_t* targetMac);
        void log(const char* format, ...);
    };
}
#endif // ESPNOW_MANAGER_H
