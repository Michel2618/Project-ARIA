# Project A.R.I.A.
**A**udio **R**eceiver **I**nterface **A**rchitecture

## 🛠️ Hardware Used
* **Brain:** ESP32-S3 Development Board
* **Audio Engine:** PCM5102A I2S DAC Module (Purple Board)
* **Power:** External 5V Power Supply required for DAC due to ESP32 voltage starvation.

### ⚡ Critical Hardware Modifications
* **The Mute Bypass:** The PCM5102A board ships permanently muted. Solder a bridge connecting the Middle and Left (H) pads of the H3L switch to pull XSMT High (3.3V) and unmute the board.
* **The Clock Ground:** The SCK pin on the DAC must be permanently wired to a GND pin to generate the internal clock.
* **Shared Ground:** The external 5V power supply for the DAC must share a common Ground connection with the ESP32-S3, or the digital signals will fail.

### 🔌 Final Wiring Pinout

| ESP32-S3 Pin | PCM5102A DAC Pin | Function |
| :--- | :--- | :--- |
| GND | GND | Common Ground |
| GND | SCK | Internal Clock Generation |
| External 5V | VIN | Main Power |
| GPIO 15 | BCK | Bit Clock |
| GPIO 16 | LRCK / LCK | Left-Right Clock (Word Select) |
| GPIO 17 | DIN | Data Input |

## 💻 Software & Firmware
* **OS:** Squeezelite-ESP32
* **Installer URL:** [https://sonocotta.github.io/esp32-audio-dock/](https://sonocotta.github.io/esp32-audio-dock/)
* **Target Firmware:** HiFi-ESP32-S3 (16-bit Standard)

## ⚙️ Critical Web UI Settings (192.168.1.xxx)

### Audio Tab
* **Output:** I2S Dac
* **Server:** *(Leave completely blank. Do not use -disable if connecting to Lyrion).*

### Hardware Tab -> DAC Options
* **DAC Model Name:** I2S
* **Clock GPIO:** 15
* **Word Select GPIO:** 16
* **Data GPIO:** 17

### System Tab -> Services
* Ensure **AirPlay** and **Bluetooth Speaker** are checked.

## 🧪 The "Bare Metal" Diagnostic Tool (`Hardware_Test_Imperial_March.ino`)

While this streamer relies on the open-source Squeezelite-ESP32 OS, this repository includes a custom C++ Arduino script. 

**Why is this here?**
When building custom I2S audio hardware, it can be extremely difficult to tell if a "no audio" issue is caused by a Wi-Fi network firewall, a software bug, or a physical soldering mistake. 

This script is a hardware diagnostic tool. Flashing this `.ino` file via the Arduino IDE completely wipes the Squeezelite OS and tests the raw metal. It forces the ESP32 to manually generate the I2S clock and play a hardcoded 16-bit melody (The Star Wars Imperial March). 

* **If it plays the melody:** Your hardware, soldering, DAC, and pins (15, 16, 17) are 100% flawless. You can safely reinstall the Squeezelite firmware knowing any future issues are purely software/network related.
* **If it is silent:** You have a physical hardware issue (bad ground, cold solder, or a dead DAC chip).
