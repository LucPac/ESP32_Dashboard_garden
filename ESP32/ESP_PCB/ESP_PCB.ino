#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// WiFi Credentials
#define WIFI_SSID "..................."
#define WIFI_PASSWORD "..................."

// Firebase
#define FIREBASE_HOST "...................."
#define FIREBASE_AUTH "..................."

// DHT11
#define DHTTYPE DHT11
#define DHTPIN_1 4
#define DHTPIN_2 0
#define DHTPIN_3 2
#define DHTPIN_4 15
DHT dht1(DHTPIN_1, DHTTYPE);
DHT dht2(DHTPIN_2, DHTTYPE);
DHT dht3(DHTPIN_3, DHTTYPE);
DHT dht4(DHTPIN_4, DHTTYPE);

// Cảm biến ánh sáng
#define LIGHT_SENSOR_1 36
#define LIGHT_SENSOR_2 39
#define LIGHT_SENSOR_3 34
#define LIGHT_SENSOR_4 35

#define LIGHT_THRESHOLD 500

// Thiết bị
#define WATERING_1 13
#define FAN_1 12
#define SHADE_1 14
#define WATERING_2 27
#define FAN_2 26
#define SHADE_2 25
#define WATERING_3 16
#define FAN_3 17
#define SHADE_3 5
#define WATERING_4 18
#define FAN_4 19
#define SHADE_4 23

int nhietDo[4], doAmKK[4], doSang[4];
int lightSensors[] = {LIGHT_SENSOR_1, LIGHT_SENSOR_2, LIGHT_SENSOR_3, LIGHT_SENSOR_4};

// Firebase Objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

unsigned long previousMillis = 0;
const long interval = 2000; // Gửi dữ liệu mỗi 2 giây

// Danh sách vườn
const char* vuons[] = {"Location1", "Location2", "Location3", "Location4"};
const int numVuons = 4;

void initializeFirebaseStates();
void docTatCaDHT11();
void docTatCaAnhSang();
void nhanLenhStringTuFirebase(const String& path, int pin);

void setup() {
    Serial.begin(115200);
    Serial.println(F("Khoi dong ESP32 voi 4 cam bien DHT11 + anh sang cho 4 vuon!"));

    int Pins[] = {WATERING_1, FAN_1, SHADE_1, WATERING_2, FAN_2, SHADE_2,
                  WATERING_3, FAN_3, SHADE_3, WATERING_4, FAN_4, SHADE_4};
    for (int i = 0; i < 12; i++) {
        pinMode(Pins[i], OUTPUT);
        digitalWrite(Pins[i], LOW);
    }

    for (int i = 0; i < 4; i++) {
        pinMode(lightSensors[i], INPUT);
    }

    dht1.begin(); dht2.begin(); dht3.begin(); dht4.begin();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Dang ket noi WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    if (Firebase.ready()) {
        Serial.println("Firebase ket noi thanh cong!");
        for (int i = 0; i < numVuons; i++) {
            Firebase.setInt(firebaseData, String("/") + vuons[i] + "/status/connection", 1);
        }
    } else {
        Serial.println("Khong the ket noi Firebase!");
    }

    initializeFirebaseStates();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi mat ket noi, dang thu lai...");
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            int retry = 0;
            while (WiFi.status() != WL_CONNECTED && retry < 20) {
                delay(500);
                Serial.print(".");
                retry++;
            }
        }

        if (Firebase.ready()) {
            docTatCaDHT11();
            docTatCaAnhSang();

            for (int i = 0; i < numVuons; i++) {
                String base = "/" + String(vuons[i]);
                nhanLenhStringTuFirebase(base + "/Device1", i == 0 ? WATERING_1 : i == 1 ? WATERING_2 : i == 2 ? WATERING_3 : WATERING_4);
                nhanLenhStringTuFirebase(base + "/Device2", i == 0 ? FAN_1 : i == 1 ? FAN_2 : i == 2 ? FAN_3 : FAN_4);
                nhanLenhStringTuFirebase(base + "/Device3", i == 0 ? SHADE_1 : i == 1 ? SHADE_2 : i == 2 ? SHADE_3 : SHADE_4);
            }
        }
    }
}

void initializeFirebaseStates() {
    for (int i = 0; i < numVuons; i++) {
        String base = "/" + String(vuons[i]);
        Firebase.setInt(firebaseData, base + "/Element1", 0);
        Firebase.setInt(firebaseData, base + "/Element2", 0);
        Firebase.setInt(firebaseData, base + "/Element3", 0);
        Firebase.setString(firebaseData, base + "/Device1", "0");
        Firebase.setString(firebaseData, base + "/Device2", "0");
        Firebase.setString(firebaseData, base + "/Device3", "0");
        Firebase.setInt(firebaseData, base + "/status/connection", 0);

        json.clear();
        json.set("Device1", 0);
        json.set("Device2", 0);
        json.set("Device3", 0);
        Firebase.setJSON(firebaseData, base + "/totalTime", json);
    }
}

void docTatCaDHT11() {
    float t[4] = {
        dht1.readTemperature(),
        dht2.readTemperature(),
        dht3.readTemperature(),
        dht4.readTemperature()
    };
    float h[4] = {
        dht1.readHumidity(),
        dht2.readHumidity(),
        dht3.readHumidity(),
        dht4.readHumidity()
    };

    for (int i = 0; i < numVuons; i++) {
        nhietDo[i] = (int)t[i];
        doAmKK[i] = (int)h[i];
        String path = "/" + String(vuons[i]);
        if (!isnan(t[i]) && !isnan(h[i])) {
            Firebase.setInt(firebaseData, path + "/Element1", nhietDo[i]);
            Firebase.setInt(firebaseData, path + "/Element2", doAmKK[i]);
        } else {
            Firebase.setString(firebaseData, path + "/sensor_status", "DHT11 Error");
        }
    }
}

void docTatCaAnhSang() {
    for (int i = 0; i < numVuons; i++) {
        int raw = analogRead(lightSensors[i]);
        doSang[i] = map(raw, 0, 4095, 0, 1000);
        String path = "/" + String(vuons[i]);
        Firebase.setInt(firebaseData, path + "/Element3", doSang[i]);

        if (doSang[i] < LIGHT_THRESHOLD) {
            json.clear();
            json.set("message", String("Cuong do anh sang thap: ") + String(doSang[i]));
            Firebase.setJSON(firebaseData, path + "/logs/" + String(millis()), json);
        }
    }
}

void nhanLenhStringTuFirebase(const String& path, int pin) {
    if (Firebase.getString(firebaseData, path)) {
        String val = firebaseData.stringData();
        int state = (val == "1") ? 1 : 0;
        digitalWrite(pin, state ? HIGH : LOW);
        Serial.printf("%s: %s\n", path.c_str(), state ? "Bat" : "Tat");
    } else {
        Serial.printf("Loi doc %s: %s\n", path.c_str(), firebaseData.errorReason().c_str());
    }
}
