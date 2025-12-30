#include "I2CManager.h"

I2CManager* I2CManager::instance = nullptr;

I2CManager::I2CManager(uint8_t address)
    : _address(address), _callback(nullptr) {
    instance = this;
}

// ---------------- MASTER ----------------
void I2CManager::beginMaster() {
    Wire.begin();
}

#if defined(ESP32) || defined(ESP8266)
void I2CManager::beginMaster(int sda, int scl) {
    Wire.begin(sda, scl);
}
#endif

// ---------------- SLAVE ----------------
void I2CManager::beginSlave() {
    Wire.begin(_address);
    Wire.onReceive(onReceiveService);
}

#if defined(ESP32) || defined(ESP8266)
void I2CManager::beginSlave(int sda, int scl) {
    Wire.begin(_address, sda, scl);
    Wire.onReceive(onReceiveService);
}
#endif

// ---------------- CALLBACK ----------------
void I2CManager::onDataReceived(I2CDataCallback callback) {
    _callback = callback;
}

// ---------------- SEND ----------------
bool I2CManager::sendData(uint8_t targetAddress, const String& json) {
    uint16_t len = json.length();
    uint16_t offset = 0;

    while (offset < len) {
        Wire.beginTransmission(targetAddress);

        uint8_t chunkSize = min((uint16_t)24, (uint16_t)(len - offset));

        Wire.write((uint8_t)(chunkSize >> 8));
        Wire.write((uint8_t)(chunkSize & 0xFF));

        for (uint8_t i = 0; i < chunkSize; i++) {
            Wire.write(json[offset + i]);
        }

        if (Wire.endTransmission() != 0) {
            return false;
        }

        offset += chunkSize;
        delay(5);
    }

    return true;
}

// ---------------- RECEIVE ----------------
void I2CManager::onReceiveService(int len) {
    if (instance) {
        instance->handleReceive(len);
    }
}

void I2CManager::handleReceive(int len) {
    if (len < 2) return;

    uint16_t size = (Wire.read() << 8);
    size |= Wire.read();

    while (Wire.available() && size--) {
        _rxBuffer += (char)Wire.read();
    }

    if (_callback && size == 0) {
        _callback(_rxBuffer);
        _rxBuffer = "";
    }
}
