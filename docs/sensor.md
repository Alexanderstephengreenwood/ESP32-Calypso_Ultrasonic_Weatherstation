# Environmental Sensors (BME680)

## Overview
The BME680 provides:
- Temperature
- Humidity
- Pressure
- Gas resistance (optional)

## Sampling Strategy
- Sample once per hour
- Use I2C at 100 kHz or 400 kHz
- Use oversampling for stable readings

## Library
- Adafruit BME680 or Seeed Grove driver

## Notes
- Gas resistance is not used in this project
- Temperature compensation may be required