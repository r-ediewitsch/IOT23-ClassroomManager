# 🏫 Classroom Manager

**Final Project Group 23 - Universitas Indonesia** 

> **Anggota Tim:**
> * Muhammad Hilmi Al Muttaqi (2306267082)
> * R. Aisha Syauqi Ramadhani (2306250554)
> * Wesley Frederick Oh (2306202763)
> * Muhammad Riyan Satrio Wibowo (2306229323)

---

## 1. Introduction

### 1.1 Problem Statement
Sistem manajemen ruang kelas saat ini masih mengandalkan kunci fisik dan peran manual petugas (*janitor*) yang menyebabkan tiga masalah utama:
1.  **Inefisiensi Operasional:** Ketergantungan pada petugas untuk membuka/mengunci pintu.
2.  **Risiko Keamanan:** Kunci fisik mudah hilang atau diduplikasi tanpa adanya log akses.
3.  **Pemborosan Energi:** AC dan lampu sering tertinggal menyala di ruangan kosong.

### 1.2 Proposed Solution
**Classroom Manager** adalah sistem kontrol akses otomatis berbasis IoT dan *Embedded Systems* yang menawarkan:
* **Keamanan Hibrida:** Kombinasi kunci fisik (Solenoid) dengan autentikasi digital **BLE Challenge-Response (HMAC-SHA256)**.
* **Resiliensi Tinggi:** Fitur **Offline Mode** yang memungkinkan sistem tetap berjalan menggunakan database lokal saat internet mati.
* **Otomatisasi Cerdas:** Integrasi **Sensor PIR** untuk manajemen penguncian otomatis (*auto-lock*) dan efisiensi energi.

### 1.3 Acceptance Criteria
Sistem dirancang untuk memenuhi kriteria keberhasilan berikut:
1.  Kendali Solenoid Door Lock via Relay.
2.  Autentikasi Aman (Anti-Replay Attack).
3.  Role-Based Access Control (RBAC).
4.  Sinkronisasi Database (Backend ke ESP32).
5.  Fitur Offline Mode (Fail-safe).
6.  Otomatisasi via Sensor PIR.
7.  Logging Real-time ke Dashboard MQTT.
8.  Remote Deep Sleep (Hemat Daya).
9.  Indikator Visual via **MAX7219 LED Matrix**.

---

## 2. Implementation

### 2.1 Hardware Design
Sistem menggunakan **ESP32** sebagai kontroler utama dengan arsitektur daya terpisah (12V untuk Solenoid, 5V via Buck Converter untuk ESP32) untuk stabilitas.

