#include <WiFi.h>
#include <esp_now.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

WiFiUDP udp;

const char* ssid = "ESP32now2";
IPAddress IPOut(192, 168, 4, 2);
const int oscPort = 3004;

typedef struct Message {
  bool hit;
} Message;

void sendOSCTrigger() {
  OSCMessage msg("/hit");
  msg.add((int32_t)1);
  udp.beginPacket(IPOut, oscPort);
  msg.send(udp);
  udp.endPacket();
  msg.empty();
  Serial.println("OSC sent");
}

void onReceive(const esp_now_recv_info_t* recvInfo, const uint8_t* data, int len) {
  if (len != sizeof(Message)) return;
  Message incoming;
  memcpy(&incoming, data, sizeof(incoming));
  if (incoming.hit) {
    Serial.println("Hit received!");
    sendOSCTrigger();
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid);
  delay(500);  // Reduced delay
  Serial.println("ESP32 Access Point Started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  udp.begin(8000);  // Local UDP port
}

void loop() {
  // No delay — leave loop empty for responsiveness
}
