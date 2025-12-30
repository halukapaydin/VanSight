# VanSightBLE Library

ESP32 için otomatik bağlantı ve yeniden bağlanma özelliklerine sahip BLE Client ve Server yönetim kütüphanesi.

## Özellikler

- ✅ **BLE Server**: Otomatik advertising yeniden başlatma
- ✅ **BLE Client**: Otomatik bağlanma ve yeniden bağlanma
- ✅ **String Data Exchange**: Client ve Server arasında string veri alışverişi
- ✅ **Callback Support**: Gelen veriler için callback desteği
- ✅ **State Management**: Client için durum yönetimi (DISCONNECTED, SCANNING, CONNECTING, CONNECTED)
- ✅ **Auto-Reconnection**: Bağlantı koptuğunda otomatik yeniden bağlanma

## Kurulum

### PlatformIO

Kütüphaneyi projenizin `lib` dizinine kopyalayın:

```bash
cp -r VanSightBLE /path/to/your/project/lib/
```

Veya `platformio.ini` dosyanıza ekleyin:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    VanSightBLE
```

## Kullanım

### BLE Server

```cpp
#include <Arduino.h>
#include <VanSightBLE.h>

BleServerManager bleServer;

void onDataReceived(String data) {
    Serial.print("Received: ");
    Serial.println(data);
    
    // Echo back
    bleServer.sendData("Server: " + data);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize server
    bleServer.begin();
    
    // Set callback
    bleServer.setDataReceivedCallback(onDataReceived);
}

void loop() {
    if (bleServer.isClientConnected()) {
        bleServer.sendData("Hello from server!");
        delay(5000);
    }
}
```

### BLE Client

```cpp
#include <Arduino.h>
#include <VanSightBLE.h>

BleClientManager bleClient;

void onDataReceived(String data) {
    Serial.print("Received: ");
    Serial.println(data);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize client
    bleClient.begin();
    
    // Set callback
    bleClient.setDataReceivedCallback(onDataReceived);
}

void loop() {
    // IMPORTANT: Must call loop() for auto-reconnection
    bleClient.loop();
    
    if (bleClient.isConnected()) {
        bleClient.sendData("Hello from client!");
        delay(5000);
    }
}
```

## API Referansı

### BleServerManager

#### Metodlar

- `bool begin()` - Server'ı başlatır ve advertising'i başlatır
- `bool sendData(const String& data)` - Bağlı client'lara veri gönderir
- `bool isClientConnected()` - Client bağlantı durumunu döner
- `void setDataReceivedCallback(DataReceivedCallback callback)` - Veri alındığında çağrılacak callback'i ayarlar

### BleClientManager

#### Metodlar

- `bool begin()` - Client'ı başlatır ve ilk bağlantıyı dener
- `void loop()` - **ÖNEMLİ**: Otomatik yeniden bağlanma için loop() içinde çağrılmalıdır
- `bool sendData(const String& data)` - Server'a veri gönderir
- `bool isConnected()` - Bağlantı durumunu döner
- `void setDataReceivedCallback(DataReceivedCallback callback)` - Veri alındığında çağrılacak callback'i ayarlar
- `BleClientState getState()` - Mevcut durumu döner

#### Durumlar (BleClientState)

- `BLE_DISCONNECTED` - Bağlantı yok
- `BLE_SCANNING` - Server taranıyor
- `BLE_CONNECTING` - Server'a bağlanılıyor
- `BLE_CONNECTED` - Bağlantı kuruldu

## Konfigürasyon

`BleConfig.h` dosyasında aşağıdaki parametreleri değiştirebilirsiniz:

```cpp
#define BLE_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_SERVER_NAME         "VanSight_Server"
#define BLE_CLIENT_NAME         "VanSight_Client"
#define BLE_SCAN_DURATION       5     // Tarama süresi (saniye)
#define BLE_RECONNECT_INTERVAL  3000  // Yeniden bağlanma aralığı (ms)
#define BLE_MAX_DATA_LENGTH     512   // Maksimum veri uzunluğu
```

## Örnekler

Kütüphane ile birlikte gelen örnekler:

- **BleServer**: Server örneği - Periyodik mesaj gönderme ve echo
- **BleClient**: Client örneği - Otomatik bağlanma ve periyodik mesaj gönderme

Örnekleri çalıştırmak için:

```bash
cd examples/BleServer
pio run -t upload -t monitor

# Başka bir ESP32'de
cd examples/BleClient
pio run -t upload -t monitor
```

## Troubleshooting

### Client bağlanamıyor

- Server'ın çalıştığından emin olun
- Serial monitor'dan "Advertising" mesajını kontrol edin
- UUID'lerin her iki tarafta da aynı olduğundan emin olun

### Bağlantı sık sık kopuyor

- `BLE_RECONNECT_INTERVAL` değerini artırın
- ESP32'lerin birbirine yakın olduğundan emin olun
- Güç kaynağının yeterli olduğundan emin olun

### Veri gönderilmiyor

- `isConnected()` veya `isClientConnected()` ile bağlantıyı kontrol edin
- Veri uzunluğunun `BLE_MAX_DATA_LENGTH` değerini aşmadığından emin olun
- Serial monitor'dan hata mesajlarını kontrol edin

## Lisans

MIT License

## Katkıda Bulunma

Pull request'ler memnuniyetle karşılanır. Büyük değişiklikler için lütfen önce bir issue açarak neyi değiştirmek istediğinizi tartışın.
