#include "BleServerManager.h"

BleServerManager::BleServerManager() 
    : pServer(nullptr), pCharacteristic(nullptr), deviceConnected(false) {
}

BleServerManager::~BleServerManager() {
    if (pServer) {
        NimBLEDevice::getAdvertising()->stop();
    }
}

bool BleServerManager::begin() {
    // Initialize BLE Device
    NimBLEDevice::init(BLE_SERVER_NAME);
    
    // Create BLE Server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));
    
    // Create BLE Service
    NimBLEService* pService = pServer->createService(BLE_SERVICE_UUID);
    
    // Create BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        BLE_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::WRITE |
        NIMBLE_PROPERTY::NOTIFY
    );
    
    // Set characteristic callbacks
    pCharacteristic->setCallbacks(new CharacteristicCallbacks(this));
    
    // Start the service
    pService->start();
    
    // Start advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    // Mimic the original timing if needed, though NimBLE is more efficient
    pAdvertising->start();
    
    Serial.println("NimBLE Server started, waiting for client connection...");
    return true;
}

bool BleServerManager::sendData(const String& data) {
    if (!deviceConnected) {
        Serial.println("No client connected, cannot send data");
        return false;
    }
    
    if (data.length() > BLE_MAX_DATA_LENGTH) {
        Serial.println("Data too long, truncating...");
        String truncated = data.substring(0, BLE_MAX_DATA_LENGTH);
        pCharacteristic->setValue((uint8_t*)truncated.c_str(), truncated.length());
    } else {
        pCharacteristic->setValue((uint8_t*)data.c_str(), data.length());
    }
    
    pCharacteristic->notify();
    return true;
}

bool BleServerManager::isClientConnected() {
    return deviceConnected;
}

void BleServerManager::setDataReceivedCallback(DataReceivedCallback callback) {
    dataReceivedCallback = callback;
}

// Server Callbacks Implementation
void BleServerManager::ServerCallbacks::onConnect(NimBLEServer* pServer) {
    manager->deviceConnected = true;
    Serial.println("Client connected!");
}

void BleServerManager::ServerCallbacks::onDisconnect(NimBLEServer* pServer) {
    manager->deviceConnected = false;
    Serial.println("Client disconnected!");
    
    // Restart advertising
    pServer->getAdvertising()->start();
    Serial.println("Advertising restarted, waiting for client connection...");
}

// Characteristic Callbacks Implementation
void BleServerManager::CharacteristicCallbacks::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
        String receivedData = String(value.c_str());
        Serial.print("Received from client: ");
        Serial.println(receivedData);
        
        // Call user callback if set
        if (manager->dataReceivedCallback) {
            manager->dataReceivedCallback(receivedData);
        }
    }
}
