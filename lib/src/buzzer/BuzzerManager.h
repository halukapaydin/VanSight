//
// Created by haluk on 12/27/25.
//

#ifndef LIB_BUZZERMANAGER_H
#define LIB_BUZZERMANAGER_H


#include <Arduino.h>

class BuzzerManager {
public:
    explicit BuzzerManager(uint8_t pin);

    void begin();
    void beep(unsigned int frequency, unsigned int duration);

private:
    uint8_t _pin;
};

#endif //LIB_BUZZERMANAGER_H