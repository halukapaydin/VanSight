#include "InfraredManager.h"
#include "Arduino.h"
#include <IRremote.h>

InfraredManager::InfraredManager()
    : _irPin(0), _callback(nullptr) {}

InfraredManager& InfraredManager::getInstance() {
    static InfraredManager instance;
    return instance;
}

void InfraredManager::begin(uint8_t irPin) {
    _irPin = irPin;

    IrReceiver.begin(_irPin, ENABLE_LED_FEEDBACK);

    Serial.println("[IR] Infrared receiver started");
}

void InfraredManager::onReceive(IrReceiveCallback callback) {
    _callback = callback;
}

void InfraredManager::loop() {
    if (IrReceiver.decode()) {
        uint32_t value = IrReceiver.decodedIRData.decodedRawData;

        if (_callback) {
            _callback(value);
        }

        IrReceiver.resume();
    }
}
