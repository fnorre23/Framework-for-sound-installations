# ESP32_easy_wifi_data

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP32_easy_wifi_data.svg?)](https://www.ardu-badge.com/ESP32_easy_wifi_data)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/joshua1024/library/ESP32_easy_wifi_data.svg)](https://registry.platformio.org/libraries/joshua1024/ESP32_easy_wifi_data)

Simple library for sending and receiving booleans, bytes, integers, and float variables over UDP. The esp32 can be connected to a wifi network or create its own hotspot.

If you set up port forwarding on your wifi router you can control your esp32 from anywhere!

Note: Communication with this library is not secured or error checked, don't use it to control anything dangerous or private.

Now works with ESP8266 chips too! (After Version 1.2.0)

Used by https://github.com/rcmgames/rcmv2 (hardware and software for small wifi controlled robots)

[This](https://github.com/RCMgames/RCMDS) and [this](https://github.com/RCMgames/RCMDS-new) are programs that can be used for sending data to this library from a computer or Android phone.

Now allows two boards to communicate with eachother (after version 1.4.0), see examples.
