<div align="center">
  <h1>🌱 Hệ Thống Giám Sát & Điều Khiển Vườn Thông Minh - ESP32 🌤️</h1>
  <p>✨ Dự án tích hợp cảm biến và điều khiển tự động hệ thống nông nghiệp nhỏ với <strong>ESP32</strong> và <strong>Dashboard + App điện thoại</strong>.</p>
  <p>Hệ thống có thể theo dõi <strong>nhiệt độ, độ ẩm, ánh sáng</strong> và điều khiển <strong>tưới cây, quạt tránh ẩm, mái che</strong> một cách thông minh.</p>

  <p>
    <img src="https://img.shields.io/badge/ESP32-Microcontroller-brightgreen?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32 Badge">
    <img src="https://img.shields.io/badge/Ngôn ngữ-Arduino C++-blue?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino Badge">
    <img src="https://img.shields.io/badge/Giám sát-App%20điện%20thoại%20+%20Web-orange?style=for-the-badge&logo=firebase&logoColor=white" alt="App Badge">
  </p>

---

  <p>
    <a href="#🚀-tổng-quan">Tổng quan</a> •
    <a href="#🛠️-hướng-dẫn-sử-dụng">Hướng dẫn sử dụng</a> •
    <a href="#⚙️-chi-tiết-chức-năng">Chi tiết chức năng</a> •
    <a href="#🚩-sản-phẩm-thực-tế">Sản phẩm thực tế</a>
  </p>

---
</div>

<br>

## 🚀 Tổng quan

Dự án này mô phỏng **hệ thống giám sát và điều khiển môi trường cây trồng** thông minh dựa trên **ESP32**. Hệ thống có khả năng:

- **Đọc dữ liệu** từ cảm biến nhiệt độ, độ ẩm, và ánh sáng.
- **Hiển thị thời gian thực** trên **dashboard web/app điện thoại** (Firebase).
- **Điều khiển** tưới cây, bật quạt, đóng/mở mái che dựa theo điều kiện cài đặt.

<br>

## 🛠️ Hướng dẫn Sử dụng

### 🔧 Yêu cầu phần mềm
1. Arduino IDE / PlatformIO – Biên dịch và nạp mã nguồn vào ESP32.

2. Firebase Realtime Database – Lưu trữ và đồng bộ dữ liệu giám sát.

3. App điện thoại / Dashboard Web – Điều khiển từ xa.

### 📥 Các bước triển khai
1. Tải dự án:
   
```bash
git clone https://github.com/LucPac/ESP32_Dashboard_garden.git
```
   
2. Mở thư mục bằng Arduino IDE hoặc PlatformIO và cấu hình:

    WiFi SSID & password  
  
    Firebase host & token

3. Nạp chương trình vào ESP32.

4. Mở index.html trên trình duyệt hoặc tải app tển điện thoại để giám sát và điều khiển.

5. Quan sát dữ liệu nhiệt độ, độ ẩm, ánh sáng hiển thị và kiểm soát thiết bị bằng nút.

<br>

## ⚙️ Chi tiết chức năng

```bash
| Thành phần        | Chức năng                                              |
|-------------------|--------------------------------------------------------|
| Nhiệt độ / Độ ẩm  | Đọc bằng cảm biến DHT11                                |
| Ánh sáng          | Đọc bằng cảm biến quang trở / LDR                      |
| Tưới cây          | Tự động bật khi độ ẩm dưới ngưỡng / Điều khiển từ app  |
| Quạt thông gió    | Bật khi độ ẩm quá cao / Điều khiển thủ công            |
| Mái che           | Đóng/mở khi ánh sáng quá gắt hoặc theo lệnh người dùng |
| Dashboard Web/App | Hiển thị dữ liệu và gửi lệnh điều khiển (qua Firebase) |
```

<br>

## 🚩 Sản phẩm thực tế  

Hình ảnh Dashboard

![Screenshot (88)](https://github.com/user-attachments/assets/69fb3322-0883-4147-86d6-2e13d60fea53)

Hình ảnh App

<div align="center">![Screenshot_2025 06 16_21 40 53 810](https://github.com/user-attachments/assets/4e3e2b38-30c4-4426-9b80-3b8a2767c010)

Hình ảnh mạch thực tế

![image](https://github.com/user-attachments/assets/f16a79a2-6e48-48a7-94b4-9fe21af7fad4)


Video Demo

[![image](https://github.com/user-attachments/assets/a3b1c62b-9412-4591-9d95-9bcef3d3614c)](https://www.youtube.com/watch?v=0sz0hhzup2c)

<br>

---

<div align="center">
  <br>
  <p>Cảm ơn bạn đã ghé thăm! Hy vọng repo này hữu ích cho việc học tập và nghiên cứu của bạn. 😊</p>
  </div>
