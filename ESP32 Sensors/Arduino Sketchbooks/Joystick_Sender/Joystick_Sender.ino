//This code has been written with help from RandomNerdTutorials and some ChatGPT prompts

/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/




#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x7C, 0xDF, 0xA1, 0x55, 0xF8, 0x6A};

// Structure to send data
typedef struct struct_message {
    int id;
    float valueX;
    float valueY;
    float pressed;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// Sensor variables
#define JOYX A1
#define JOYY A3
#define JOYPRESS 13

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  
  // Connect to the ESPnow network
  WiFi.mode(WIFI_STA);
  WiFi.begin("ESP32now");
  Serial.println("Connecting to ESP32now network");
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

  pinMode(JOYX, INPUT);
  pinMode(JOYY, INPUT);
  pinMode(JOYPRESS, INPUT);
}

float readSensorX() {
  float valueX = analogRead(JOYX);

  valueX = map(valueX, 0, 4095, 50, 1000);

  return valueX;
}

float readSensorY() {
  float valueY = analogRead(JOYY);

  valueY = map(valueY, 0, 4095, 50, 1000);

  return valueY;
}


float readButton()
{
  int buttonState = 0;
  buttonState = digitalRead(JOYPRESS);
  
  Serial.println(buttonState);

  if (buttonState == HIGH) 
  {
    return 0;
  } 
  else 
  {
    return 1;
  }

}

void loop() {
  myData.id = 1;
  myData.valueX = readSensorX();
  myData.valueY = readSensorY();
  myData.pressed = readButton();


  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  } else {
    //Serial.println("Error sending the data");
  }

  delay(10);
}
