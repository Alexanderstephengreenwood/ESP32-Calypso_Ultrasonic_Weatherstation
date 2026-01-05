# Troubleshooting Guide

## BLE Issues
- Symptom: ESP32 hangs during GATT read
  Fix: Enable NimBLE timeouts

- Symptom: Device not found
  Fix: Increase scan duration to 5–10 seconds

## WiFi Issues
- Symptom: Frequent disconnects
  Fix: Use external antenna, reduce TX power

## HTTPS Issues
- Symptom: Handshake failure
  Fix: Ensure correct system time
  Fix: Increase mbedTLS buffer sizes

## JSON Issues
- Symptom: Serialization fails
  Fix: Increase PSRAM JSON buffer