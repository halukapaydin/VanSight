#include "HttpManager.h"
#include "WebPage.h"

HttpManager::HttpManager() : server(nullptr), ws(nullptr) {
}

HttpManager::~HttpManager() {
    if (ws) delete ws;
    if (server) delete server;
}

bool HttpManager::start(const char* ssid, const char* password, uint16_t port) {
    // WiFi'yi AP (Access Point) modunda başlat
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(ssid, password)) {
        Serial.println("WiFi AP başlatılamadı!");
        return false;
    }
    
    Serial.println("WiFi AP Başlatıldı!");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP Adresi: ");
    Serial.println(WiFi.softAPIP());
    
    // Web server'ı oluştur
    server = new AsyncWebServer(port);
    
    // WebSocket'i oluştur
    ws = new AsyncWebSocket("/ws");
    
    // WebSocket event handler'ı bağla
    ws->onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client,
                       AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->handleWebSocketEvent(server, client, type, arg, data, len);
    });
    
    server->addHandler(ws);
    
    // Route'ları ayarla
    setupRoutes();
    
    // Server'ı başlat
    server->begin();
    
    return true;
}

void HttpManager::onDataReceived(std::function<void(String)> callback) {
    dataReceivedCallback = callback;
}

void HttpManager::sendData(const String& data) {
    if (ws) {
        ws->textAll(data);
    }
}

String HttpManager::getIPAddress() {
    return WiFi.localIP().toString();
}

int HttpManager::getClientCount() {
    return ws ? ws->count() : 0;
}

void HttpManager::handleWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                                      AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected\n", client->id());
            break;
            
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
            
        case WS_EVT_DATA: {
            AwsFrameInfo* info = (AwsFrameInfo*)arg;
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                data[len] = 0;
                String message = String((char*)data);
                
                Serial.print("WebSocket'ten alındı: ");
                Serial.println(message);
                
                // Callback'i çağır
                if (dataReceivedCallback) {
                    dataReceivedCallback(message);
                }
            }
            break;
        }
            
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void HttpManager::setupRoutes() {
    // Ana sayfa
    server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", HTML_PAGE);
    });
    
    // 404 handler
    server->onNotFound([](AsyncWebServerRequest* request) {
        request->send(404, "text/plain", "Not found");
    });
}