//
// Created by haluk on 12/27/25.
//

#ifndef SERVER_HTTPMANAGER_H
#define SERVER_HTTPMANAGER_H


#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <functional>

class HttpManager {
public:
    HttpManager();
    ~HttpManager();

    // WiFi ve server'ı başlat
    bool start(const char* ssid, const char* password, uint16_t port = 80);

    // Callback fonksiyonları
    void onDataReceived(std::function<void(String)> callback);

    // WebSocket üzerinden veri gönder
    void sendData(const String& data);

    // IP adresini al
    String getIPAddress();

    // Bağlı client sayısı
    int getClientCount();

private:
    AsyncWebServer* server;
    AsyncWebSocket* ws;
    std::function<void(String)> dataReceivedCallback;

    // WebSocket event handler
    void handleWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                             AwsEventType type, void* arg, uint8_t* data, size_t len);

    // HTML sayfası
    void setupRoutes();
};


#endif //SERVER_HTTPMANAGER_H