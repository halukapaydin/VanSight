#include "BuzzerManager.h"

BuzzerManager::BuzzerManager(uint8_t pin) : _pin(pin) {}

void BuzzerManager::begin() {
    pinMode(_pin, OUTPUT);
    noTone(_pin);
}

void BuzzerManager::beep(unsigned int frequency, unsigned int duration) {
    tone(_pin, frequency);
    delay(duration);
    noTone(_pin);
}
