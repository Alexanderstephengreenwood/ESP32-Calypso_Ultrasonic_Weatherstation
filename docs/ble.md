# BLE Subsystem (Calypso Ultrasonic)

## Overview
The Calypso Ultrasonic exposes BLE GATT characteristics for:
- AWS (Apparent Wind Speed)
- AWA (Apparent Wind Angle)
- GUST (Max gust since last read)
- Battery level

## Polling Strategy
- Poll every 1 second for 4:50 minutes
- Store AWS/AWA samples in PSRAM
- Track maximum gust over the period

## Connection Flow
1. Scan for device name "ULTRASONIC"
2. Connect using NimBLE-Arduino
3. Discover services & characteristics
4. Poll characteristics using non-blocking reads
5. Handle disconnects gracefully

## Error Handling
- Automatic reconnect attempts
- BLE timeouts to avoid task lockups
- Watchdog protection on Core 0

## Known Issues
- Calypso sometimes stalls on GATT reads
- NimBLE requires careful memory tuning