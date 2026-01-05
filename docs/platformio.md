# PlatformIO Configuration

This project targets the **ESP32‑WROVER‑IE** module with:
- 8MB Flash (QuadSPI)
- 8MB PSRAM (QuadSPI)
- External WiFi/BLE antenna

The firmware is built using **PlatformIO** with the **Arduino framework** and optimized for:
- PSRAM usage
- NimBLE BLE stack
- Large OTA partitions
- Stable HTTPS + JSON handling
- Dual‑core task separation (BLE vs WiFi)

Below is the complete configuration used in this project.

---

## Full platformio.ini

```ini
[env:esp-wrover-kit]
platform = espressif32
board = esp-wrover-kit
framework = arduino
monitor_speed = 115200

board_build.psram = enabled
board_build.arduino.event_loop_stack = 40960
board_build.arduino.event_loop_priority = 1

board_upload.flash_size = 8MB
board_upload.maximum_size = 3670016
board_build.partitions = default_8MB_OTA.csv

build_flags = 
    -DBOARD_HAS_PSRAM
    -mfix-esp32-psram-cache-issue
    -I src
    -I include
    -DCONFIG_BT_BLE_ENABLED=1
    -DCONFIG_BT_NIMBLE_LOG_LEVEL=3
    -DCONFIG_NIMBLE_POCI_TASK_CORE=0 
    -DCONFIG_NIMBLE_POCI_TASK_PRIORITY=3

lib_deps = 
    h2zero/NimBLE-Arduino@^1.4.3
    bblanchon/ArduinoJson@^6.21.5
    adafruit/Adafruit BME680 Library @ ^2.0.5
    adafruit/Adafruit Unified Sensor @ ^1.1.13
    HTTPClient
    UrlEncode