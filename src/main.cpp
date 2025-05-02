#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>

// Include our module headers
#include "rfid.h"
#include "wifi_tool.h"
#include "ble_spoofer.h"

// Create instances of each module
RFIDModule rfidModule;
WiFiTool wifiTool;
BLESpoofer bleSpoofer;

// Enum for active module
enum ActiveModule {
  NONE,
  RFID,
  WIFI,
  BLE
};

ActiveModule currentModule = NONE;

// Function prototypes
void displayMainMenu();
void processSerialCommand();

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000); // Give time for serial to connect
  
  Serial.println("\n==== ESP32 Multi-Tool ====");
  Serial.println("Version 1.8"); // Updated version to reflect profile lookup on scan
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS initialized");

  // Initialize SPI for RFID
  SPI.begin(); // SPI pins: SCK=18, MOSI=23, MISO=19
  
  // Initialize all modules
  Serial.println("Initializing modules...");
  rfidModule.setup();
  wifiTool.setup();
  bleSpoofer.setup();
  
  displayMainMenu();
}

void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    processSerialCommand();
  }
  
  // Run the active module's loop function
  switch (currentModule) {
    case RFID:
      rfidModule.loop();
      break;
    case WIFI:
      wifiTool.loop();
      break;
    case BLE:
      bleSpoofer.loop();
      break;
    default:
      // Nothing active
      break;
  }
  
  // Small delay to prevent CPU hogging
  delay(10);
}

void displayMainMenu() {
  Serial.println("\n===== MAIN MENU =====");
  Serial.println("1. RFID Module");
  Serial.println("2. WiFi Tool");
  Serial.println("3. BLE Spoofer");
  Serial.println("0. Return to Main Menu (from any submenu)");
  Serial.println("h. Help/Menu");
  Serial.println("====================");
  Serial.println("Enter your choice:");
}

void displayRFIDMenu() {
  Serial.println("\n===== RFID MODULE MENU =====");
  Serial.println("1. Start RFID Scanning");
  Serial.println("2. Stop RFID Scanning");
  Serial.println("3. Create RFID Profile");
  Serial.println("4. Read RFID Profiles");
  Serial.println("0. Return to Main Menu");
  Serial.println("==========================");
  Serial.println("Enter your choice:");
}

void displayBLEMenu() {
  Serial.println("\n===== BLE SPOOFER MENU =====");
  Serial.println("1. Start BLE Spoofing");
  Serial.println("2. Stop BLE Spoofing");
  Serial.println("3. Set Delay (milliseconds)");
  Serial.println("0. Return to Main Menu");
  Serial.println("===========================");
  Serial.println("Enter your choice:");
}

void displayWiFiMenu() {
  Serial.println("\n===== WIFI TOOL MENU =====");
  Serial.println("1. Scan for WiFi Networks");
  Serial.println("2. Start Evil Twin Attack");
  Serial.println("3. Start Beacon Flooding");
  Serial.println("4. Start Evil Twin with Captive Portal");
  Serial.println("5. Stop Current Operation");
  Serial.println("0. Return to Main Menu");
  Serial.println("=========================");
  Serial.println("Enter your choice:");
}

void processSerialCommand() {
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  // Check if user wants to return to main menu
  if (input == "0") {
    // Deactivate any running modules
    rfidModule.stop();
    wifiTool.stopOperation();
    bleSpoofer.stop();
    currentModule = NONE;
    displayMainMenu();
    return;
  }
  
  // Help command works anywhere
  if (input == "h") {
    switch (currentModule) {
      case NONE:
        displayMainMenu();
        break;
      case RFID:
        displayRFIDMenu();
        break;
      case WIFI:
        displayWiFiMenu();
        break;
      case BLE:
        displayBLEMenu();
        break;
    }
    return;
  }
  
  // Process based on current module/menu
  switch (currentModule) {
    case NONE:
      // Main menu
      if (input == "1") {
        currentModule = RFID;
        displayRFIDMenu();
      } else if (input == "2") {
        currentModule = WIFI;
        displayWiFiMenu();
      } else if (input == "3") {
        currentModule = BLE;
        displayBLEMenu();
      } else {
        Serial.println("Invalid option. Enter 'h' to see the menu.");
      }
      break;
      
    case RFID:
      // RFID Module menu
      if (input == "1") {
        rfidModule.start();
      } else if (input == "2") {
        rfidModule.stop();
      } else if (input == "3") {
        Serial.println("Enter profile name (max 20 chars):");
        unsigned long startTime = millis();
        while (!Serial.available()) {
          if (millis() - startTime > 10000) {
            Serial.println("Timeout waiting for profile name input.");
            displayRFIDMenu();
            return;
          }
          delay(10);
        }
        String profileName = Serial.readStringUntil('\n');
        profileName.trim();
        if (profileName.length() > 20 || profileName.length() == 0) {
          Serial.println("Invalid profile name. Must be 1-20 characters.");
        } else {
          rfidModule.createProfile(profileName);
        }
        displayRFIDMenu();
      } else if (input == "4") {
        rfidModule.readProfiles();
        displayRFIDMenu();
      } else {
        Serial.println("Invalid RFID option. Enter 'h' to see the menu.");
      }
      break;
      
    case WIFI:
      // WiFi Tool menu
      if (input == "1") {
        wifiTool.scanWiFi();
        displayWiFiMenu();
      } else if (input == "2") {
        wifiTool.startEvilTwin();
      } else if (input == "3") {
        wifiTool.startBeaconFlood();
        displayWiFiMenu();
      } else if (input == "4") {
        wifiTool.startEvilTwinWithPortal();
      } else if (input == "5") {
        wifiTool.stopOperation();
        displayWiFiMenu();
      } else {
        Serial.println("Invalid WiFi Tool option. Enter 'h' to see the menu.");
      }
      break;
      
    case BLE:
      // BLE Spoofer menu
      if (input == "1") {
        bleSpoofer.start();
        displayBLEMenu();
      } else if (input == "2") {
        bleSpoofer.stop();
        displayBLEMenu();
      } else if (input == "3") {
        Serial.println("Enter delay in milliseconds:");
        unsigned long startTime = millis();
        while (!Serial.available()) {
          if (millis() - startTime > 10000) {
            Serial.println("Timeout waiting for delay input.");
            break;
          }
          delay(10);
        }
        if (Serial.available()) {
          uint32_t newDelay = Serial.parseInt();
          Serial.read(); // Clear newline
          if (newDelay > 0) {
            bleSpoofer.setDelay(newDelay);
          } else {
            Serial.println("Invalid delay value.");
          }
        }
        displayBLEMenu();
      } else {
        Serial.println("Invalid BLE Spoofer option. Enter 'h' to see the menu.");
      }
      break;
  }
}