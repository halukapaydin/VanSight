//
// Created by haluk on 12/27/25.
//

#ifndef LIB_UARTMANAGER_H
#define LIB_UARTMANAGER_H
#pragma once
#include <Arduino.h>

typedef void (*UartDataCallback)(String data);

class UartManager {
public:
#if defined(ESP32)
    UartManager(HardwareSerial& serial);
    void start(int rxPin, int txPin, uint32_t baudRate = 115200);
#else
    UartManager(Stream& serial);
    void start(uint32_t baudRate = 115200);
#endif

    void loop();
    void sendData(const String& json);
    void onDataReceived(UartDataCallback cb);

private:
#if defined(ESP32)
    HardwareSerial* _serial;
#else
    Stream* _serial;
#endif

    String _rxBuffer;
    UartDataCallback _callback = nullptr;
};


#endif //LIB_UARTMANAGER_H