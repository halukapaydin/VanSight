#include "UartManager.h"

#if defined(ESP32)

UartManager::UartManager(HardwareSerial& serial) {
    _serial = &serial;
}

void UartManager::start(int rxPin, int txPin, uint32_t baudRate) {
    _serial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
}

#else

UartManager::UartManager(Stream& serial) {
    _serial = &serial;
}

void UartManager::start(uint32_t baudRate) {
    if (_serial == &Serial) {
        Serial.begin(baudRate);
    }
#if defined(ARDUINO_AVR_MEGA2560)
    if (_serial == &Serial1) {
        Serial1.begin(baudRate);
    }
#endif
}

#endif

void UartManager::onDataReceived(UartDataCallback cb) {
    _callback = cb;
}

void UartManager::sendData(const String& json) {
    _serial->print(json);
    _serial->print('\n');
}

void UartManager::loop() {
    while (_serial->available()) {
        char c = _serial->read();
        Serial.print(c);
        if (c == '\n') {
            if (_callback && _rxBuffer.length()) {
                _callback(_rxBuffer);
            }
            _rxBuffer = "";
        } else {
            _rxBuffer += c;
            if (_rxBuffer.length() > 512) {
                _rxBuffer = "";
            }
        }
    }
}
