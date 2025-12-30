#ifndef BLE_SERVER_MANAGER_H
#define BLE_SERVER_MANAGER_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "BleConfig.h"

// Callback type for data received from client
typedef std::function<void(String)> DataReceivedCallback;

class BleServerManager {
public:
    BleServerManager();
    ~BleServerManager();
    
    // Initialize BLE Server and start advertising
    bool begin();
    
    // Send data to all connected clients
    bool sendData(const String& data);
    
    // Check if any client is connected
    bool isClientConnected();
    
    // Set callback for when data is received from client
    void setDataReceivedCallback(DataReceivedCallback callback);
    
private:
    NimBLEServer* pServer;
    NimBLECharacteristic* pCharacteristic;
    bool deviceConnected;
    DataReceivedCallback dataReceivedCallback;
    
    // Server callbacks
    class ServerCallbacks : public BLEServerCallbacks {
    public:
        ServerCallbacks(BleServerManager* manager) : manager(manager) {}
        void onConnect(NimBLEServer* pServer);
        void onDisconnect(NimBLEServer* pServer);
    private:
        BleServerManager* manager;
    };
    
    // Characteristic callbacks
    class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    public:
        CharacteristicCallbacks(BleServerManager* manager) : manager(manager) {}
        void onWrite(NimBLECharacteristic* pCharacteristic);
    private:
        BleServerManager* manager;
    };
    
    friend class ServerCallbacks;
    friend class CharacteristicCallbacks;
};

#endif // BLE_SERVER_MANAGER_H
