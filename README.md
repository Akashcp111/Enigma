# 🔐 ENIGMA – CLI-Based IoT Security & Wireless Attack Simulator

ENIGMA is a command-line IoT security toolkit powered by the ESP32 NodeMCU, built for real-time wireless signal monitoring, basic attack simulation, and hands-on cybersecurity research. It provides an educational, modular platform for ethical hackers, students, and professionals to safely explore wireless vulnerabilities.

> ⚠️ **For educational and ethical use only. Do not use this tool for unauthorized activities.**

---

## 🚀 Features

### 📡 Wi-Fi Security & Monitoring
- Wi-Fi Scanner
- Evil Twin Attack
- Hardcoded Evil Twin Attack

### 📶 Bluetooth Surveillance
- iPhone BLE Spamming

### 🔐 RFID/NFC Access Control
- RFID Tag Reader (MFRC522)
- UID storage with Profile

### ⚙️ Real-Time CLI Control
- Full Command-Line Operation
- Real-time log of events
- Configurable commands via Serial Terminal

---

## 🧰 Hardware Requirements

| Component             | Description                             |
|----------------------|-----------------------------------------|
| ESP32 NodeMCU        | Main Microcontroller (Wi-Fi + BLE)      |
| MFRC522 RFID Module  | RFID tag reader module                  |
| Buzzer, LEDs         | For alerts and interaction              |
| Push Button          | Trigger actions                         |
| Breadboard & Wires   | Prototyping setup                       |
| USB Cable            | Power + Serial Communication            |

**Pin Mapping Example for MFRC522 to ESP32** (update if needed):
- `SDA` → GPIO21  
- `SCK` → GPIO18  
- `MOSI` → GPIO23  
- `MISO` → GPIO19  
- `RST` → GPIO22  
- `VCC` → 3.3V  
- `GND` → GND  

---

## 💻 Software Components

- Custom ESP32 Firmware (C++) for wireless scanning and attack emulation
- Command-Line Interface (CLI) for real-time interaction
- Event Logger for tracking actions and signal events
- Task Scheduler for timed scans and auto-actions
- Integrated drivers: Wi-Fi, Bluetooth, RFID
- Compatible with macOS/Linux (via Serial Terminal)

---

## 🔧 Getting Started

Follow these steps to set up and run the ENIGMA CLI-based IoT security toolkit.

### 🧱 1. Hardware Setup

Ensure all components are connected according to the hardware table and pin map above.

---

### 💻 2. Software Requirements

Install the following tools and libraries:

#### Arduino IDE or PlatformIO
- [Download Arduino IDE](https://www.arduino.cc/en/software)

#### ESP32 Board Package for Arduino
1. Open **File → Preferences**
2. Add this URL to **Additional Board Manager URLs**:
3. Go to **Tools → Board → Board Manager**
4. Search for **ESP32** and click **Install**

#### Required Libraries (can be installed via Library Manager)
- `WiFi.h`
- `MFRC522.h`
- `BluetoothSerial.h`
- `SPI.h`

### 🚀 3. Clone and Upload the Firmware

```bash
git clone https://github.com/your-username/enigma-iot-cli.git
cd enigma-iot-cli

```
### 🚀 3. Clone and Upload the Firmware

### 🧭 Upload & Run ENIGMA Firmware

#### 1. Upload the Firmware

1. Open the firmware `.ino` or `.cpp` file in **Arduino IDE**.
2. Select the board:  
   **Tools → Board → ESP32 Dev Module**
3. Choose the appropriate **COM port** from  
   **Tools → Port**
4. Click the **Upload** button to flash the firmware.

---

#### 🖥️ 2. Run the Command-Line Interface

After uploading is complete, you can interact with the ENIGMA CLI:

##### 📟 Using Arduino IDE Serial Monitor:
- Go to: **Tools → Serial Monitor**
- Set **baud rate** to `115200`

##### 🖥️ Using Terminal (Linux/macOS/Windows):
```bash
screen /dev/ttyUSB0 115200
```
Replace /dev/ttyUSB0 with your actual device path.

🧪 What You Can Do with ENIGMA CLI
Once connected, you can execute the following operations in real time:

📡 Scan Wi-Fi networks

🔐 Read RFID tags

📶 Emulate Evil Twin access points

📲 Spams BLE signals

⏱️  periodic wireless scans

📊 Example Use Cases
✅ Simulate an Evil Twin access point
✅ Read and log RFID tag data
✅ BLE spam advertisements to test device filtering
✅ Schedule periodic wireless scans

⚠️ Disclaimer
This tool is intended strictly for educational and ethical hacking purposes.

Do not use ENIGMA to perform unauthorized attacks or invade others’ privacy.
Always obtain proper consent before testing networks or devices.

🔒 Use responsibly. Stay ethical. Hack smart.


