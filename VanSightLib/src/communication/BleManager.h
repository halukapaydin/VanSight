#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEClient.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <functional>

namespace VanSight {

// BLE UUIDs
#define VANSIGHT_SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define COMMAND_CHAR_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define RESPONSE_CHAR_UUID    "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"

enum class BleRole {
    SERVER,  // Hub
    CLIENT   // DisplayClient
};

/**
 * @brief BLE Manager for VanSight communication
 * 
 * Handles BLE GATT server/client operations for reliable communication
 */
class BleManager {
public:
    /**
     * @brief Construct BLE Manager
     * @param role Server or Client mode
     * @param deviceName BLE device name
     */
    BleManager(BleRole role, const char* deviceName);
    ~BleManager();
    
    /**
     * @brief Initialize BLE
     * @return true if successful
     */
    bool begin();
    
    /**
     * @brief Check if initialized
     */
    bool isInitialized() const { return _initialized; }
    
    /**
     * @brief Check if connected (Client mode)
     */
    bool isConnected() const { return _connected; }
    
    /**
     * @brief Send data
     * @param data Data buffer
     * @param len Data length
     * @return true if sent successfully
     */
    bool sendData(const uint8_t* data, size_t len);
    
    /**
     * @brief Register data received callback
     */
    void onDataReceived(std::function<void(const uint8_t*, size_t)> callback);
    
    /**
     * @brief Register connection state callback
     */
    void onConnectionChanged(std::function<void(bool connected)> callback);
    
    /**
     * @brief Get device name
     */
    const char* getDeviceName() const { return _deviceName; }
    
    /**
     * @brief Enable/disable verbose logging
     */
    void setVerbose(bool verbose) { _verbose = verbose; }

private:
    BleRole _role;
    const char* _deviceName;
    bool _initialized;
    bool _connected;
    bool _verbose;
    
    // Server mode
    BLEServer* _server;
    BLEService* _service;
    BLECharacteristic* _commandChar;
    BLECharacteristic* _responseChar;
    
    // Client mode
    BLEClient* _client;
    BLERemoteService* _remoteService;
    BLERemoteCharacteristic* _remoteCommandChar;
    BLERemoteCharacteristic* _remoteResponseChar;
    BLEAdvertisedDevice* _serverDevice;
    
    // Callbacks
    std::function<void(const uint8_t*, size_t)> _dataCallback;
    std::function<void(bool)> _connectionCallback;
    
    // Server callbacks
    class ServerCallbacks : public BLEServerCallbacks {
    public:
        ServerCallbacks(BleManager* manager) : _manager(manager) {}
        void onConnect(BLEServer* server) override;
        void onDisconnect(BLEServer* server) override;
    private:
        BleManager* _manager;
    };
    
    class CommandCharCallbacks : public BLECharacteristicCallbacks {
    public:
        CommandCharCallbacks(BleManager* manager) : _manager(manager) {}
        void onWrite(BLECharacteristic* characteristic) override;
    private:
        BleManager* _manager;
    };
    
    // Client callbacks
    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    public:
        AdvertisedDeviceCallbacks(BleManager* manager) : _manager(manager) {}
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    private:
        BleManager* _manager;
    };
    
    class ClientCallbacks : public BLEClientCallbacks {
    public:
        ClientCallbacks(BleManager* manager) : _manager(manager) {}
        void onConnect(BLEClient* client) override;
        void onDisconnect(BLEClient* client) override;
    private:
        BleManager* _manager;
    };
    
    // Notification callback
    static void notifyCallback(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t len, bool isNotify);
    static BleManager* _instance;
    
    // Internal methods
    bool initServer();
    bool initClient();
    bool connectToServer();
    void handleConnectionChange(bool connected);
    void log(const char* format, ...);
    
    friend class ServerCallbacks;
    friend class CommandCharCallbacks;
    friend class AdvertisedDeviceCallbacks;
    friend class ClientCallbacks;
};

} // namespace VanSight

#endif // BLE_MANAGER_H
