<div align="center">
  <h1>🌱 Smart Garden Monitoring & Control System - ESP32 🌤️</h1>
  <p>✨ A project integrating sensors and automatic control for small-scale agriculture using <strong>ESP32</strong> with <strong>Web Dashboard + Mobile App</strong>.</p>
  <p>The system can monitor <strong>temperature, humidity, and light</strong> and control <strong>watering, ventilation fan, and roof cover</strong> automatically.</p>

  <p>
    <img src="https://img.shields.io/badge/ESP32-Microcontroller-brightgreen?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32 Badge">
    <img src="https://img.shields.io/badge/Language-Arduino C++-blue?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino Badge">
    <img src="https://img.shields.io/badge/Monitoring-Mobile%20App%20+%20Web-orange?style=for-the-badge&logo=firebase&logoColor=white" alt="App Badge">
  </p>

---

  <p>
    <a href="#🚀-overview">Overview</a> •
    <a href="#🛠️-how-to-use">How to Use</a> •
    <a href="#⚙️-features">Features</a> •
    <a href="#🚩-real-product">Real Product</a>
  </p>

---
</div>

<br>

## 🚀 Overview

This project simulates a **smart plant environment monitoring and control system** based on **ESP32**. The system can:

- **Read data** from temperature, humidity, and light sensors.
- **Display in real-time** on **web dashboard/mobile app** (Firebase).
- **Control** watering, fan, and roof cover based on configured settings.

<br>

## 🛠️ How to Use

### 🔧 Software Requirements
1. Arduino IDE / PlatformIO – Compile and upload code to ESP32.

2. Firebase Realtime Database – Store and sync monitoring data.

3. Mobile App / Web Dashboard – Remote control.

### 📥 Installation Steps
1. Download the project:
   
```bash
git clone https://github.com/LucPac/ESP32_Dashboard_garden.git
```
   
2. Open the folder with Arduino IDE or PlatformIO and configure:

    WiFi SSID & password  
  
    Firebase host & token

3. Upload the program to ESP32.

4. Open index.html in your browser or install the mobile app to monitor and control.

5. View temperature, humidity, light data and control devices with buttons.

<br>

## ⚙️ Features

```bash
| Component         | Function                                                |
|-------------------|---------------------------------------------------------|
| Temperature / Humidity | Read by DHT11 sensor                               |
| Light             | Read by photoresistor / LDR sensor                      |
| Watering          | Manual control                                          |
| Ventilation Fan   | Manual control                                          |
| Roof Cover        | Open/close by user command                              |
| Web/App Dashboard | Display data and send control commands (via Firebase)  |
```

<br>

## 🚩 Real Product  

PCB Layout

![image](https://github.com/user-attachments/assets/d5d86b0d-4d34-4ffb-9508-a59af557e320)

![image](https://github.com/user-attachments/assets/19747b0d-8c98-4550-b9a2-ca5be101650c)

Dashboard Image

![Screenshot (88)](https://github.com/user-attachments/assets/69fb3322-0883-4147-86d6-2e13d60fea53)

App Image

![Screenshot_2025 06 16_21 40 53 810](https://github.com/user-attachments/assets/4e3e2b38-30c4-4426-9b80-3b8a2767c010)

Actual Circuit Image

![image](https://github.com/user-attachments/assets/f16a79a2-6e48-48a7-94b4-9fe21af7fad4)


Demo Video

[![image](https://github.com/user-attachments/assets/a3b1c62b-9412-4591-9d95-9bcef3d3614c)](https://www.youtube.com/watch?v=0sz0hhzup2c)

<br>

---

<div align="center">
  <br>
  <p>Thank you for visiting! I hope this repo is useful for your learning and research. 😊</p>
  </div>
