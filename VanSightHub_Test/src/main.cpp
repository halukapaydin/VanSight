#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// VanSightHub MAC adresi (Serial Monitor'den aldığınız adres)
// Örnek: 08:b6:1f:be:12:24
uint8_t hubAddress[] = {0x08, 0xb6, 0x1f, 0xbe, 0x12, 0x24};

// ESP-NOW gönderim callback
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

// ESP-NOW alma callback
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Serial.println("\n[Response Received]");
  
  // JSON parse et
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, incomingData, len);
  
  if (!error) {
    serializeJsonPretty(doc, Serial);
    Serial.println();
  } else {
    Serial.println("JSON parse error!");
  }
}

void sendCommand(const char* jsonCommand) {
  Serial.println("\n[Sending Command]");
  Serial.println(jsonCommand);
  
  esp_err_t result = esp_now_send(hubAddress, (uint8_t*)jsonCommand, strlen(jsonCommand));
  
  if (result == ESP_OK) {
    Serial.println("Sent successfully");
  } else {
    Serial.println("Send failed!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n================================");
  Serial.println("VanSightHub Test Controller");
  Serial.println("================================\n");
  
  // WiFi Station mode
  WiFi.mode(WIFI_STA);
  
  Serial.print("My MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // ESP-NOW başlat
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }
  
  Serial.println("ESP-NOW initialized");
  
  // Callback'leri kaydet
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);
  
  // VanSightHub'ı peer olarak ekle
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, hubAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer!");
    return;
  }
  
  Serial.println("VanSightHub added as peer");
  Serial.println("\nReady! Enter command number:");
  Serial.println("1 - Turn ON relay 1");
  Serial.println("2 - Turn OFF relay 1");
  Serial.println("3 - Turn ON all relays");
  Serial.println("4 - Turn OFF all relays");
  Serial.println("5 - Get all status");
  Serial.println("6 - Read sensor 1");
  Serial.println("7 - Read all sensors");
  Serial.println("8 - Toggle relay 3");
  Serial.println();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch(cmd) {
      case '1':
        sendCommand("{\"cmd\":\"relay_on\",\"relay\":1}");
        break;
      case '2':
        sendCommand("{\"cmd\":\"relay_off\",\"relay\":1}");
        break;
      case '3':
        sendCommand("{\"cmd\":\"all_relays_on\"}");
        break;
      case '4':
        sendCommand("{\"cmd\":\"all_relays_off\"}");
        break;
      case '5':
        sendCommand("{\"cmd\":\"all_status\"}");
        break;
      case '6':
        sendCommand("{\"cmd\":\"sensor_status\",\"sensor\":1}");
        break;
      case '7':
        sendCommand("{\"cmd\":\"all_sensor_status\"}");
        break;
      case '8':
        sendCommand("{\"cmd\":\"relay_toggle\",\"relay\":3}");
        break;
      default:
        // Ignore other characters
        break;
    }
  }
  
  delay(10);
}
