# Contributing Guidelines

Thank you for your interest in contributing to this project!

This repository focuses on the ESP32 firmware for a Bluetooth-enabled weather
station using the Calypso Ultrasonic anemometer and BME680 sensor. Contributions
that improve stability, performance, documentation, or hardware compatibility
are welcome.

## How to Contribute

### 1. Fork the Repository
Create your own fork and work from a feature branch:
- `main` is reserved for stable releases.
- Use descriptive branch names (e.g., `feature/psram-allocator`, `fix/ble-timeout`).

### 2. Coding Standards
- Use clear, maintainable C++.
- Prefer non-blocking patterns (FreeRTOS tasks, queues, timers).
- Avoid heap fragmentation; prefer PSRAM for large buffers.
- Follow PlatformIO structure conventions.

### 3. Submitting Pull Requests
- Ensure your code compiles without warnings.
- Include a clear description of the change.
- Reference related issues when applicable.
- Add comments for complex logic (BLE, SSL, memory handling).

### 4. Reporting Issues
When opening an issue, please include:
- ESP32 model and revision
- PlatformIO version
- Partition table used
- Steps to reproduce
- Logs (BLE, WiFi, SSL, memory)

## Code of Conduct
Be respectful, constructive, and collaborative. This project values clarity,
maintainability, and technical precision.