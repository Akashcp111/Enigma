#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <FS.h>
#include <SPIFFS.h>

// RFID Pins
#define LED_PIN 13     // GPIO13 for LED
#define BUZZER_PIN 12  // GPIO12 for Buzzer
#define RST_PIN 22     // GPIO22 for RFID Reset
#define SS_PIN 5       // GPIO21 for RFID SDA/SS

class RFIDModule {
private:
  MFRC522 rfid;
  bool isActive;

public:
  RFIDModule() : rfid(SS_PIN, RST_PIN), isActive(false) {}

  void setup() {
    // Initialize pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    // Initialize RFID
    rfid.PCD_Init();
    Serial.println("RFID module initialized");
    Serial.println("Scan an RFID tag...");
    isActive = false;
  }

  void start() {
    isActive = true;
    Serial.println("RFID scanning activated");
  }

  void stop() {
    isActive = false;
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("RFID scanning deactivated");
  }

  void createProfile(String profileName) {
    Serial.println("Scan RFID tag to create profile...");
    unsigned long startTime = millis();
    
    while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
      if (millis() - startTime > 10000) {
        Serial.println("Timeout waiting for RFID tag.");
        return;
      }
      delay(100);
    }
    
    // Card detected, get UID
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uid += "0";
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    
    // Activate LED and Buzzer for feedback
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Save to file
    String filename = "/rfid_profiles.txt";
    File file = SPIFFS.open(filename, FILE_APPEND);
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }
    
    String entry = "Profile: " + profileName + ", UID: " + uid + "\n";
    if (file.print(entry)) {
      Serial.println("Profile saved: " + entry);
    } else {
      Serial.println("Failed to save profile");
    }
    file.close();
    
    // Halt PICC and stop encryption
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  void readProfiles() {
    String filename = "/rfid_profiles.txt";
    File file = SPIFFS.open(filename, FILE_READ);
    if (!file) {
      Serial.println("No RFID profiles found or failed to open file.");
      return;
    }
    
    Serial.println("Contents of rfid_profiles.txt:");
    while (file.available()) {
      Serial.write((char)file.read());
    }
    file.close();
  }

  void loop() {
    if (!isActive) return;
    
    // Check for new RFID cards
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
      digitalWrite(LED_PIN, LOW);    // LED off when no tag
      digitalWrite(BUZZER_PIN, LOW); // Buzzer off
      return;
    }
    
    // Card detected, get UID
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uid += "0";
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    
    // Check for profile in rfid_profiles.txt
    String profileName = "";
    bool profileFound = false;
    File file = SPIFFS.open("/rfid_profiles.txt", FILE_READ);
    if (file) {
      while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.indexOf("UID: " + uid) != -1) {
          // Extract profile name from line (format: "Profile: <name>, UID: <uid>")
          int profileStart = line.indexOf("Profile: ") + 9;
          int profileEnd = line.indexOf(", UID: ");
          if (profileStart != -1 && profileEnd != -1) {
            profileName = line.substring(profileStart, profileEnd);
            profileFound = true;
            break;
          }
        }
      }
      file.close();
    }
    
    // Display result
    if (profileFound) {
      Serial.println("Profile found: " + profileName + " (UID: " + uid + ")");
    } else {
      Serial.print("Card UID: ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println(" (No profile found)");
    }
    
    // Activate LED and Buzzer for feedback
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500); // Beep and light for 0.5 seconds
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Halt PICC and stop encryption
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(500); // Small delay before next scan
  }
};

#endif // RFID_H