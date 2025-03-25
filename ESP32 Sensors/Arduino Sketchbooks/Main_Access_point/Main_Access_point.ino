#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

// Access Point credentials
const char* ssid = "ESP32now";

// UDP settings - This is for sending OSC messages
//#define UDP_PORT 3000
WiFiUDP pdudp; // Define UDP object for OSC

IPAddress IPOut(192, 168, 4, 2); // Destination IP for sending OSC
#define OUT_PORT 3001



// Structure to receive data
typedef struct struct_message {
    int id;
    float valueX;
    float valueY;

} struct_message;


struct_message myData;


void SendToPD(float message) {
    Serial.print("Sending OSC: ");
    Serial.println(message);
    OSCMessage msg("/sensordata");
    msg.add(message);
    pdudp.beginPacket(IPOut, OUT_PORT);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}


// Callback function when ESP-NOW data is received
void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("int: ");
    Serial.println(myData.id);
    Serial.print("float: ");
    Serial.println(myData.valueX);
    Serial.print("float: ");
    Serial.println(myData.valueY);

    // Send received ESP-NOW data over OSC
    SendToPD(myData.valueX);
    SendToPD(myData.valueY);

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

    esp_now_register_recv_cb(OnDataRecv);

    // Start UDP
    //pdudp.begin(UDP_PORT);
}


void loop() {
    // Nothing needed here, everything runs in the callback
}