> **![Imgur](https://imgur.com/ZjYiHIm.png)**  
> *Fig 2. Desain Perangkat Keras dan Skematik Desain*

### 2.2 Software Development
Pengembangan perangkat lunak dilakukan secara terintegrasi:

* **A. Firmware ESP32 (FreeRTOS):**
    * **Task Management:** `doorTask` (High Priority) untuk kontrol pintu dan sensor, berjalan paralel dengan komunikasi WiFi/MQTT.
    * **Security:** Implementasi algoritma **HMAC-SHA256** untuk verifikasi tanda tangan digital pengguna.
    * **Offline Mode:** Fungsi `syncDatabase()` menyimpan data pengguna dari server ke *Flash Memory* (Preferences) untuk akses tanpa internet.

* **B. Backend Service:**
    * Dibangun menggunakan **Node.js** dan **Express.js** dengan database **MongoDB** untuk manajemen pengguna dan pencatatan log akses terpusat.

* **C. Alur Logika:**
    * **Autentikasi:** Validasi koneksi BLE hingga pembukaan kunci.
    * **Komunikasi Data:** Pengiriman log via MQTT dengan mekanisme *queue buffering*.
    * **State Machine:** Transisi status sistem (Idle -> Verifying -> Unlocked -> Deep Sleep).

> **![Imgur](https://imgur.com/U7QzKq4.png)**  
> *Fig 3. Flowchart proses autentikasi BLE dan kontrol solenoid*

> **![Imgur](https://imgur.com/2INRdUT.png)**  
> *Fig 5. State Machine Operasi Sistem Classroom Manager*

### 2.3 Hardware & Software Integration
Integrasi dilakukan melalui pemetaan GPIO:
* **Aktuator:** Sinyal dari `doorTask` mengaktifkan Relay di **GPIO 16**.
* **Sensor:** Input dari PIR di **GPIO 17** mereset *Software Timer* (Auto-lock extension).
* **Display:** Status sistem ditampilkan pada **LED Matrix MAX7219** via jalur SPI (GPIO 23, 18, 5).

---

## 3. Testing and Evaluation

### 3.1 Testing Methodology
Pengujian dilakukan menggunakan metode *Black-Box Testing* dengan 9 skenario uji utama, mencakup validasi keamanan, ketahanan jaringan (Offline Mode), otomatisasi PIR, dan manajemen daya.

### 3.2 Result
Hasil pengujian menunjukkan sistem berfungsi sesuai spesifikasi:
* **Keamanan:** Sistem berhasil menolak akses ilegal (Log: `Access Denied`) dan memvalidasi pengguna terdaftar.
* **Resiliensi:** Transisi ke **Offline Mode** berjalan mulus saat WiFi diputus, pintu tetap dapat dibuka.
* **Otomatisasi:** Sensor PIR berhasil memperpanjang durasi buka kunci saat mendeteksi gerakan.
* **Logging:** Data akses tercatat di Dashboard Node-RED secara *real-time*.

> **![Imgur](https://imgur.com/uiDTa1z.png)**  
> *Fig 11. Dashboard Node-RED Menampilkan Log Akses*

### 3.3 Evaluation
Evaluasi menyimpulkan bahwa sistem memiliki keunggulan signifikan pada:
1.  **Keamanan:** Mitigasi risiko *replay attack* dan duplikasi kunci.
2.  **Ketersediaan (Availability):** Sistem tidak lumpuh saat gangguan internet berkat arsitektur *hybrid*.
3.  **Efisiensi:** Penghapusan faktor *human error* dalam penguncian pintu dan manajemen daya.

---

## 4. Conclusion

Proyek **Classroom Manager** berhasil merealisasikan solusi keamanan cerdas yang andal. Penggunaan **FreeRTOS** menjamin responsivitas sistem dalam menangani *multitasking*. Fitur **Offline Mode** dan **Sinkronisasi Database** menjadi nilai tambah utama yang menjamin keberlangsungan operasional akademik tanpa hambatan konektivitas. Sistem ini siap dikembangkan lebih lanjut untuk implementasi skala luas di lingkungan kampus.

---

## 5. References

1. EMQX Platform Docs, “Connect with ESP32”, 2025. [Online]. Available: https://docs.emqx.com/en/cloud/latest/connect_to_deployments/esp32.html [Accessed: Dec. 08, 2025].
2. EMQX Blog, “MQTT on ESP32: A Beginner's Guide”, Aug. 05, 2024. [Online]. Available: https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker [Accessed: Dec. 08, 2025]. www.emqx.com
3. Random Nerd Tutorials, “ESP32 MQTT – Publish DS18B20 Temperature Readings …”, 2024. [Online]. Available: https://randomnerdtutorials.com/esp32-mqtt-publish-ds18b20-temperature-arduino/ [Accessed: Dec. 08, 2025].
4. Random Nerd Tutorials, “ESP32 MQTT Publish/Subscribe with Arduino IDE”, 2025. [Online]. Available: https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/ [Accessed: Dec. 08, 2025].
5. Random Nerd Tutorials, “ESP32 with PIR Motion Sensor using Interrupts and Timers”, Oct. 27, 2025. [Online]. Available: https://randomnerdtutorials.com/esp32-pir-motion-sensor-interrupts-timers/ [Accessed: Dec. 08, 2025].
6. SunFounder Documentation, “Lesson 12: PIR Motion Module (HC-SR501)”, 2025. [Online]. Available: https://docs.sunfounder.com/projects/umsk/en/latest/03_esp32/esp32_lesson12_pir_motion.html [Accessed: Dec. 08, 2025].
7. Instructables, “How to Use PIRs With Arduino & Raspberry Pi”, 2023. [Online]. Available: https://www.instructables.com/PIR-Motion-Sensor-How-to-Use-PIRs-With-Arduino-Ras/ [Accessed: Dec. 08, 2025].
8. ArXiv, J. Dizdarevic, M. Michalke, A. Jukan, “Engineering and Experimentally Benchmarking Open Source MQTT Broker Implementations”, May 2023. [Online]. Available: https://arxiv.org/abs/2305.13893 [Accessed: Dec. 08, 2025].
9. ArXiv, M. Ahmed, M. M. Akhtar, “Smart Home: Application using HTTP and MQTT as Communication Protocols”, Dec. 2021. [Online]. Available: https://arxiv.org/abs/2112.10339 [Accessed: Dec. 08, 2025].
