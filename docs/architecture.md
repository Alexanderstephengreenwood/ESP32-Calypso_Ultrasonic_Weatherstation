# System Architecture

## Overview
This project implements a Bluetooth-enabled weather station using:
- Calypso Ultrasonic anemometer (BLE)
- BME680 environmental sensor (I2C)
- ESP32-WROVER-IE (8MB Flash, 8MB PSRAM)

The ESP32 collects wind and environmental data, aggregates values, and posts
results to a secure API using HTTPS + JWT authentication.

## High-Level Diagram (Text)
Calypso Ultrasonic (BLE)
        ↓
BLE Polling Task (Core 0)
        ↓
Wind Aggregation Engine
        ↓
JSON Serialization (PSRAM)
        ↓
HTTPS POST (Core 1)
        ↓
API Server → Dashboard → Windy.com

## Core Responsibilities
### Core 0 (APP CPU)
- BLE scanning and GATT polling
- Wind sampling every second
- Gust tracking
- BLE reconnect logic

### Core 1 (PRO CPU)
- WiFi connection management
- JWT token retrieval & refresh
- HTTPS POST tasks
- BME680 sampling (hourly)

## Data Retention
- Wind samples stored in PSRAM buffers
- Aggregated values stored in RAM
- No persistent storage required

## Partition Table
- Large OTA partitions for future firmware growth
- Dedicated SPIFFS for logs/config (optional)