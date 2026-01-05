# System Architecture

## Overview
The system consists of an ESP32-WROVER-IE with 8MB Flash and 8MB PSRAM, a
Calypso Ultrasonic anemometer connected via BLE, and a BME680 sensor connected
via I2C. Data is aggregated locally and periodically uploaded to a secure API.

## Components

### 1. ESP32 Firmware
- PlatformIO + Arduino framework
- Dual-core architecture:
  - Core 0: BLE polling (NimBLE-Arduino)
  - Core 1: WiFi HTTPS POST tasks
- PSRAM-backed buffers for BLE sampling and JSON serialization
- JWT authentication via Auth0 token endpoint
- HTTPS communication using mbedTLS

### 2. BLE Subsystem
- Polls Calypso Ultrasonic every second
- Retrieves AWS, AWA, and GUST characteristics
- Aggregates values over 4:50 minutes
- Tracks maximum gust for the period
- Implements retry and timeout logic to avoid hangs

### 3. Environmental Sensor Subsystem
- BME680 via I2C
- Provides temperature, humidity, pressure
- Sampled hourly to reduce power consumption

### 4. Networking & API
- HTTPS POST to:
  - `/weatherdata/` (wind)
  - `/battery-PTH/` (battery + PTH)
- JWT token retrieval and automatic refresh
- SSL certificate validation enabled

### 5. Dashboard
- HTML + Google Charts
- Fetches data from API endpoints
- Displays wind, gusts, temperature, humidity, pressure

## Data Flow Diagram (Text Version)

ESP32  
→ BLE Polling (Calypso)  
→ Aggregation Engine  
→ JSON Serialization  
→ HTTPS POST (JWT-secured API)  
→ API Server  
→ Dashboard + Windy.com Forwarding

## Partition Table
- 8MB Flash custom layout
- Large OTA partitions for future firmware growth
- Dedicated SPIFFS for logs/config

## Future Extensions
- Water temperature sensor
- Solar panel + power management