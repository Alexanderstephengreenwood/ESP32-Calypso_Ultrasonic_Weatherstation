#ifndef SECRETS_H
#define SECRETS_H

// --- Wi-Fi Credentials ---
#define SECRET_SSID "YOUR WIFI AP NAME"
#define SECRET_PASS "YOUR WIFI AP PWD"

// --- WAPI Server Configuration ---
#define WAPI_MAIN_URL "URL TO YOUR API ROOT"
#define WEATHER_ENDPOINT "/weatherdata" //in my case all GET and POST wind data goes to this end point
#define BAT_TPH_ENDPOINT "/battery-PTH" //in my case battery level, pressure, Temp. and humidity point there
#define AUTH_URL "https://YOUR-AUTHO-DOMAIN.eu.auth0.com/oauth/token" // Auth0 domain for token

// --- Auth0 Machine-to-Machine Credentials ---
#define AUTHO_DOMAIN "YOUR-AUTHO-DOMAIN.eu.auth0.com"
#define AUTHO_CLIENT_ID "APPLICATION ID"
#define AUTHO_CLIENT_SECRET "APPLICATION SECRET"
#define AUTHO_AUDIENCE "YOUR API END POINT"
#define AUTHO_GRANT_TYPE "client_credentials"

// --- Calypso Ultrasonic BLE GATT Configuration ---
#define CALYPSO_MAC_ADDRESS "xx:yy:zz:xx:yy:zz" 
#define ESS_SERVICE_UUID "181A"
#define WIND_SPEED_CHAR_UUID "2A72" // Apparent Wind Speed (AWS) - float (knots)
#define WIND_DIRECTION_CHAR_UUID "2A73" // Apparent Wind Direction (AWA) - float (degrees)

// --- Root CA Certificate (for TLS enforcement) ---
// ISRG Root X1 - Using C++ raw string literal for cleaner multi-line definition.
static const char* rootCACert = R"EOF(
-----BEGIN CERTIFICATE-----
INSERT CERTIFICATE DETAILS HERE
-----END CERTIFICATE-----
)EOF";

#endif