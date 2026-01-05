#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <NimBLEDevice.h> 
#include <algorithm> 
#include <vector>    
#include "secrets.h" 
#include <esp_system.h>
#include <esp_heap_caps.h>
#include <time.h>    
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

// --- Configuration ---
const char* API_HOST = WAPI_MAIN_URL;
const char* API_PATH = WEATHER_ENDPOINT;
const char* BAT_TPH_PATH = BAT_TPH_ENDPOINT;
const char* AUTH_TOKEN_URL = AUTH_URL;
const char* CA_CERT = rootCACert;

const int MAX_SAMPLES = (5 * 60) + 20; 
const float MS_TO_KNOTS = 1.94384f;
const char* NTP_SERVER = "pool.ntp.org";
const char* TIMEZONE_INFO = "CET-1CEST,M3.5.0,M10.5.0/3"; 

// --- Data Structures ---
struct WindPostPayload {
    float avgAws;
    float gustAws;
    int avgAwa;
    int sampleCount;
    bool isMaintenance;
    float temp;
    float press;
    float hum;
    int batt;
};

struct WindData {
    float instantAWS = 0.0f;
    float instantAWA = 0.0f;
    float gustMax_ms = 0.0f;
    float* awsSamples = nullptr;
    float* awaSamples = nullptr;
    int sampleCount = 0;
    unsigned long lastNotification = 0;
};

// --- Globals ---
QueueHandle_t postQueue;
TaskHandle_t WiFiTaskHandle;
WindData windData;
Adafruit_BME680 bme;
bool bmeFound = false;
bool connected = false;
bool doConnect = true;
NimBLEAddress calypsoAddress;
NimBLEClient* pClient = nullptr;

String jwtToken = ""; // Global cache for your 30-day token
int lastTriggeredMinute = -1;

// PSRAM Allocator for ArduinoJson
namespace ArduinoJson { namespace V6215PB2 { class PsramAllocator { public:
    void* allocate(size_t s) { return heap_caps_malloc(s, MALLOC_CAP_SPIRAM); }
    void deallocate(void* p) { free(p); }
    void* reallocate(void* p, size_t s) { return heap_caps_realloc(p, s, MALLOC_CAP_SPIRAM); }
}; } }
using PsramJsonDocument = ArduinoJson::V6215PB2::BasicJsonDocument<ArduinoJson::V6215PB2::PsramAllocator>;

// ==============================================================================
// CORE 0: WIFI & AUTH (Token Optimized)
// ==============================================================================

bool fetchTokenOnCore0() {
 //   Serial.println("[CORE 0] Auth0: Requesting new JWT token...");
    WiFiClientSecure client;
    client.setCACert(CA_CERT);
    HTTPClient http;
    
    if (!http.begin(client, AUTH_TOKEN_URL)) return false;
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<512> requestDoc; 
    requestDoc["client_id"] = AUTHO_CLIENT_ID;
    requestDoc["client_secret"] = AUTHO_CLIENT_SECRET;
    requestDoc["audience"] = AUTHO_AUDIENCE;
    requestDoc["grant_type"] = AUTHO_GRANT_TYPE;

    String requestBody;
    serializeJson(requestDoc, requestBody);
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode == 200) {
        String payload = http.getString();
        PsramJsonDocument responseDoc(2048); 
        deserializeJson(responseDoc, payload);
        jwtToken = responseDoc["access_token"].as<String>();
       // Serial.println("[CORE 0] Auth0: JWT successfully updated.");
        http.end();
        return true;
    }
    //Serial.printf("[CORE 0] Auth0 FAIL: %d\n", httpResponseCode);
    http.end();
    return false;
}

void wifiPostingTask(void * pvParameters) {
    WindPostPayload data;
    for(;;) {
        if (xQueueReceive(postQueue, &data, portMAX_DELAY)) {
           // Serial.println("[CORE 0] Wakeup: Connecting WiFi...");
            
            WiFi.begin(SECRET_SSID, SECRET_PASS);
            unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }

            if (WiFi.status() == WL_CONNECTED) {
                // Ensure we have an initial token
                if (jwtToken == "") fetchTokenOnCore0();

                if (jwtToken != "") {
                    bool success = false;
                    int attempts = 0;

                    while (!success && attempts < 2) {
                        WiFiClientSecure client;
                        client.setCACert(CA_CERT);
                        client.setTimeout(15);
                        HTTPClient http;
                        
                        String url = String("https://") + API_HOST + (data.isMaintenance ? BAT_TPH_PATH : API_PATH);
                        http.begin(client, url);
                        http.addHeader("Content-Type", "application/json");
                        http.addHeader("Authorization", "Bearer " + jwtToken);

                        PsramJsonDocument doc(1024);
                        if (data.isMaintenance) {
                            doc["battery"] = data.batt;
                            doc["temperature"] = data.temp;
                            doc["pressure"] = data.press;
                            doc["humidity"] = data.hum;
                        } else {
                            doc["AWA"] = data.avgAwa;
                            doc["AWS"] = data.avgAws;
                            doc["GUST"] = data.gustAws;
                        }

                        String body;
                        serializeJson(doc, body);
                        int code = http.POST(body);

                        if (code == 200) {
                           // Serial.printf("[CORE 0] API Success: %d\n", code);
                            success = true;
                        } else if (code == 401) {
                           // Serial.println("[CORE 0] 401 Unauthorized. Fetching new token...");
                            fetchTokenOnCore0();
                            attempts++; // Retry with new token
                        } else {
                          //  Serial.printf("[CORE 0] API Failed: %d\n", code);
                            attempts = 2; // Don't retry other errors
                        }
                        http.end();
                    }
                }
            }
            // SOFT DISCONNECT: Drops the link but keeps the driver initialized to avoid crash
            WiFi.disconnect();
           // Serial.println("[CORE 0] WiFi Idle.");
        }
    }
}

