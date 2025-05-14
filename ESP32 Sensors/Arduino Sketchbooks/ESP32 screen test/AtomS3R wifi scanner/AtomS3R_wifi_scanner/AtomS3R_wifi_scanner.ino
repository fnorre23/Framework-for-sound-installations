#include <M5AtomS3.h>
#include <WiFi.h>

const char* targetSSID = "ESP32now";

void setup() {
  auto cfg = M5.config();
  cfg.clear_display = true;
  cfg.output_power = true;
  M5.begin(cfg);

  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Scanning...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void drawSignalBar(int rssi) {
  int barWidth = map(rssi, -100, -30, 0, 200);  // scale RSSI to pixel width
  barWidth = constrain(barWidth, 0, 200);

  M5.Lcd.fillRect(20, 80, 200, 20, DARKGREY);     // background bar
  M5.Lcd.fillRect(20, 80, barWidth, 20, GREEN);   // signal bar

  M5.Lcd.setCursor(20, 110);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("RSSI: %d dBm", rssi);
}

void loop() {
  int n = WiFi.scanNetworks();
  bool found = false;

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == targetSSID) {
      int rssi = WiFi.RSSI(i);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(20, 40);
      M5.Lcd.setTextSize(2);
      M5.Lcd.printf("SSID: %s", targetSSID);
      drawSignalBar(rssi);
      found = true;
      break;
    }
  }

  if (!found) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 60);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.println("ESP32now not");
    M5.Lcd.println("found");
  }

  delay(3000);  // refresh every 3 seconds
}
