#include "BleManager.h"

namespace VanSight {

// Static instance
BleManager* BleManager::_instance = nullptr;

BleManager::BleManager(BleRole role, const char* deviceName)
    : _role(role),
      _deviceName(deviceName),
      _initialized(false),
      _connected(false),
      _verbose(true),
      _server(nullptr),
      _service(nullptr),
      _commandChar(nullptr),
      _responseChar(nullptr),
      _client(nullptr),
      _remoteService(nullptr),
      _remoteCommandChar(nullptr),
      _remoteResponseChar(nullptr),
      _serverDevice(nullptr),
      _dataCallback(nullptr),
      _connectionCallback(nullptr)
{
    _instance = this;
}

BleManager::~BleManager()
{
    if (_client) {
        delete _client;
    }
}

bool BleManager::begin()
{
    if (_initialized) {
        return true;
    }
    
    log("[BLE] Initializing as %s: %s", 
        _role == BleRole::SERVER ? "SERVER" : "CLIENT", _deviceName);
    
    // Initialize BLE Device
    BLEDevice::init(_deviceName);
    
    if (_role == BleRole::SERVER) {
        if (!initServer()) {
            log("[BLE] Server initialization failed");
            return false;
        }
    } else {
        if (!initClient()) {
            log("[BLE] Client initialization failed");
            return false;
        }
    }
    
    _initialized = true;
    log("[BLE] Initialization complete");
    return true;
}

bool BleManager::initServer()
{
    // Create BLE Server
    _server = BLEDevice::createServer();
    _server->setCallbacks(new ServerCallbacks(this));
    
    // Set MTU size for larger messages
    BLEDevice::setMTU(512);
    
    // Create Service
    _service = _server->createService(VANSIGHT_SERVICE_UUID);
    
    // Create Command Characteristic (Write)
    _commandChar = _service->createCharacteristic(
        COMMAND_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    _commandChar->setCallbacks(new CommandCharCallbacks(this));
    
    // Create Response Characteristic (Notify)
    _responseChar = _service->createCharacteristic(
        RESPONSE_CHAR_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    _responseChar->addDescriptor(new BLE2902());
    
    // Start service
    _service->start();
    
    // Start advertising
    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(VANSIGHT_SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);
    advertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    log("[BLE] Server started, advertising...");
    return true;
}

bool BleManager::initClient()
{
    // Create BLE Client
    _client = BLEDevice::createClient();
    _client->setClientCallbacks(new ClientCallbacks(this));
    
    // Start scanning
    BLEScan* scan = BLEDevice::getScan();
    // Don't use callbacks for sync scanning
    // scan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(this));
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);
    
    log("[BLE] Client initialized, scanning for server...");
    
    // Scan synchronously for 10 seconds
    BLEScanResults foundDevices = scan->start(10, false);
    int count = foundDevices.getCount();
    log("[BLE] Scan complete, found %d devices", count);
    
    // Manually check each device
    for (int i = 0; i < count; i++) {
        BLEAdvertisedDevice device = foundDevices.getDevice(i);
        log("[BLE] Device %d: %s", i, device.toString().c_str());
        
        if (device.haveServiceUUID() && 
            device.isAdvertisingService(BLEUUID(VANSIGHT_SERVICE_UUID))) {
            log("[BLE] Found VanSight server!");
            _serverDevice = new BLEAdvertisedDevice(device);
            connectToServer();
            break;
        }
    }
    
    if (!_serverDevice) {
        log("[BLE] VanSight server not found");
    }
    
    return true;
}

bool BleManager::connectToServer()
{
    if (!_serverDevice) {
        log("[BLE] No server device found");
        return false;
    }
    
    log("[BLE] Connecting to server...");
    
    if (!_client->connect(_serverDevice)) {
        log("[BLE] Connection failed");
        return false;
    }
    
    log("[BLE] Connected! Setting MTU...");
    
    // Request larger MTU for bigger messages
    uint16_t mtu = _client->setMTU(512);
    delay(200); // Wait for MTU negotiation
    
    // Get actual MTU
    uint16_t actualMtu = _client->getMTU();
    log("[BLE] MTU requested: 512, actual: %d", actualMtu);
    
    log("[BLE] Getting service...");
    
    // Get service
    _remoteService = _client->getService(VANSIGHT_SERVICE_UUID);
    if (!_remoteService) {
        log("[BLE] Service not found");
        _client->disconnect();
        return false;
    }
    
    // Get characteristics
    _remoteCommandChar = _remoteService->getCharacteristic(COMMAND_CHAR_UUID);
    _remoteResponseChar = _remoteService->getCharacteristic(RESPONSE_CHAR_UUID);
    
    if (!_remoteCommandChar || !_remoteResponseChar) {
        log("[BLE] Characteristics not found");
        _client->disconnect();
        return false;
    }
    
    // Register for notifications
    if (_remoteResponseChar->canNotify()) {
        _remoteResponseChar->registerForNotify(notifyCallback);
    }
    
    _connected = true;
    handleConnectionChange(true);
    
    log("[BLE] Setup complete!");
    return true;
}

bool BleManager::sendData(const uint8_t* data, size_t len)
{
    if (!_initialized) {
        log("[BLE] Not initialized");
        return false;
    }
    
    if (_role == BleRole::SERVER) {
        // Server sends via Response characteristic
        if (!_connected) {
            log("[BLE] No client connected");
            return false;
        }
        
        // BLE notify has MTU limit (typically 20-512 bytes)
        // For now, send all at once and let BLE stack handle it
        _responseChar->setValue((uint8_t*)data, len);
        _responseChar->notify();
        log("[BLE] TX (notify): %d bytes", len);
        return true;
    } else {
        // Client sends via Command characteristic
        if (!_connected || !_remoteCommandChar) {
            log("[BLE] Not connected to server");
            return false;
        }
        
        // BLE write can handle larger packets
        _remoteCommandChar->writeValue((uint8_t*)data, len);
        log("[BLE] TX (write): %d bytes", len);
        return true;
    }
}

void BleManager::onDataReceived(std::function<void(const uint8_t*, size_t)> callback)
{
    _dataCallback = callback;
}

void BleManager::onConnectionChanged(std::function<void(bool)> callback)
{
    _connectionCallback = callback;
}

void BleManager::handleConnectionChange(bool connected)
{
    _connected = connected;
    log("[BLE] Connection %s", connected ? "ESTABLISHED" : "LOST");
    
    if (_connectionCallback) {
        _connectionCallback(connected);
    }
    
    // Auto-reconnect for client
    if (!connected && _role == BleRole::CLIENT) {
        log("[BLE] Attempting reconnect in 5s...");
        delay(5000);
        BLEDevice::getScan()->start(5, false);
    }
}

void BleManager::log(const char* format, ...)
{
    if (!_verbose) return;
    
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Serial.println(buffer);
}

// ============================================================================
// Server Callbacks
// ============================================================================

void BleManager::ServerCallbacks::onConnect(BLEServer* server)
{
    _manager->handleConnectionChange(true);
}

void BleManager::ServerCallbacks::onDisconnect(BLEServer* server)
{
    _manager->handleConnectionChange(false);
    // Restart advertising
    BLEDevice::startAdvertising();
    _manager->log("[BLE] Restarted advertising");
}

void BleManager::CommandCharCallbacks::onWrite(BLECharacteristic* characteristic)
{
    std::string value = characteristic->getValue();
    if (value.length() > 0 && _manager->_dataCallback) {
        _manager->log("[BLE] RX: %d bytes", value.length());
        _manager->_dataCallback((uint8_t*)value.data(), value.length());
    }
}

// ============================================================================
// Client Callbacks
// ============================================================================

void BleManager::AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    _manager->log("[BLE] Device found: %s", advertisedDevice.toString().c_str());
    
    if (advertisedDevice.haveServiceUUID() && 
        advertisedDevice.isAdvertisingService(BLEUUID(VANSIGHT_SERVICE_UUID))) {
        
        _manager->log("[BLE] Found VanSight server!");
        BLEDevice::getScan()->stop();
        _manager->_serverDevice = new BLEAdvertisedDevice(advertisedDevice);
        _manager->connectToServer();
    } else {
        _manager->log("[BLE] Not VanSight server (no matching service UUID)");
    }
}

void BleManager::ClientCallbacks::onConnect(BLEClient* client)
{
    _manager->log("[BLE] Client connected callback");
}

void BleManager::ClientCallbacks::onDisconnect(BLEClient* client)
{
    _manager->handleConnectionChange(false);
}

void BleManager::notifyCallback(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t len, bool isNotify)
{
    if (_instance && _instance->_dataCallback) {
        _instance->log("[BLE] RX (notify): %d bytes", len);
        _instance->_dataCallback(data, len);
    }
}

} // namespace VanSight
