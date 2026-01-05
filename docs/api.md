# API Specification

This document describes the JSON structure returned by the Weather Station API.
Two datasets are exposed:

- Wind data (`/weatherdata/`)
- Battery + Pressure/Temperature/Humidity (`/battery-PTH/`)

All timestamps are in ISO‑8601 UTC format.

---

# 1. Wind Data Endpoint

**GET** https://wapi.greenwoods.ch/weatherdata/

Returns an array of wind measurement objects.

## Example Response

[
  {
    "id": 4609,
    "AWA": 60,
    "AWS": "1.6",
    "GUST": "4.5",
    "created_at": "2026-01-05T10:50:53.000Z"
  },
  {
    "id": 4608,
    "AWA": 57,
    "AWS": "1.7",
    "GUST": "4.7",
    "created_at": "2026-01-05T10:45:53.000Z"
  },
  {
    "id": 4607,
    "AWA": 75,
    "AWS": "2.3",
    "GUST": "4.9",
    "created_at": "2026-01-05T10:40:51.000Z"
  },
  {
    "id": 4606,
    "AWA": 99,
    "AWS": "1.6",
    "GUST": "3.3",
    "created_at": "2026-01-05T10:35:53.000Z"
  }
]

## Field Description

| Field        | Type    | Description                                      |
|--------------|---------|--------------------------------------------------|
| `id`         | integer | Database record ID                               |
| `AWA`        | integer | Apparent Wind Angle (degrees)                    |
| `AWS`        | string  | Apparent Wind Speed (knots)                      |
| `GUST`       | string  | Maximum gust during the 4:50 sampling period     |
| `created_at` | string  | Timestamp when the record was stored (UTC)       |

---

# 2. Battery + PTH Endpoint

**GET** https://wapi.greenwoods.ch/battery-PTH/

Returns an array of environmental measurement objects.

## Example Response

[
  {
    "id": 326,
    "battery": 100,
    "temperature": "12.9",
    "humidity": "32.2",
    "pressure": 963,
    "created_at": "2026-01-05T10:20:54.000Z"
  },
  {
    "id": 325,
    "battery": 100,
    "temperature": "13.5",
    "humidity": "29.0",
    "pressure": 963,
    "created_at": "2026-01-05T09:21:24.000Z"
  },
  {
    "id": 324,
    "battery": 100,
    "temperature": "19.5",
    "humidity": "35.7",
    "pressure": 963,
    "created_at": "2026-01-05T09:19:40.000Z"
  }
]

## Field Description

| Field         | Type    | Description                                      |
|---------------|---------|--------------------------------------------------|
| `id`          | integer | Database record ID                               |
| `battery`     | integer | Battery level of the Calypso Ultrasonic (%)     |
| `temperature` | string  | Temperature in °C                                |
| `humidity`    | string  | Relative humidity (%)                            |
| `pressure`    | integer | Atmospheric pressure (hPa)                       |
| `created_at`  | string  | Timestamp when the record was stored (UTC)       |

---

# Authentication

All POST operations from the ESP32 require a valid JWT token retrieved from Auth0.

Clients reading data (GET requests) do **not** require authentication.

---

# Notes

- Numeric values may be returned as strings depending on the database driver.
- Timestamps follow MySQL/MariaDB UTC format.
- Wind data is posted every **5 minutes**.
- Battery + PTH data is posted every **hour**.