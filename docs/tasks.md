# FreeRTOS Task Structure

## Core 0 (BLE)
Task: blePollTask
- Polls BLE every second
- Stores samples in PSRAM
- Handles reconnects

## Core 1 (Networking)
Task: wifiPostTask
- Manages WiFi connection
- Retrieves JWT token
- Posts wind + PTH data

## Synchronization
- Queues for inter-task communication
- Mutex for shared JSON buffer