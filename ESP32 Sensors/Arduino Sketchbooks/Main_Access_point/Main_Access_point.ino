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
typedef struct joystickStruct {
    int id;
    float valueX;
    float valueY;

} joystickStruct;

// Structure to receive data
typedef struct distancestruct {
    int id;
    float distance;
} distancestruct;

typedef struct myDataStruct {
    int id;
    float value;
} myDataStruct;

distancestruct distanceData;

joystickStruct joystickData;

myDataStruct myData;

// For sending ONE float or value
void SendToPD(float message, char* name) 
{
    Serial.print("Sending OSC: ");
    Serial.println(message);
    OSCMessage msg("/Sensordata");
    msg.add(message);
    pdudp.beginPacket(IPOut, OUT_PORT);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    //Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}

// overload method for sending TWO floats

void SendToPD(float message1, float message2, char* name) 
{
    Serial.print("Sending OSC: ");
    Serial.println(message1);
    Serial.println(message2);
    OSCMessage msg(name);
    msg.add(message1);
    msg.add(message2);
    pdudp.beginPacket(IPOut, OUT_PORT);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    //Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}

/*
// Callback function when ESP-NOW data is received
void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    //Serial.print("Bytes received: ");
    //Serial.println(len);
    //Serial.print("int: ");
    //Serial.println(myData.id);
    Serial.print("float: ");
    Serial.println(myData.value);

    // Send received ESP-NOW data over OSC
    SendToPD(myData.value, "/JoystickXY");

}
*/


void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
    // Read the first integer (ID) before knowing the full structure
    int receivedID;
    memcpy(&receivedID, incomingData, sizeof(int));  // Extract only the first integer

    // Serial.println("Listening");

    if(receivedID == 1 && len == sizeof(joystickStruct))
    {
      Serial.println("Received ID: 1");
      memcpy(&joystickData, incomingData, sizeof(joystickStruct));

      Serial.println(joystickData.valueX);
    }

    if(receivedID == 2 && len == sizeof(distancestruct))
    {
       Serial.println("Received ID: 2");
       memcpy(&distanceData, incomingData, sizeof(distancestruct));

       Serial.println(distanceData.distance);
    }
}


// Print the correct MAC address
void printMacAddress() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    //Serial.printf("STA MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
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
