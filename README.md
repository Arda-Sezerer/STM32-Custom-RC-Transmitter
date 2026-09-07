# STM32 Tabanlı Özel Tasarım Radyo Kontrol (RC) Vericisi

Bu proje; STM32F103C8 (ARM Cortex-M3) mikrodenetleyicisi mimarisi üzerine kurulu, yüksek menzilli RF haberleşme modüllerine ve kafa takibi (head-tracking) sensör entegrasyonuna sahip özel tasarım bir radyo kontrol vericisidir (RC Transmitter).

---

## 📌 Mimari ve Sistem Özellikleri

- **İşlemci & Donanım:** STM32F103C8 (ARM Cortex-M3) mikrodenetleyicisi ile düşük gecikmeli sinyal işleme.
- **Analog Girdiler:** Joystikler ve potansiyometreler üzerinden hassas ADC (Analog-Digital Converter) veri okuması.
- **Kablosuz Haberleşme:** Ebyte / NRF24L01 RF modülü entegrasyonu ile kesintisiz ve yüksek menzilli veri aktarımı.
- **Gelişmiş Sensör Entegrasyonu:** MPU6050 IMU sensörü ile FPV sistemleri ve RC hava araçları için eğim/kafa takip (head-tracking) kontrolü.
- **Anahtarlama & Arayüz:** Durum LED'leri ve mod seçim anahtarları ile tam modüler kontrol.

---

## 🛠️ Donanım & Arayüz Yapılandırması

| Bileşen / Çevre Birimi | STM32 Pini / Birim | Açıklama |
| :--- | :--- | :--- |
| **MPU6050 (IMU)** | I2C1 (SCL/SDA) | Kafa Takip (Head-Tracking) Verisi |
| **Ebyte / NRF24L01 RF** | SPI1 (SCK/MISO/MOSI/CSN/CE) | Uzak Mesafe RF İletişimi |
| **Joystikler & Potansiyometreler** | ADC1 (PA0 - PA4) | Analog Kontrol Kanalları |
| **Kontrol Anahtarları** | GPIO (Input Pull-up) | Mod Seçim ve Toggle Düğmeleri |

---

## 📂 Proje Yapısı

```text
.
├── Core/
│   ├── Inc/                 # Header (.h) dosyaları ve kütüphaneler
│   └── Src/                 # Ana C kaynak kodları (main.c, rf.c, mpu6050.c)
├── Hardware/                # PCB / Şematik görsel veya dosyaları
└── README.md                # Proje dokümantasyonu
