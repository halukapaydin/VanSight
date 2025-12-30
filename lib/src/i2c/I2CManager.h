//
// Created by haluk on 12/27/25.
//

#ifndef LIB_I2CMANAGER_H
#define LIB_I2CMANAGER_H



#pragma once
#include <Arduino.h>
#include <Wire.h>

typedef void (*I2CDataCallback)(const String& data);

class I2CManager {
public:
    I2CManager(uint8_t address);

    void beginMaster();
    void beginSlave();

#if defined(ESP32) || defined(ESP8266)
    void beginMaster(int sda, int scl);
    void beginSlave(int sda, int scl);
#endif

    void onDataReceived(I2CDataCallback callback);
    bool sendData(uint8_t targetAddress, const String& json);

private:
    static void onReceiveService(int len);
    void handleReceive(int len);

    static I2CManager* instance;

    uint8_t _address;
    I2CDataCallback _callback;
    String _rxBuffer;
};


#endif //LIB_I2CMANAGER_H