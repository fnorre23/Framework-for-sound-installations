#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0xB8, 0xD6, 0x1A, 0x0E, 0x17, 0x0C};

// Structure to send data
typedef struct struct_message {
    int id;
    float value;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// Sensor variables
#include "Seeed_CY8C401XX.h"

#ifdef SEEED_XIAO_M0
    #define SERIAL Serial
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif
int valueSum;
int lastValue;
CY8C sensor;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

  // Specific to touch sensor
  sensor.init();

  lastValue = 0;
}

// Update with specific sensor logic
float readSensor()
{
  u8 value = 0;
    sensor.get_touch_slider_value(&value);

    if(lastValue < value)
    {
      valueSum += (value / 10);
      if(valueSum > 1000)
      {
        valueSum = 1000;
      }
    }
    else if(lastValue > value)
    {
      valueSum -= (value / 10);
      if(valueSum < 50)
      {
        valueSum = 50;
      }
    }

    lastValue = value;

    SERIAL.println(valueSum);

  return valueSum;
}

void loop() {
  myData.id = 3;
  myData.value = readSensor();
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  delay(10);
}
