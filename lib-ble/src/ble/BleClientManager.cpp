#include "BleClientManager.h"

// Static instance for callback
BleClientManager* BleClientManager::instance = nullptr;

BleClientManager::BleClientManager() 
    : pClient(nullptr), pRemoteCharacteristic(nullptr), targetDevice(nullptr),
      state(BLE_DISCONNECTED), lastReconnectAttempt(0) {
    instance = this;
}

BleClientManager::~BleClientManager() {
    disconnect();
}

bool BleClientManager::begin() {
    // Initialize BLE Device
    NimBLEDevice::init(BLE_CLIENT_NAME);
    
    Serial.println("NimBLE Client initialized");
    state = BLE_DISCONNECTED;
    
    // Start initial connection attempt
    return scanAndConnect();
}

void BleClientManager::loop() {
    // Auto-reconnection logic
    if (state == BLE_DISCONNECTED) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt >= BLE_RECONNECT_INTERVAL) {
            lastReconnectAttempt = now;
            Serial.println("Attempting to reconnect...");
            scanAndConnect();
        }
    }
    
    // Check connection health
    if (state == BLE_CONNECTED && pClient && !pClient->isConnected()) {
        Serial.println("Connection lost, switching to disconnected state");
        state = BLE_DISCONNECTED;
    }
}

bool BleClientManager::sendData(const String& data) {
    if (state != BLE_CONNECTED || !pRemoteCharacteristic) {
        Serial.println("Not connected, cannot send data");
        return false;
    }
    
    if (data.length() > BLE_MAX_DATA_LENGTH) {
        Serial.println("Data too long, truncating...");
        String truncated = data.substring(0, BLE_MAX_DATA_LENGTH);
        return pRemoteCharacteristic->writeValue(truncated.c_str(), truncated.length());
    } else {
        return pRemoteCharacteristic->writeValue(data.c_str(), data.length());
    }
}

bool BleClientManager::isConnected() {
    return state == BLE_CONNECTED;
}

void BleClientManager::setDataReceivedCallback(DataReceivedCallback callback) {
    dataReceivedCallback = callback;
}
void BleClientManager::setClientStateCallback(ClientStateCallback callback) {
    clientStateCallback = callback;
}

BleClientState BleClientManager::getState() {
    return state;
}

void BleClientManager::setState(BleClientState state) {
    this->state = state;
    if (this->clientStateCallback) {
        this->clientStateCallback(state);
    }
}

bool BleClientManager::scanAndConnect() {
    setState(BLE_SCANNING);
    Serial.println("Starting NimBLE scan...");
    
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(this));
    pScan->setActiveScan(true);
    pScan->setInterval(100);
    pScan->setWindow(99);
    
    pScan->start(BLE_SCAN_DURATION, false);
    
    if (targetDevice != nullptr) {
        return connectToServer();
    } else {
        Serial.println("Target server not found");
        setState(BLE_DISCONNECTED);
        return false;
    }
}

bool BleClientManager::connectToServer() {
    if (!targetDevice) {
        return false;
    }
    
    setState(BLE_CONNECTING);
    Serial.print("Connecting to ");
    Serial.println(targetDevice->getAddress().toString().c_str());
    
    // Create client
    if (!pClient) {
        pClient = NimBLEDevice::createClient();
        pClient->setClientCallbacks(new ClientCallbacks(this));
    }
    
    // Connect to server
    if (!pClient->connect(targetDevice)) {
        Serial.println("Failed to connect to server");
        setState(BLE_DISCONNECTED);
        return false;
    }
    
    Serial.println("Connected to server");
    
    // Obtain reference to service
    NimBLERemoteService* pRemoteService = pClient->getService(BLE_SERVICE_UUID);
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find service UUID: ");
        Serial.println(BLE_SERVICE_UUID);
        pClient->disconnect();
        setState(BLE_DISCONNECTED);
        return false;
    }
    
    // Obtain reference to characteristic
    pRemoteCharacteristic = pRemoteService->getCharacteristic(BLE_CHARACTERISTIC_UUID);
    if (pRemoteCharacteristic == nullptr) {
        Serial.print("Failed to find characteristic UUID: ");
        Serial.println(BLE_CHARACTERISTIC_UUID);
        pClient->disconnect();
        setState(BLE_DISCONNECTED);
        return false;
    }
    
    // Register for notifications
    if (pRemoteCharacteristic->canNotify()) {
        if (!pRemoteCharacteristic->subscribe(true, notifyCallback)) {
            Serial.println("Failed to subscribe to notifications");
        }
    }
    
    setState(BLE_CONNECTED);
    Serial.println("Successfully connected and ready!");
    return true;
}

void BleClientManager::disconnect() {
    if (pClient && pClient->isConnected()) {
        pClient->disconnect();
    }
    setState(BLE_DISCONNECTED);
}

// Advertised Device Callbacks Implementation
void BleClientManager::AdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice* advertisedDevice) {
    Serial.print("Found device: ");
    Serial.println(advertisedDevice->toString().c_str());
    
    // Check if device has our service UUID
    if (advertisedDevice->haveServiceUUID() && 
        advertisedDevice->isAdvertisingService(NimBLEUUID(BLE_SERVICE_UUID))) {
        
        Serial.println("Found target server!");
        NimBLEDevice::getScan()->stop();
        
        // Store device for connection
        if (manager->targetDevice) {
            delete manager->targetDevice;
        }
        manager->targetDevice = new NimBLEAdvertisedDevice(*advertisedDevice);
    }
}

// Client Callbacks Implementation
void BleClientManager::ClientCallbacks::onConnect(NimBLEClient* pClient) {
    Serial.println("Client callback: Connected");
}

void BleClientManager::ClientCallbacks::onDisconnect(NimBLEClient* pClient) {
    Serial.println("Client callback: Disconnected");
    manager->setState(BLE_DISCONNECTED);
}

// Notification Callback Implementation
void BleClientManager::notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
                                      uint8_t* pData, size_t length, bool isNotify) {
    if (instance && length > 0) {
        std::string strData((char*)pData, length);
        String receivedData = String(strData.c_str());
        Serial.print("Received from server: ");
        Serial.println(receivedData);
        
        // Call user callback if set
        if (instance->dataReceivedCallback) {
            instance->dataReceivedCallback(receivedData);
        }
    }
}
