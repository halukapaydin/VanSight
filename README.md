# VanSight

**VanSight**, karavan/kamyonet gibi araçlarda röle kontrolü ve sıvı seviye izlemesi için geliştirilmiş, gömülü C/C++ tabanlı bir IoT kontrol sistemidir. Arduino Mega ve ESP32 mikrodenetleyicilerini bir arada kullanarak Bluetooth (BLE), Wi-Fi HTTP ve kızılötesi (IR) üzerinden kontrol imkânı sunar.

---

## Mimari Genel Bakış

Sistem birbirinden bağımsız çalışan üç donanım katmanından oluşur:

```
┌──────────────────────────────────────────────────────────────────┐
│                         DISPLAY CLIENT                           │
│              (ESP32-S3 Box — LVGL Dokunmatik Ekran)              │
│           BLE Client  ←→  Görsel Arayüz (16 Röle + 3 Sensör)    │
└──────────────────────────┬───────────────────────────────────────┘
                           │ Bluetooth BLE (NimBLE)
┌──────────────────────────▼───────────────────────────────────────┐
│                           SERVER                                 │
│                     (ESP32 — Köprü Katmanı)                      │
│    BLE Server  ←→  UART  ←→  HTTP (ESPAsyncWebServer)           │
│                 + IR Uzaktan Kumanda Alıcısı                     │
└──────────────────────────┬───────────────────────────────────────┘
                           │ UART (Seri Haberleşme)
┌──────────────────────────▼───────────────────────────────────────┐
│                            HUB                                   │
│                  (Arduino Mega 2560 — Ana Kontrol)               │
│     16× Röle  +  3× Sıvı Seviye Sensörü  +  I2C  +  Buzzer      │
└──────────────────────────────────────────────────────────────────┘
```

---

## Modüller

### `hub/` — Arduino Mega 2560 (Ana Kontrol Birimi)

Sistemin fiziksel kontrol katmanıdır. Röleleri ve sensörleri doğrudan yönetir.

**Donanım:**
- Platform: Arduino Mega 2560
- Framework: Arduino (PlatformIO)
- Programlama: USBtiny ISP

**Özellikler:**
- 16 kanallı röle kontrolü (pin 22–37)
- 3 adet analog sıvı seviye sensörü (A0, A1, A2)
- Direnç tabanlı sensör kalibrasyonu (Sensör 1: 0–190Ω, Sensör 2: 30–240Ω, Sensör 3: 30–270Ω)
- UART üzerinden JSON tabanlı komut alımı
- I2C slave desteği (adres: `0x20`)
- Buzzer geri bildirimi (pin 8)

**Komut Tipleri:**

| Komut | Açıklama |
|---|---|
| `RELAY_TOGGLE` | Belirtilen röleyi açar/kapar |
| `RELAY_ON` | Röleyi açar |
| `RELAY_OFF` | Röleyi kapatır |
| `ALL_RELAYS_OFF` | Tüm röleleri kapatır |
| `ALL_STATUS` | Tüm durum bilgisini döner |
| `LEVEL_STATUS` | Sensör seviyelerini döner |

---

### `server/` — ESP32 (Köprü / Gateway Katmanı)

BLE, HTTP ve UART arasında çift yönlü veri köprüsü kurar.

**Donanım:**
- Platform: ESP32 Dev Module
- Framework: Arduino (PlatformIO)

**Özellikler:**
- BLE Server (NimBLE-Arduino) — Display Client'tan komut alır, Hub'a iletir
- ESPAsyncWebServer — HTTP üzerinden komut alımı ve durum bildirimi
- UART köprüsü (Serial2) — Hub ile haberleşme
- IR kızılötesi alıcı (pin 27) — Uzaktan kumanda desteği (0–9, OK, yön tuşları)
- Wi-Fi erişim noktası: SSID `VanSight`, Şifre `password123`
- I2C master (SDA: 21, SCL: 22)

**Veri akışı:**

```
BLE  →  UART (Hub'a komut gönder)
HTTP →  UART (Hub'a komut gönder)
IR   →  UART (Tuş koduna göre röle toggle)
UART →  BLE + HTTP (Hub yanıtını ilet)
```

---

### `display-client/` — ESP32-S3 Box (Dokunmatik Ekran Arayüzü)

LVGL tabanlı grafik arayüzüyle kullanıcıya görsel kontrol sunar.

**Donanım:**
- Platform: ESP32-S3 Box
- Bellek: 8 MB Flash, PSRAM etkin
- Framework: Arduino (PlatformIO)

**Özellikler:**
- LVGL 8.3.8 ile dokunmatik UI
- BLE Client (NimBLE) — Server'a bağlanır
- 16 röle butonu (açık/kapalı renk gösterimi)
- 3 sensör için doluluk çubuğu ve yüzde etiketi
- Thread-safe UI güncellemeleri (FreeRTOS Queue)
- Uyku yöneticisi (SleepManager) ve panel yöneticisi

