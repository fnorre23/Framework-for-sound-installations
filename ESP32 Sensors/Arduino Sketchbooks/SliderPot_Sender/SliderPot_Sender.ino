#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x48, 0xCA, 0x43, 0xB7, 0xD1, 0xD8};

// Structure to send data
typedef struct struct_message {
    int id;
    float value;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// Sensor variables
#define DATAINPUT 1

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

  pinMode(DATAINPUT, INPUT);
}

float readSensor() {
  float value = analogRead(DATAINPUT);
  
  Serial.println(value);

  value = map(value, 0, 4095, 50, 1000);


  return value;
}

void loop() {
  myData.id = 3;
  myData.value = readSensor();
  //myData.pressed = readButton();

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  } else {
    //Serial.println("Error sending the data");
  }

  delay(10);
}
