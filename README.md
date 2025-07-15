# 🔔 Smart School Bell with ESP32

This project rings a real school bell automatically using an **ESP32** and a **relay**. You can control it through Wi-Fi and see the time on an OLED screen.

---

## 📁 Files in This Project

- **ESP32_code_file.ino** – The main code for your ESP32
- **circuit connection.png** – How to connect everything
- **code.txt** – Code in text format (for reading)
- **README.md** – This file

---

## 💡 Features

- Rings bell at set times (class, exam, or custom mode)
- Controlled through a web page
- Shows time and current mode on OLED
- Keeps ringing schedule even if Wi-Fi goes offline
- Uses a **real bell** (relay connected), not a buzzer

---

## 🛠 What You Need

- ESP32 board  
- OLED display (0.96", I2C)  
- Relay module  
- Wi-Fi connection (for setup)

---

## ⏰ Ring Times

- Class bell: rings for **3 seconds**  
- Break/Lunch bell: rings for **6 seconds**

---

## 🔌 Circuit Overview

- OLED:  
  - SDA → GPIO 21  
  - SCL → GPIO 22  
- Relay → GPIO 27  
- Power via USB or adapter

📷 **Circuit image:**  
![Circuit](circuit%20connection.png)

---

## 🌐 Web Control

- Login password: `7900`  
- Modes: CLASS, EXAM, CUSTOM  
- Set times and labels via web

---

## 📲 How to Use

1. Open the `.ino` file in Arduino IDE
2. Install required libraries:
   - Adafruit SSD1306  
   - Adafruit GFX  
   - NTPClient  
   - Preferences  
   - WiFi  
3. Choose **ESP32 board** in Tools  
4. Upload the code and connect

---

## 📘 Learn More

Visit my blog for full steps and updates:  
👉 [My Website](https://satower9thgen.blogspot.com/2025/07/smart-bell-system-web-control-offline.html)

---

Made with ❤️ by **Sanjey**