---

### `display-client-simulator/`

Display Client'ın masaüstü ortamında simüle edilmesi için yardımcı proje. Geliştirme ve test amaçlıdır.

---

### `lib/` — Ortak Kütüphane

Hub ve Server tarafından paylaşılan bileşenleri içerir.

**Bileşenler:**
- `protocol/protocol.h` — Komut ve yanıt veri yapıları
- `json/JsonCodec` — JSON serileştirme/deserileştirme
- `relay/RelayController` — Röle yönetimi
- `level-sensor/SensorController` — Analog seviye okuma ve hesaplama
- `uart/UartManager` — UART haberleşme yönetimi
- `i2c/I2CManager` — I2C haberleşme yönetimi
- `buzzer/BuzzerManager` — Buzzer kontrol

---

### `lib-ble/` — BLE Kütüphanesi

Server ve Display Client tarafından paylaşılan Bluetooth Low Energy bileşenleri.

**Bileşenler:**
- `BleServerManager` — NimBLE tabanlı BLE sunucu (Server tarafı)
- `BleClientManager` — NimBLE tabanlı BLE istemci (Display Client tarafı)
- `BleConfig` — Servis ve karakteristik UUID tanımları

**Bağımlılık:** `h2zero/NimBLE-Arduino@^1.4.1`

---

## İletişim Protokolü

Bileşenler arasındaki veri alışverişi JSON formatında yapılır.

**Komut Yapısı:**

```json
{
  "type": "RELAY_TOGGLE",
  "parameter": {
    "item_number": 3
  }
}
```

**Yanıt Yapısı:**

```json
{
  "relays": [true, false, true, ...],
  "levels": [75, 40, 90]
}
```

---

## Proje Yapısı

```
VanSight/
├── hub/                        # Arduino Mega — Fiziksel kontrol
│   ├── platformio.ini
│   └── src/
│       ├── main.cpp
│       ├── config.h
│       ├── relay/
│       ├── level-sensor/
│       ├── uart/
│       ├── i2c/
│       ├── buzzer/
│       ├── protocol/
│       └── json/
├── server/                     # ESP32 — BLE/HTTP/IR köprüsü
│   ├── platformio.ini
│   └── src/
│       ├── main.cpp
│       ├── config.h
│       ├── ble/
│       ├── uart/
│       ├── infrared/
│       ├── i2c/
│       └── http/
├── display-client/             # ESP32-S3 — Dokunmatik ekran
│   ├── platformio.ini
│   └── src/
│       ├── main.cpp
│       ├── BleCommandHandler
│       ├── PanelManager
│       ├── UIStateManager
│       └── SleepManager
├── display-client-simulator/   # Masaüstü simülatörü
├── lib/                        # Paylaşılan ortak kütüphane
└── lib-ble/                    # Paylaşılan BLE kütüphanesi
```

---

## Donanım Gereksinimleri

| Bileşen | Model | Adet |
|---|---|---|
| Ana kontrol birimi | Arduino Mega 2560 | 1 |
| Köprü / Gateway | ESP32 Dev Module | 1 |
| Dokunmatik ekran | ESP32-S3 Box | 1 |
| Röle modülü | 16 Kanal Röle | 1 |
| Sıvı seviye sensörü | Direnç tipi | 3 |
| IR alıcı modülü | 38 kHz | 1 |
| Buzzer | Aktif buzzer | 1 |
| Programlayıcı | USBtiny ISP | 1 |

---

## Geliştirme Ortamı

Bu proje **PlatformIO** ile geliştirilmektedir. Her alt proje kendi `platformio.ini` dosyasına sahiptir.

### Bağımlılıklar

| Kütüphane | Versiyon | Kullanıldığı Yer |
|---|---|---|
| NimBLE-Arduino | ^1.4.1 | server, display-client, lib-ble |
| lvgl | 8.3.8 | display-client |
| ESPAsyncWebServer | latest | server |
| AsyncTCP | latest | server |
| IRremote | latest | server |

### Kurulum

```bash
# Repo'yu klonlayın
git clone https://github.com/halukapaydin/VanSight.git
cd VanSight

# İstediğiniz alt projeyi PlatformIO ile açın
cd hub        # veya server, display-client
pio run       # Derleme
pio run -t upload  # Yükleme
```

---

## Lisans

Bu proje hakkında açık bir lisans bilgisi bulunmamaktadır. Kaynak kodu incelemek ve katkıda bulunmak için repository sahibiyle iletişime geçiniz.

---

*Bu belge, kaynak kodu incelenerek oluşturulmuştur. Proje aktif geliştirme aşamasındadır.*
