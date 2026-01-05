Weather Station: ESP32 + Calypso Ultrasonic + BME680
A Bluetooth‑enabled wind and environmental monitoring station using an ESP32 (8MB Flash, 8MB PSRAM), PlatformIO, and the Arduino framework.

Overview

This project implements a compact weather station focused primarily on wind measurement using the Calypso Ultrasonic anemometer, complemented by temperature, humidity, and pressure readings from a BME680 sensor.
The ESP32 retrieves wind data over BLE, aggregates values, and periodically posts results to a secure API using HTTPS + JWT authentication. Additional environmental data and battery levels are also reported at regular intervals.
An online dashboard visualizes the collected data using Google Charts.

Hardware
Calypso Ultrasonic Anemometer (Bluetooth Portable)
- https://www.calypsoinstruments.com/shop/cmi1006-ultrasonic-portable-solar-wind-meter-2?category=1
- https://www.calypsoinstruments.com/web/content/482617?unique=df6beb68052065b7dfd6208d7d5bdcc31082f9fb&download=true&access_token=65da349e-1edd-49a6-bbe0-3388beb14a2d
Grove BME680 Sensor (Temperature, Humidity, Pressure, Gas)
- https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/
ESP32 Dev Kit (External WiFi/BLE Antenna)
- Model: ESP32‑WROVER‑IE‑N8R8
- Flash: 8MB (QuadSPI)
- PSRAM: 8MB (QuadSPI)
- Datasheet: https://documentation.espressif.com/esp32-wrover-e_esp32-wrover-ie_datasheet_en.pdf

System Architecture
Wind Sampling
- ESP32 polls the Calypso Ultrasonic every second for 4 minutes 50 seconds.
- Wind speed and direction are aggregated.
- Maximum gust is tracked for the entire sampling period.
Data Upload
- Every 5 minutes:
Wind data is posted via HTTPS to a JWT‑secured API (Auth0 token retrieval).
- Every hour:
Ultrasonic battery level + BME680 pressure, temperature, and humidity are posted.
Dashboard
A live dashboard displays the collected data:
https://wapi.greenwoods.ch/weather.html
- Built using Google Charts
- Retrieves datasets directly from the API
API Endpoints
- Wind data:
https://wapi.greenwoods.ch/weatherdata/
- Pressure/Temperature/Humidity + Battery:
https://wapi.greenwoods.ch/battery-PTH/
Windy.com Integration
The API server also forwards data to Windy.com, enabling hosting of a personal weather station on their platform.

Repository Scope
This repository focuses on:
- ESP32 firmware
- BLE communication with the Calypso Ultrasonic
- BME680 sensor integration
- HTTPS posting with JWT authentication
- Memory‑optimized design using PSRAM
- PlatformIO configuration for 8MB Flash + 8MB PSRAM
The Node.js API and dashboard scripts are available in separate GitHub repositories.

Challenges Encountered
- Configuring platformio.ini to fully utilize 8MB Flash + 8MB PSRAM
- Managing memory constraints during SSL/TLS handshakes (all communication is HTTPS)
- Handling serialization/deserialization with ArduinoJson under tight memory conditions
- Avoiding hangs/crashes when polling BLE GATT via NimBLE-Arduino
- Leveraging ESP32 dual‑core architecture to split WiFi POST and BLE polling tasks (FreeRTOS)
- Optimizing WiFi connectivity using an external low‑gain antenna for distant access points

Upcoming Upgrades
- Integration of a water temperature sensor
- Addition of a solar panel for autonomous power
- OTA over API
