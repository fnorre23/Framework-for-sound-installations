//This code has been written with help from RandomNerdTutorials and some ChatGPT prompts

/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

// Access Point credentials
const char* ssid = "ESP32now";

// UDP settings - This is for sending OSC messages
WiFiUDP pdudp; // Define UDP object for OSC
IPAddress IPOut(192, 168, 4, 2); // Destination IP for sending OSC

// Each sensor sending on a different port
#define OUT_PORT 3001

// Array with values
float distArray[2];

const int arrayLength = 2;

volatile bool newDataAvailable = false;

// Structure to receive distance sensor data
typedef struct dataStruct {
    int id;
    float distance;
} dataStruct;

// Creating instance of the struct to temporarily store data
dataStruct data;

// For sending the array
void SendToPD(float message[arrayLength], char* name, int port) 
{
    OSCMessage msg(name);

    for(int i = 0; i < arrayLength; i++)
    {
      float floatValue;

      if(message[i] < 25)
      {
        long value = map(message[i], 0, 25, 0, 100);

        floatValue = (float)value / 100;

        if(floatValue > 1)
        {
          floatValue = 1;
        }
      }
      else
      {
        floatValue = 1;
      }

      Serial.println(floatValue);

      msg.add(floatValue);
    }

    pdudp.beginPacket(IPOut, port);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    //Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) 
{  
  // Copy received data into the struct
    memcpy(&data, incomingData, sizeof(data));

  // Set bool so the data gets sent to PD in main loop
    newDataAvailable = true;
}


// Print the correct MAC address
void printMacAddress() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.printf("STA MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void setup() {
    Serial.begin(115200);

    // Set ESP32 as both Access Point and Station
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid);
    delay(1000);

    Serial.println("ESP32 Access Point Started");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Print the station MAC address (use this in the sender ESP)
    printMacAddress();

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    Serial.println(arrayLength);

    // Start receiving data
    esp_now_register_recv_cb(OnDataRecv);

    // Start UDP
    //pdudp.begin(UDP_PORT);
}


void loop() 
{
  // Handle sending data in main loop
    if(newDataAvailable)
    {
      newDataAvailable = false;

      distArray[data.id - 1] = data.distance;

      SendToPD(distArray, "/distArray", OUT_PORT);
    }
}
