#include "BuzzerManager.h"

void BuzzerManager::begin(uint8_t pin) {
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    _initialized = true;
    
    Serial.printf("[Buzzer] Initialized on pin %d\n", _pin);
}

void BuzzerManager::beep(uint16_t duration) {
    if (!_initialized) {
        Serial.println("[Buzzer] Warning: Not initialized!");
        return;
    }
    
    digitalWrite(_pin, HIGH);
    delay(duration);
    digitalWrite(_pin, LOW);
}

void BuzzerManager::beepPattern(uint8_t count, uint16_t duration, uint16_t pause) {
    if (!_initialized) {
        Serial.println("[Buzzer] Warning: Not initialized!");
        return;
    }
    
    for (uint8_t i = 0; i < count; i++) {
        digitalWrite(_pin, HIGH);
        delay(duration);
        digitalWrite(_pin, LOW);
        
        if (i < count - 1) {
            delay(pause);
        }
    }
}
