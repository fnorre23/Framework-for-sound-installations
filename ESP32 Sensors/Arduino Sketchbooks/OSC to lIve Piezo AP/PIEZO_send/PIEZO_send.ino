#include <WiFi.h>
#include <esp_now.h>

#define PIEZO_PIN 32
#define THRESHOLD 2000

uint8_t receiverAddress[] = {0xCC, 0x8D, 0xA2, 0xE0, 0xAF, 0x94};  // Change this


typedef struct Message {
  bool hit;
} Message;

Message msg;


 void printPiezovalue()
  {
    int piezoIs = analogRead(PIEZO_PIN);
    Serial.println(piezoIs);
    delay(10);
  }


void setup() {
  Serial.begin(115200);
  pinMode(PIEZO_PIN, INPUT);
  
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }


  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW sender ready");
}

void loop() {
  int piezoVal = analogRead(PIEZO_PIN);
  printPiezovalue();

  if (piezoVal > THRESHOLD) {
    msg.hit = true;
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t*)&msg, sizeof(msg));
    if (result == ESP_OK) {
      Serial.println("Hit sent!");
    } else {
      Serial.println("Send failed");
    }
    delay(10);  // Debounce
  }

  delay(10);
}
