#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

// Access Point credentials
const char* ssid = "ESP32now";

// UDP settings - This is for sending OSC messages
WiFiUDP pdudp; // Define UDP object for OSC
IPAddress IPOut(192, 168, 4, 2); // Destination IP for sending OSC

// Each sensor sending on a different port
#define OUT_PORT_JOY 3001
#define OUT_PORT_DIST 3002
#define OUT_PORT_SLID 3003

// Structure to receive joystick data
typedef struct joystickStruct {
    int id;
    float valueX;
    float valueY;
    //bool pressed;
} joystickStruct;

// Structure to receive distance sensor data
typedef struct distancestruct {
    int id;
    float distance;
} distancestruct;

typedef struct sliderStruct {
  int id;
  float value;
} sliderStruct;

// Creating instances of the structs to temporarily store data
joystickStruct joystickData;
distancestruct distanceData;
sliderStruct sliderData;

// For sending ONE float or value
void SendToPD(float message, char* name, int port) 
{
    Serial.print("Sending OSC: ");
    Serial.println(message);
    OSCMessage msg(name);
    msg.add(message);
    pdudp.beginPacket(IPOut, port);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    //Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}

// Overload method for sending TWO floats
void SendToPD(float message1, float message2, char* name, int port) 
{
    Serial.print("Sending OSC: ");
    Serial.println(message1);
    Serial.println(message2);
    OSCMessage msg(name);
    msg.add(message1);
    msg.add(message2);
    pdudp.beginPacket(IPOut, port);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    //Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}

// Overload method for sending TWO floats and a bool
void SendToPD(float message1, float message2, bool pressed,char* name, int port) 
{
    //Serial.print("Sending OSC: ");
    //Serial.println(message1);
    //Serial.println(message2);
    OSCMessage msg(name);
    msg.add(message1);
    msg.add(message2);
    msg.add(pressed);
    pdudp.beginPacket(IPOut, port);
    msg.send(pdudp);
    int success = pdudp.endPacket();
    msg.empty();
    //Serial.println(success ? "OSC Sent!" : "OSC Failed!");
}

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
    
    // Read the first integer (ID) before knowing the full structure
    int receivedID;
    memcpy(&receivedID, incomingData, sizeof(int));  // Extract only the first integer

    // Based on the integer, which is the ID of the board, read the data as a specific type of struct.
    // If the ID = 1, then we know we are receiving from joystick, therefore two values 
    // if ID = 2, distancesensor ect.
    // ALSO: Each different ID has to send on a unique port to receive it properly in PureData

    // ---------- OVERVIEW OF BOARDS -------------
    // ID = 1 = Joystick board
    // ID = 2 = Distance board
    // ID = 3 = Slider board

    if(receivedID == 1 && len == sizeof(joystickStruct))
    {
      Serial.println("Received ID: 1");
      memcpy(&joystickData, incomingData, sizeof(joystickStruct));

      //Serial.println(joystickData.pressed);

      SendToPD(joystickData.valueX, joystickData.valueY, "/joystickXY", OUT_PORT_JOY);

      Serial.println(joystickData.valueX);
      Serial.println(joystickData.valueY);
    }

    if(receivedID == 2 && len == sizeof(distancestruct))
    {
       Serial.println("Received ID: 2");
       memcpy(&distanceData, incomingData, sizeof(distancestruct));

       SendToPD(distanceData.distance, "/distance", OUT_PORT_DIST);

       //Serial.println(distanceData.distance);
    }

    if(receivedID == 3 && len == sizeof(sliderStruct))
    {
        Serial.println("Received ID: 3");
        memcpy(&sliderData, incomingData, sizeof(sliderStruct));

        SendToPD(distanceData.distance, "/slider", OUT_PORT_SLID);
    }
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
    //printMacAddress();

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Start receiving data
    esp_now_register_recv_cb(OnDataRecv);

    // Start UDP
    //pdudp.begin(UDP_PORT);
}


void loop() {
    // Nothing needed here, everything runs in the callback
}
