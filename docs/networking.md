# Networking & HTTPS

## WiFi
- External antenna recommended for long-range APs
- Auto-reconnect enabled
- Blocking WiFi operations moved to Core 1

## HTTPS
- All communication uses TLS 1.2
- Certificate validation enabled
- mbedTLS memory usage optimized for PSRAM

## JWT Authentication
- Token retrieved from Auth0 endpoint
- Token cached in RAM
- Automatic refresh before expiration

## POST Frequency
- Wind data: every 5 minutes
- Battery + PTH: every hour