// ==============================================================================
// CORE 1: BLE & SCHEDULER (Real-time)
// ==============================================================================

float calculateMedian(float* arr, int size) {
    if (size == 0) return 0.0f;
    std::vector<float> sorted(arr, arr + size);
    std::sort(sorted.begin(), sorted.end());
    if (size % 2 == 0) return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0f;
    return sorted[size / 2];
}

static void notifyCallback(NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t len, bool isNotify) {
    if (len < 2) return;
    uint16_t raw = (uint16_t)pData[0] | ((uint16_t)pData[1] << 8);
    if (raw == 0xFFFF) return;
    float val = (float)raw / 100.0f;
    windData.lastNotification = millis();

    if (pChar->getUUID().equals(NimBLEUUID("2A72"))) {
        windData.instantAWS = val;
        if (val > windData.gustMax_ms) windData.gustMax_ms = val;
    } else if (pChar->getUUID().equals(NimBLEUUID("2A73"))) {
        windData.instantAWA = val;
    }
}

class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* p) { connected = true; /*Serial.println("BLE: Connected.");*/ }
    void onDisconnect(NimBLEClient* p) { connected = false; doConnect = true; /*Serial.println("BLE: Disconnected.");*/ }
};

class AdvCallbacks: public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* dev) {
        if(dev->getName() == "ULTRASONIC") {
            dev->getScan()->stop();
            calypsoAddress = dev->getAddress();
            doConnect = true;
        }
    }
};

void setup() {
   // Serial.begin(115200);
    delay(2000);
    
    if (!psramInit()) {
       // Serial.println("PSRAM ERROR");
        while(1);
    }

    windData.awsSamples = (float*)heap_caps_malloc(MAX_SAMPLES * sizeof(float), MALLOC_CAP_SPIRAM);
    windData.awaSamples = (float*)heap_caps_malloc(MAX_SAMPLES * sizeof(float), MALLOC_CAP_SPIRAM);
    
    NimBLEDevice::init("");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new AdvCallbacks());
    pScan->start(0, false);

    if (bme.begin(0x76)) bmeFound = true;

    // WiFi Task on Core 0
    postQueue = xQueueCreate(5, sizeof(WindPostPayload));
    xTaskCreatePinnedToCore(wifiPostingTask, "WiFiTask", 16384, NULL, 1, &WiFiTaskHandle, 0);

    // Initial NTP Sync for scheduler
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    configTime(0, 0, NTP_SERVER);
    setenv("TZ", TIMEZONE_INFO, 1);
    tzset();
    WiFi.disconnect();

   // Serial.println("System: Ready.");
}

void loop() {
    unsigned long now = millis();

    if (doConnect && !connected) {
        if (pClient == nullptr) {
            pClient = NimBLEDevice::createClient();
            pClient->setClientCallbacks(new ClientCallbacks());
        }
        if (pClient->connect(calypsoAddress)) {
            NimBLERemoteService* pSvc = pClient->getService("181A");
            if (pSvc) {
                auto pAws = pSvc->getCharacteristic("2A72");
                if (pAws) pAws->subscribe(true, notifyCallback);
                auto pAwa = pSvc->getCharacteristic("2A73");
                if (pAwa) pAwa->subscribe(true, notifyCallback);
            }
        }
        doConnect = false;
    }

    // 1Hz Polling
    static unsigned long last1Hz = 0;
    if (now - last1Hz >= 1000) {
        if (connected && windData.sampleCount < MAX_SAMPLES) {
            windData.awsSamples[windData.sampleCount] = windData.instantAWS;
            windData.awaSamples[windData.sampleCount] = windData.instantAWA;
            windData.sampleCount++;
        }
        last1Hz = now;
    }

    // Precise Scheduler (Every 5 Minutes)
    time_t tnow = time(nullptr);
    struct tm* ti = localtime(&tnow);
    
    if (ti->tm_min % 5 == 0 && ti->tm_min != lastTriggeredMinute) {
        if (windData.sampleCount > 10) {
            WindPostPayload p;
            float medAws = calculateMedian(windData.awsSamples, windData.sampleCount);
            float medAwa = calculateMedian(windData.awaSamples, windData.sampleCount);
            
            p.avgAws = roundf((medAws * MS_TO_KNOTS) * 10.0f) / 10.0f;
            p.gustAws = roundf((windData.gustMax_ms * MS_TO_KNOTS) * 10.0f) / 10.0f;
            p.avgAwa = (int)round(medAwa);
            p.sampleCount = windData.sampleCount;

            // Hourly Maintenance
            if (ti->tm_min == 0) {
                p.isMaintenance = true;
                if (bmeFound && bme.performReading()) {
                    p.temp = bme.temperature;
                    p.press = bme.pressure / 100.0F;
                    p.hum = bme.humidity;
                }
                p.batt = 100; 
            } else {
                p.isMaintenance = false;
            }

            if (xQueueSend(postQueue, &p, 0) == pdPASS) {
                lastTriggeredMinute = ti->tm_min;
                windData.sampleCount = 0;
                windData.gustMax_ms = 0.0f;
               // Serial.printf("Queued: %02d:%02d\n", ti->tm_hour, ti->tm_min);
            }
        }
    }

    vTaskDelay(20 / portTICK_PERIOD_MS); 
}