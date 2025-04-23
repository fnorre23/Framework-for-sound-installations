/This code has been written with help from RandomNerdTutorials and some ChatGPT prompts
// The Sparkfun Linear Accelerometer code is from the example code from sparkfun BN0086 example Git repository found at https://github.com/sparkfun/SparkFun_VR_IMU_Breakout_BNO086_QWIIC
/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/




#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_BNO08x_Arduino_Library.h" // CTRL+Click here to get the library: http://librarymanager/All#SparkFun_BNO08x


// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x48, 0xCA, 0x43, 0xB7, 0xCF, 0x10};

BNO08x myIMU;

// Structure to send data
typedef struct struct_message {
    int id;
    float x ;
    float y ;
    float z ;
    float Acceleration ;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// For the most reliable interaction with the SHTP bus, we need
// to use hardware reset control, and to monitor the H_INT pin.
// The H_INT pin will go low when its okay to talk on the SHTP bus.
// Note, these can be other GPIO if you like.
// Define as -1 to disable these features.
//#define BNO08X_INT  A4
#define BNO08X_INT  -1
//#define BNO08X_RST  A5
#define BNO08X_RST  -1

#define BNO08X_ADDR 0x4B  // SparkFun BNO08x Breakout (Qwiic) defaults to 0x4B
//#define BNO08X_ADDR 0x4A // Alternate address if ADR jumper is closed


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Here is where you define the accelerometer sensor outputs you want to receive
void setReports(void) {
  Serial.println("Setting desired reports");
  if (myIMU.enableLinearAccelerometer() == true) {
    Serial.println(F("Linear Accelerometer enabled"));
    Serial.println(F("Output in form x, y, z, in m/s^2"));
  } else {
    Serial.println("Could not enable linear accelerometer");
  }
}


void CalculateSpeed()
{
float Acceleration = sqrt((myData.x*myData.x+ myData.y*myData.y+ myData.z*myData.z));
myData.Acceleration=Acceleration;

}

void setup() {
  Serial.begin(115200);
  
  // Connect to the ESPnow network
  WiFi.mode(WIFI_STA);
  WiFi.begin("ESP32now");
  Serial.print("Connecting to ESP32now network");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to ESPnow network");

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
//Accelerometer related
  while(!Serial) delay(10); // Wait for Serial to become available.
  // Necessary for boards with native USB (like the SAMD51 Thing+).
  // For a final version of a project that does not need serial debug (or a USB cable plugged in),
  // Comment out this while loop, or it will prevent the remaining code from running.
  
 Serial.println();
  Serial.println("BNO08x Read Example");

  Wire.begin();

  //if (myIMU.begin() == false) {  // Setup without INT/RST control (Not Recommended)
  if (myIMU.begin(BNO08X_ADDR, Wire, BNO08X_INT, BNO08X_RST) == false) {
    Serial.println("BNO08x not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1)
      ;
  }
  Serial.println("BNO08x found!");

    Wire.setClock(400000); //Increase I2C data rate to 400kHz

    setReports();


  myData.id = 11;
  Serial.print("Board ID: ");
  Serial.println(myData.id);

  Serial.println("Sending data");
  Serial.println("Reading events");
  //delay(100);
}


void loop() {
  //myData.distance = readSensor();
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }


  if (myIMU.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
  }

  // Has a new event come in on the Sensor Hub Bus?
  if (myIMU.getSensorEvent() == true) {

    // is it the correct sensor data we want?
    if (myIMU.getSensorEventID() == SENSOR_REPORTID_LINEAR_ACCELERATION) {

      myData.x = myIMU.getLinAccelX();
      myData.y = myIMU.getLinAccelY();
      myData.z = myIMU.getLinAccelZ();
      //byte linAccuracy = myIMU.getLinAccelAccuracy();

      Serial.print(myData.x, 2);
      Serial.print(F(","));
      Serial.print(myData.y, 2);
      Serial.print(F(","));
      Serial.print(myData.z, 2);
      //Serial.print(F(","));
      //Serial.print(linAccuracy);

      Serial.println();

  
    }
    CalculateSpeed();

  }

  delay(50);
}

