# ESP8266-CaptiveWiFiConfig

**🌐 Language**: English | [简体中文](../README.md)

> A flexible and user-friendly WiFi configuration portal for ESP8266, featuring captive portal support, an HTML-based UI, and EEPROM credential storage.

This project provides a complete ESP8266-based WiFi configuration system. It includes captive portal redirection, network scanning, manual input, and EEPROM-based credential storage—perfect for embedded device initialization and network setup.

---

## ✨ Features

- ✅ Automatically creates a hotspot and redirects all HTTP traffic (Captive Portal)
- ✅ Scans nearby `WiFi` networks and displays them in a dropdown list
- ✅ Allows manual input for hidden SSIDs
- ✅ Saves credentials to EEPROM and restarts to connect
- ✅ Serial command `clear` can erase saved WiFi configuration
- ✅ Compatible with Android / iOS / macOS / Windows network detection mechanisms
- ✅ Automatically triggers the system's WiFi configuration popup

---

## 📷 UI Preview

- ✅ **macOS**
  ![macOS.png](image/macOS.png)

The configuration page includes:
- WiFi dropdown list (selectable)
- SSID input field (auto-filled or manually editable)
- Password input field
- Submit button

---

## 📦 Usage Instructions

### 📌 Hardware Requirements
- ESP8266 module (e.g., NodeMCU, Wemos D1 Mini)
- PlatformIO development environment

### 🛠 Build Steps

Clone this repository:

```bash
git clone https://github.com/ExMikuPro/captive_portal.git
