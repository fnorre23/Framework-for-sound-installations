#include <esp_now.h>
#include <WiFi.h>

// Replace with your receiver's MAC address
uint8_t receiverMac[] = { 0xA0, 0x85, 0xE3, 0xFB, 0x52, 0xF0 };  // <--- replace with your real MAC

typedef struct struct_message {
  float temperature;
  uint16_t co2;
  uint16_t tvoc;
} struct_message;

struct_message outgoingData;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (!esp_now_add_peer(&peerInfo)) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW sender (simulated CCS811) ready");
}

void loop() {
  outgoingData.temperature = 24.0 + random(-20, 20) * 0.1;  // Fake temperature (22–26°C)
  outgoingData.co2 = 400 + random(0, 200);                  // Simulated eCO2 (400–600 ppm)
  outgoingData.tvoc = 5 + random(0, 50);                    // Simulated TVOC (5–55 ppb)

  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&outgoingData, sizeof(outgoingData));
  if (result == ESP_OK) {
    Serial.printf("Sent: Temp=%.2f°C, CO2=%d ppm, TVOC=%d ppb\n",
                  outgoingData.temperature, outgoingData.co2, outgoingData.tvoc);
  } else {
    Serial.println("Failed to send");
  }

  delay(2000);
}
