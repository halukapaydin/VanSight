#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include <Arduino.h>

class BuzzerManager {
public:
    static BuzzerManager& getInstance() {
        static BuzzerManager instance;
        return instance;
    }

    void begin(uint8_t pin = 4);
    void beep(uint16_t duration = 50);
    void beepPattern(uint8_t count = 1, uint16_t duration = 50, uint16_t pause = 100);

private:
    BuzzerManager() : _pin(0), _initialized(false) {}
    BuzzerManager(const BuzzerManager&) = delete;
    BuzzerManager& operator=(const BuzzerManager&) = delete;

    uint8_t _pin;
    bool _initialized;
};

#endif // BUZZER_MANAGER_H
