//This code has been written with help from RandomNerdTutorials and some ChatGPT prompts
// The Sparkfun Linear Accelerometer code is from the example code from sparkfun BN0086 example Git repository found at https://github.com/sparkfun/SparkFun_VR_IMU_Breakout_BNO086_QWIIC
/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "esp_adc_cal.h"

// ESP-NOW MAC Address of receiver
uint8_t broadcastAddress[] = {0x48, 0xCA, 0x43, 0xB7, 0xD1, 0xB4};

// BNO055 sensor instance
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);  // Default I2C address 0x28

// Structure to send data
typedef struct struct_message {
  int id;
  float x;
  float y;
  float z;
  float Acceleration;

  float E_x;
  float E_y;
  float E_z;

  //float q_w;
  //float q_x;
  //float q_y;
  //float q_z;

} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// ESP-NOW callback
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void ReadLiniarAcceleration()

{
//code to read the acceleration Vector 
imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);


  myData.x = acc.x();
  myData.y = acc.y();
  myData.z = acc.z();

}





/*void ReadEuler()

{

imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  
  myData.E_x = euler.x();
  myData.E_y = euler.y();
  myData.E_z = euler.z();




/* Display the floating point data

Serial.print("Euler_X: ");
Serial.print(euler.x());
Serial.print(" Euler_Y: ");
Serial.print(euler.y());
Serial.print(" Euler_z: ");
Serial.print(euler.z());
Serial.println(""); 
}
*/


/*void ReadQuaternion()
{
imu::Quaternion quat =bno.getQuat();

  myData.q_w=quat.w();
  myData.q_x=quat.z();
  myData.q_y=quat.y();
  myData.q_z=quat.z();


}
*/


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nESP-NOW + BNO-055");

  // Init WiFi in STA mode for ESP-NOW
  WiFi.mode(WIFI_STA);

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

  // Initialize BNO055
  if (!bno.begin()) {
    Serial.println("BNO055 not detected. Check wiring.");
    while (1);
  }

  delay(1000);  // Let sensor initialize

  bno.setExtCrystalUse(true);

  myData.id = 11;
  Serial.print("Board ID: ");
  Serial.println(myData.id);
}

void loop() {



sensors_event_t event;

    bno.getEvent(&event);
    Serial.println(event.orientation.x);
    Serial.println(event.orientation.y);
    Serial.println(event.orientation.z);
    myData.E_x= event.orientation.x;
    myData.E_y= event.orientation.y;
    myData.E_z= event.orientation.z;
    



  /* Also send calibration data for each sensor. */
uint8_t sys, gyro, accel, mag = 0;
  bno.getCalibration(&sys, &gyro, &accel, &mag);
  Serial.print(F("Calibration: "));
  Serial.print(sys, DEC);
  Serial.print(F(" "));
  Serial.print(gyro, DEC);
  Serial.print(F(" "));
  Serial.print(accel, DEC);
  Serial.print(F(" "));
  Serial.println(mag, DEC);

int8_t boardTemp = bno.getTemp();
  Serial.println();
  Serial.print(F("temperature: "));
  Serial.println(boardTemp);

  //imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  //myData.x = acc.x();
  //myData.y = acc.y();
  //myData.z = acc.z();

  ReadLiniarAcceleration();
  //ReadEuler();
  //ReadQuaternion();


  // Calculate magnitude of acceleration
  myData.Acceleration = sqrt(myData.x * myData.x + myData.y * myData.y + myData.z * myData.z);

  Serial.print("X: "); Serial.print(myData.x, 2);
  Serial.print(" Y: "); Serial.print(myData.y, 2);
  Serial.print(" Z: "); Serial.print(myData.z, 2);
  Serial.print(" | Accel Mag: "); Serial.println(myData.Acceleration, 2);
  //Serial.print("q_w");Serial.println(myData.q_w);



  // Send data via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  if (result != ESP_OK) {
    Serial.println("Error sending data");
  }

  delay(100);
}
