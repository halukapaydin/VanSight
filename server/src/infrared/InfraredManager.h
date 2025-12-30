//
// Created by haluk on 12/30/25.
//

#ifndef SERVER_INFRAREDMANAGER_H
#define SERVER_INFRAREDMANAGER_H


#include <Arduino.h>

constexpr uint32_t IR_BUTTON_NUMBER_0 = 0xE619FF00;
constexpr uint32_t IR_BUTTON_NUMBER_1 = 0xBA45FF00;
constexpr uint32_t IR_BUTTON_NUMBER_2 = 0xB946FF00;
constexpr uint32_t IR_BUTTON_NUMBER_3 = 0xB847FF00;
constexpr uint32_t   IR_BUTTON_NUMBER_4 = 0xBB44FF00;
constexpr uint32_t  IR_BUTTON_NUMBER_5 = 0xBF40FF00;
constexpr uint32_t  IR_BUTTON_NUMBER_6 = 0xBC43FF00;
constexpr uint32_t  IR_BUTTON_NUMBER_7 = 0xF807FF00;
constexpr uint32_t  IR_BUTTON_NUMBER_8 = 0xEA15FF00;
constexpr uint32_t  IR_BUTTON_NUMBER_9 = 0xF609FF00;
constexpr uint32_t  IR_BUTTON_STAR = 0xE916FF00;
constexpr uint32_t  IR_BUTTON_HASH = 0xF20DFF00;
constexpr uint32_t  IR_BUTTON_OK = 0xE31CFF00;
constexpr uint32_t  IR_BUTTON_UP = 0xE718FF00;
constexpr uint32_t  IR_BUTTON_DOWN = 0xAD52FF00;
constexpr uint32_t  IR_BUTTON_LEFT = 0xF708FF00;
constexpr uint32_t  IR_BUTTON_RIGHT = 0xA55AFF00;


typedef void (*IrReceiveCallback)(uint32_t code);

class InfraredManager {
public:
    static InfraredManager& getInstance();

    void begin(uint8_t irPin);
    void loop();

    void onReceive(IrReceiveCallback callback);

private:
    InfraredManager();
    InfraredManager(const InfraredManager&) = delete;
    InfraredManager& operator=(const InfraredManager&) = delete;

    uint8_t _irPin;
    IrReceiveCallback _callback;
};

#endif //SERVER_INFRAREDMANAGER_H