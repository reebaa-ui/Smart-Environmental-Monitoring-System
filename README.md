## Overview:

The Smart Environmental Monitoring System is an IoT-based environmental monitoring project developed using an ESP32 microcontroller, DHT11 temperature and humidity sensor, and MQ-9 gas sensor.

The system continuously monitors temperature, humidity, and air-quality-related gas sensor readings and displays the data in real time through a web-based dashboard.

## Features:

- Real-time temperature monitoring
- Real-time humidity monitoring
- MQ-9 gas sensor monitoring
- ESP32-based IoT system
- Wi-Fi connectivity
- Embedded HTTP web server
- Web-based monitoring dashboard
- Automatic sensor data updates every 2 seconds
- Automatic Wi-Fi reconnection
- Serial Monitor output for debugging

## Hardware used:

- ESP32 Development Board
- DHT11 Temperature and Humidity Sensor
- MQ-9 Gas Sensor
- Jumper Wires
- Breadboard
- USB Cable
- Power Supply

## Software used:

- Arduino/C++
- ESP32 Wi-Fi library
- ESP32 WebServer library
- DHT sensor library
- HTML
- CSS
- JavaScript

## System Working

The ESP32 collects temperature and humidity data from the DHT11 sensor and an analog gas reading from the MQ-9 sensor.

The ESP32 connects to a Wi-Fi network in Station (STA) mode and hosts an HTTP web server.

Users connected to the same network can access the ESP32's IP address through a web browser to view the sensor readings.

The dashboard automatically requests updated sensor data every two seconds.

The system also monitors the Wi-Fi connection and attempts to reconnect automatically if the connection is lost.

## Web Endpoints

### `/`

Displays the web-based monitoring dashboard.

### `/data`

Returns the latest sensor readings in JSON format.

Example:

```json
{
  "temperature": 28.4,
  "humidity": 62.1,
  "aqi": 734
}
