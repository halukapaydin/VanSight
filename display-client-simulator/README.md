# VanSight Display Simulator

PC simülatörü ile VanSightDisplayClient UI'ını ESP32 donanımı olmadan bilgisayarınızda görselleştirebilirsiniz.

## Gereksinimler

- **SDL2 development kütüphaneleri**
- **CMake** (3.10 veya üzeri)
- **GCC** veya uyumlu C derleyicisi

### SDL2 Kurulumu

Ubuntu/Debian:
```bash
sudo apt-get install libsdl2-dev cmake build-essential
```

## Derleme

### Hızlı Yöntem (Önerilen)

Build scriptini kullanın:
```bash
cd /development/workspace/VanSight/VanSightDisplaySimulator
./build.sh
```

Clean build için:
```bash
./build.sh clean
```

### Manuel Yöntem

1. Simülatör dizinine gidin:
```bash
cd /development/workspace/VanSight/VanSightDisplaySimulator
```

2. Build dizini oluşturun ve derleyin:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Çalıştırma

### Hızlı Yöntem (Önerilen)

Run scriptini kullanın:
```bash
./run.sh
```

### Manuel Yöntem

Build dizininden simülatörü çalıştırın:
```bash
cd build
./vansight_simulator
```

Bir SDL2 penceresi açılacak ve VanSight UI'ı görüntülenecektir:
- **Çözünürlük**: 800x480
- **Pencere boyutu**: 1600x960 (2x zoom)
- **Mouse**: Dokunmatik ekranı simüle eder
- **Klavye**: Navigasyon için kullanılabilir

## UI Güncelleme

SquareLine Studio'da yaptığınız değişiklikleri simülatörde test etmek için:

### Otomatik Yöntem (Önerilen)

Build scripti otomatik olarak VanSightDisplayClient'tan UI dosyalarını kopyalar:
```bash
./build.sh
```

UI dosyaları `VanSightDisplayClient/lib/ui/` dizininden otomatik olarak kopyalanır ve simülatör yeniden derlenir.

### Manuel Yöntem

1. SquareLine Studio'dan UI'ı export edin
2. Export edilen dosyaları VanSightDisplayClient'a kopyalayın
3. Build scriptini çalıştırın (otomatik olarak sync yapacak):
```bash
./build.sh
```

### UI Sync'i Atlamak

Eğer UI dosyalarını kopyalamadan sadece derlemek isterseniz:
```bash
./build.sh --no-sync
```

## Özellikler

✅ **LVGL 8.3.11** ile tam uyumluluk  
✅ **SDL2** tabanlı pencere ve input yönetimi  
✅ **800x480** çözünürlük  
✅ **2x zoom** ile rahat görüntüleme (1600x960 pencere)  
✅ **Mouse** ile dokunmatik ekran simülasyonu  
✅ **Hızlı iterasyon** - donanım olmadan UI testi  
✅ **Kolay build** - `./build.sh` ile tek komut  
✅ **Kolay çalıştırma** - `./run.sh` ile tek komut  

## Sınırlamalar

⚠️ Bu simülatör sadece **UI görselleştirmesi** içindir  
⚠️ **ESP-NOW** iletişimi simüle edilmez  
⚠️ **Sensör verileri** gerçek zamanlı güncellenmez  
⚠️ Sadece SquareLine Studio'dan export edilen temel UI gösterilir  

## Sorun Giderme

### "SDL2 bulunamadı" hatası
```bash
sudo apt-get install libsdl2-dev
```

### Font hatası (lv_font_montserrat_XX)
`lv_conf.h` dosyasında ilgili fontu etkinleştirin:
```c
#define LV_FONT_MONTSERRAT_XX 1
```

### Pencere açılmıyor
X11 display'in doğru yapılandırıldığından emin olun:
```bash
echo $DISPLAY
```

## Proje Yapısı

```
VanSightDisplaySimulator/
├── lvgl/              # LVGL 8.3.11 kütüphanesi
├── lv_drivers/        # SDL2 driver
├── ui/                # SquareLine Studio UI dosyaları
│   └── src/
│       ├── screens/   # Ekran tanımları
│       ├── ui.c       # Ana UI kodu
│       └── ui.h       # UI header
├── main.c             # Simülatör entry point
├── lv_conf.h          # LVGL konfigürasyonu
├── lv_drv_conf.h      # Driver konfigürasyonu
├── CMakeLists.txt     # Build sistemi
└── build/             # Derleme çıktıları
    └── vansight_simulator  # Executable
```

## Katkıda Bulunma

UI değişikliklerini test ettikten sonra, gerçek ESP32 donanımında da test etmeyi unutmayın!
