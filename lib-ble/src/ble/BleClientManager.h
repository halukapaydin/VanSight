#ifndef BLE_CLIENT_MANAGER_H
#define BLE_CLIENT_MANAGER_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "BleConfig.h"

// Callback type for data received from server
typedef std::function<void(String)> DataReceivedCallback;

// Connection states
enum BleClientState {
    BLE_DISCONNECTED,
    BLE_SCANNING,
    BLE_CONNECTING,
    BLE_CONNECTED
};
typedef std::function<void(BleClientState)> ClientStateCallback;

class BleClientManager {
public:
    BleClientManager();
    ~BleClientManager();
    
    // Initialize BLE Client
    bool begin();
    
    // Must be called in loop() for auto-reconnection
    void loop();
    
    // Send data to server
    bool sendData(const String& data);
    
    // Check if connected to server
    bool isConnected();
    
    // Set callback for when data is received from server
    void setDataReceivedCallback(DataReceivedCallback callback);

    void setClientStateCallback(ClientStateCallback callback);

    // Get current state
    BleClientState getState();

    // Get current state
    void setState(BleClientState state);


private:
    NimBLEClient* pClient;
    NimBLERemoteCharacteristic* pRemoteCharacteristic;
    NimBLEAdvertisedDevice* targetDevice;
    BleClientState state;
    unsigned long lastReconnectAttempt;
    DataReceivedCallback dataReceivedCallback;
    ClientStateCallback clientStateCallback;

    // Internal methods
    bool scanAndConnect();
    bool connectToServer();
    void disconnect();
    
    // Scan callback
    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    public:
        AdvertisedDeviceCallbacks(BleClientManager* manager) : manager(manager) {}
        void onResult(NimBLEAdvertisedDevice* advertisedDevice);
    private:
        BleClientManager* manager;
    };
    
    // Client callbacks
    class ClientCallbacks : public BLEClientCallbacks {
    public:
        ClientCallbacks(BleClientManager* manager) : manager(manager) {}
        void onConnect(NimBLEClient* pClient);
        void onDisconnect(NimBLEClient* pClient);
    private:
        BleClientManager* manager;
    };
    
    // Notification callback
    static void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
                               uint8_t* pData, size_t length, bool isNotify);
    static BleClientManager* instance; // For static callback
    
    friend class AdvertisedDeviceCallbacks;
    friend class ClientCallbacks;
};

#endif // BLE_CLIENT_MANAGER_H
