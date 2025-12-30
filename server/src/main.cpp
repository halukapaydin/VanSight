//
// Created by haluk on 11/25/25.
//


#include "config.h"
#include "HttpManager.h"
#include "ble/BleServerManager.h"
#include "i2c/I2CManager.h"
#include "uart/UartManager.h"

BleServerManager ble_server_manager;
HttpManager http_manager;
UartManager uart_manager(Serial2);

void onDataReceivedOverBle(String data)
{
    uart_manager.sendData(data);
    Serial.println("ble to serial : " + data);
}

void onDataReceivedOverSerial(const String data)
{
    ble_server_manager.sendData(data);
    http_manager.sendData(data);
    Serial.println("serial to ble & http : " + data);
}

void onDataReceivedOverOverHttp(String data)
{
    uart_manager.sendData(data);
    Serial.println("http to serial : " + data);
}

void setup()
{
    Serial.begin(115200);
    delay(1000); // Give serial some time to stabilize
    uart_manager.start(16, 17, 9600);
    Serial.println("ı2c manager initialized on pins 21(SDA), 22(SCL)");

    uart_manager.onDataReceived(onDataReceivedOverSerial);

    // WiFi starts BEFORE BLE for better coexistence
    if (http_manager.start(WIFI_SSID, WIFI_PASS)) {
        Serial.println("HTTP Manager başlatıldı!");
        Serial.print("Web arayüzüne erişim: http://");
        Serial.println(http_manager.getIPAddress());
    } else {
        Serial.println("HTTP Manager başlatılamadı!");
        return;
    }

    // HTTP'den gelen verileri UART'a yönlendir
    http_manager.onDataReceived(onDataReceivedOverOverHttp);

    if (ble_server_manager.begin())
    {
        Serial.println("BLE Server initialized successfully!");
    }
    else
    {
        Serial.println("Failed to initialize BLE Server!");
        return;
    }
    // Set callback for received data
    ble_server_manager.setDataReceivedCallback(onDataReceivedOverBle);

    Serial.println("Server is ready and advertising...");

}

void loop()
{
    uart_manager.loop();
}
