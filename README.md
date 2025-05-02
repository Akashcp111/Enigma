# 🔐 ENIGMA – CLI-Based IoT Security & Wireless Attack Simulator

**ENIGMA** is a command-line IoT security toolkit powered by the **ESP32 NodeMCU**, built for real-time wireless signal monitoring, basic attack simulation, and hands-on cybersecurity research. It provides an educational, modular platform for ethical hackers, students, and professionals to safely explore wireless vulnerabilities.

> ⚠️ For **ethical and educational** use only. Do not use this tool for unauthorized activities.

--


## 🚀 Features

### Wi-Fi Security & Monitoring
- Wi-Fi Scanner
- Evil twin attack
- Hardcoded Evil Twin Attack

### Bluetooth Surveillance
- Iphone BLE spamming

### RFID/NFC Access Control
- RFID Tag Reader (MFRC522)
- UID storage with Profile

### Real-Time CLI Control
- Full Command-Line Operation
---

## 🧰 Hardware Requirements

| Component           | Description                    |
|--------------------|--------------------------------|
| ESP32 NodeMCU       | Main Microcontroller (Wi-Fi + BLE) |
| MFRC522 RFID Module | RFID tag reader module         |
| Buzzer, LEDs        | For alerts and interaction     |
| Push Button         | Trigger actions                |
| Breadboard & Wires  | Prototyping setup              |
| USB Cable           | Power + Serial Communication   |

---

## 💻 Software Components

- **Custom ESP32 Firmware** (C++) for wireless scanning and attack emulation  
- **Command-Line Interface** for real-time control via Serial Terminal  
- **Event Logger** for tracking actions and signal events  
- **Task Scheduler** for timed scans and auto actions  
- **RFID, Wi-Fi, and Bluetooth drivers** integrated  
- Compatible with **macOS/Linux** (via Serial Terminal)

---

## 🔧 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/enigma-iot-cli.git
cd enigma-iot-cli
