# Smart Greenhouse System (ESP32)

## About this project

This is a small smart greenhouse project I built using an ESP32.
The idea is to monitor temperature, humidity, and light, and automatically adjust grow lights depending on the crop.

It also has a simple web dashboard where you can see live data and choose which crop you’re growing.

---

## What it does

* Reads temperature and humidity using a DHT11 sensor
* Uses an LDR to check how much light is available
* Controls two RGB LEDs (for two crop sections)
* Adjusts light color and brightness based on:

  * Crop type
  * Light level
  * Temperature
* Shows everything on a web page (hosted by ESP32)

---

## Crops supported

Right now, I’ve added:

* Lettuce
* Tomato
* Spinach
* Strawberry

Each one has slightly different lighting behavior.

---

## Components used

* ESP32
* DHT11 sensor
* LDR
* 2 RGB LEDs (common anode)
* Resistors
* Breadboard and wires

---

## Pin setup

**DHT11:** GPIO 4
**LDR:** GPIO 34

**RGB LED 1**

* R → 27
* G → 26
* B → 25

**RGB LED 2**

* R → 22
* G → 21
* B → 19

---

## How to use

1. Upload the code to ESP32
2. Update your WiFi name and password in the code
3. Open Serial Monitor to find the IP address
4. Enter that IP in your browser
5. Use the dashboard to:

   * Select crops
   * Monitor temperature, humidity, and light

---

## Notes

* LEDs are common anode, so logic is inverted
* LDR readings are averaged for stability
* Sensor updates every second

---

## Future ideas

Some things I might add later:

* Soil moisture sensor
* Automatic watering system
* Mobile app instead of web page
* Data logging

---

## Author

Made by Anupriya Singh and Ssunavya Jain
---
