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
#define OUT_PORT_DIST 3001
#define OUT_PORT_ACC 3002
#define OUT_PORT_JOY 3003

// Distance sensors
float distArray[2];
const int arrayLength = 2;
float maxSensorDistance = 50;
volatile bool newDistDataAvailable = false;

// Structure to receive distance sensor data
typedef struct distStruct {
    int id;
    float distance;
} distStruct;

bool updateArrayCheck[arrayLength];

// Acceleromneter Sensor
volatile bool newballDataAvailable = false;
int ballArrayLength = 7;

typedef struct ballStruct {
    int id;
    float ax, ay, az;
    float acceleration;
    float ex, ey, ez;
} ballStruct;

// Joystick sensor
typedef struct joyStruct {
  int id;
  float valueX;
  float valueY;
  float pressed;
} joyStruct;

volatile bool newJoyDataAvailable = false;

// Creating instance of the struct to temporarily store data
distStruct distData;
ballStruct ballData;
joyStruct joyData;

// For sending the array
void SendToArrayPD(float message[], char* name, int port, int length) 
{
    OSCMessage msg(name);
  
    for(int i = 0; i < length; i++)
    {
      float floatValue=message[i];
      msg.add(floatValue);
    }

    pdudp.beginPacket(IPOut, port);
    msg.send(pdudp);

    int success = pdudp.endPacket();
    msg.empty();
    // Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}

void SendAccToPD(float message[], char* name, int port)
{
  OSCMessage msg(name);

    for(int i = 0; i < ballArrayLength; i++)
    {
      msg.add(message[i]);
    }

    pdudp.beginPacket(IPOut, port);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    //Serial.println(success ? "OSC Sent!" : "OSC Failed!");

}

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) 
{  
  int receivedID;
  memcpy(&receivedID, incomingData, sizeof(int));  // Extract only the first integer

  if(receivedID <= 10)
  {
    // Copy received data into the struct
    memcpy(&distData, incomingData, sizeof(distData));

    // Set bool so the data gets sent to PD in main loop
    newDistDataAvailable = true;
  }
  else if(receivedID == 11)
  {
    // Copy received data into the struct
    memcpy(&ballData, incomingData, sizeof(ballData));

    // Set bool so the data gets sent to PD in main loop
    newballDataAvailable = true;
  }
  else if(receivedID == 12)
  {
    // Copy received data into the struct
    memcpy(&joyData, incomingData, sizeof(joyData));

    // Set bool so the data gets sent to PD in main loop
    newJoyDataAvailable = true;
  }
  else
  {
    Serial.println("ID not valid or something");
  }
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

    Serial.print("Distance sensor count: ");
    Serial.println(arrayLength);

    // Start receiving data
    esp_now_register_recv_cb(OnDataRecv);

    // Start UDP
    //pdudp.begin(UDP_PORT);
}

void loop() 
{
  // Handle sending data in main loop
  if (newDistDataAvailable)
  {
    newDistDataAvailable = false;

    distArray[distData.id - 1] = distData.distance;

    updateArrayCheck[distData.id - 1] = true;

    // Check if all bool values in updateArrayCheck are true
    bool allTrue = true;
    for (int i = 0; i < sizeof(updateArrayCheck) / sizeof(updateArrayCheck[0]); i++)
    {
      if (!updateArrayCheck[i])
      {
        allTrue = false;
        break;
      }
    }

    // If all values are true, send data to PD
    if (allTrue)
    {
      for (int i = 0; i < arrayLength + 1; i++)
      {
        updateArrayCheck[i] = false;
      }

      Serial.println("All distances received, sending data");

      SendToArrayPD(distArray, "/distArray", OUT_PORT_DIST, arrayLength);
    }
    delay(10);
  }


  if (newballDataAvailable)
  {
    newballDataAvailable = false;

    float accArray[ballArrayLength] = {ballData.ax, ballData.ay, ballData.az, ballData.acceleration, ballData.ex, ballData.ey, ballData.ez};

    /*
    Serial.print("Acceleration: ");
    Serial.print(ballData.acceleration);
    Serial.print(",");
    Serial.print("X: ");
    Serial.println(ballData.ex);
    */

    Serial.println("Sending Gyro data");
    SendAccToPD(accArray, "/accelerometer", OUT_PORT_ACC);
  }
  
  if(newJoyDataAvailable)
  {
    newJoyDataAvailable = false;

    float joyArray[3] = {joyData.valueX, joyData.valueY, joyData.pressed};

    Serial.println("Sending joystick");
    SendToArrayPD(joyArray, "/JoystickXYP", OUT_PORT_JOY, 3);
  }

}
