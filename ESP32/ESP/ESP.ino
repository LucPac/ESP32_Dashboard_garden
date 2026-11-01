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
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LIGHT_SENSOR_PIN 36
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
#define SHADE_3 15
#define WATERING_4 18
#define FAN_4 19
#define SHADE_4 23

int nhietDo = 0, doAmKK = 0;
int doSang = 0;

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
void docDuLieuDHT11();
void docDuLieuAnhSang();
void nhanLenhTuFirebase(const String& path, int pin);
void nhanLenhStringTuFirebase(const String& path, int pin); // Hàm mới để đọc giá trị string

void setup() {
    Serial.begin(11200); // Tăng tốc độ baud cho dễ xem log
    Serial.println(F("Khởi động DHT11, cảm biến ánh sáng và Firebase cho 4 vườn!"));

    // Thiết lập chân
    int Pins[] = {WATERING_1, FAN_1, SHADE_1, WATERING_2, FAN_2, SHADE_2,
                    WATERING_3, FAN_3, SHADE_3, WATERING_4, FAN_4, SHADE_4};
    for (int i = 0; i < 12; i++) {
        pinMode(Pins[i], OUTPUT);
        digitalWrite(Pins[i], LOW);
    }
    pinMode(LIGHT_SENSOR_PIN, INPUT); // Sử dụng chân cảm biến ánh sáng

    // Khởi động cảm biến
    dht.begin();

    // Kết nối WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

    // Cấu hình Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;

    // Kích hoạt tính năng kết nối lại WiFi tự động của FirebaseESP32
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true); // Đảm bảo thư viện sẽ tự reconnect nếu mất kết nối

    if (Firebase.ready()) {
        Serial.println("Đã kết nối Firebase!");
        for (int i = 0; i < numVuons; i++) {
            // Đảm bảo kiểu dữ liệu là int trên Firebase nếu bạn muốn dùng setInt
            Firebase.setInt(firebaseData, String("/") + vuons[i] + "/status/connection", 1);
        }
    } else {
        Serial.println("Không thể kết nối Firebase!");
    }

    // Khởi tạo trạng thái ban đầu của các thiết bị và cảm biến trên Firebase
    initializeFirebaseStates();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Kiểm tra WiFi và cố gắng kết nối lại nếu mất kết nối
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi ngắt kết nối, thử lại...");
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            int retryCount = 0;
            const int maxRetries = 20; // Thử lại 20 lần (10 giây)
            while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
                Serial.print(".");
                delay(500);
                retryCount++;
            }
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("\nWiFi connected.");
            } else {
                Serial.println("\nKhông thể kết nối WiFi!");
            }
            // Cập nhật trạng thái kết nối trên Firebase sau khi thử kết nối lại
            for (int i = 0; i < numVuons; i++) {
                Firebase.setInt(firebaseData, String("/") + vuons[i] + "/status/connection", WiFi.status() == WL_CONNECTED ? 1 : 0);
            }
        }

        if (Firebase.ready()) {
            // Đọc và gửi dữ liệu cảm biến
            docDuLieuDHT11();
            docDuLieuAnhSang();

            // Điều khiển thiết bị từ Firebase
            for (int i = 0; i < numVuons; i++) {
                String basePath = "/" + String(vuons[i]);
                // Sử dụng hàm nhanLenhStringTuFirebase() cho các thiết bị
                // vì bạn nói chúng là dạng string trên Firebase.
                nhanLenhStringTuFirebase(basePath + "/Device1", i == 0 ? WATERING_1 : i == 1 ? WATERING_2 : i == 2 ? WATERING_3 : WATERING_4);
                nhanLenhStringTuFirebase(basePath + "/Device2", i == 0 ? FAN_1 : i == 1 ? FAN_2 : i == 2 ? FAN_3 : FAN_4);
                nhanLenhStringTuFirebase(basePath + "/Device3", i == 0 ? SHADE_1 : i == 1 ? SHADE_2 : i == 2 ? SHADE_3 : SHADE_4);
            }
        } else {
            // Cập nhật trạng thái kết nối là 0 nếu Firebase không sẵn sàng
            for (int i = 0; i < numVuons; i++) {
                Firebase.setInt(firebaseData, String("/") + vuons[i] + "/status/connection", 0);
            }
        }
    }
}

// Hàm khởi tạo trạng thái ban đầu của các biến trên Firebase
void initializeFirebaseStates() {
    for (int i = 0; i < numVuons; i++) {
        String basePath = "/" + String(vuons[i]);
        // Cảm biến
        Firebase.setInt(firebaseData, basePath + "/Element1", 0); // Nhiệt độ
        Firebase.setInt(firebaseData, basePath + "/Element2", 0); // Độ ẩm
        Firebase.setInt(firebaseData, basePath + "/Element3", 0); // Ánh sáng (đổi từ co2)

        // Thiết bị - Quan trọng: Khởi tạo là "0" (string) nếu bạn muốn giữ chúng là string
        Firebase.setString(firebaseData, basePath + "/Device1", "0");
        Firebase.setString(firebaseData, basePath + "/Device2", "0");
        Firebase.setString(firebaseData, basePath + "/Device3", "0");

        Firebase.setInt(firebaseData, basePath + "/status/connection", 0);

        json.clear();
        json.set("Device1", 0); // Giá trị int cho totalTime
        json.set("Device2", 0);
        json.set("Device3", 0);
        Firebase.setJSON(firebaseData, basePath + "/totalTime", json);

        Serial.println("Khởi tạo trạng thái Firebase cho " + String(vuons[i]) + " hoàn tất!");
    }
}

// Hàm đọc dữ liệu từ cảm biến DHT11 và gửi lên Firebase
void docDuLieuDHT11() {
    nhietDo = (int)dht.readTemperature();
    doAmKK = (int)dht.readHumidity();
    for (int i = 0; i < numVuons; i++) {
        String basePath = "/" + String(vuons[i]);
        if (!isnan(nhietDo) && !isnan(doAmKK)) { // Kiểm tra giá trị hợp lệ
            Firebase.setInt(firebaseData, basePath + "/Element1", nhietDo);
            Firebase.setInt(firebaseData, basePath + "/Element2", doAmKK);
            Serial.printf("%s - Nhiet do: %d°C, Do am: %d%%\n", vuons[i], nhietDo, doAmKK);
        } else {
            Firebase.setString(firebaseData, basePath + "/sensor_status", "DHT11 Error");
            Serial.println(String(vuons[i]) + " - Lỗi đọc DHT11!");
        }
    }
}

// Hàm đọc dữ liệu từ cảm biến ánh sáng và gửi lên Firebase
void docDuLieuAnhSang() {
    int sensorValue = analogRead(LIGHT_SENSOR_PIN);
    doSang = map(sensorValue, 0, 4095, 0, 1000);

    for (int i = 0; i < numVuons; i++) {
        String basePath = "/" + String(vuons[i]);
        Firebase.setInt(firebaseData, basePath + "/Element3", doSang); // Gửi giá trị độ sáng lên Firebase
        Serial.printf("%s - Ánh sáng: %d\n", vuons[i], doSang);

        if (doSang < LIGHT_THRESHOLD) {
            json.clear();
            json.set("message", String("Cường độ ánh sáng thấp: ") + String(doSang) + " (ngưỡng: " + String(LIGHT_THRESHOLD) + ")");
            Firebase.setJSON(firebaseData, basePath + "/logs/" + String(millis()), json);
        }
    }
}

// nhận lệnh điều khiển thiết bị từ Firebase (đọc dưới dạng String)
void nhanLenhStringTuFirebase(const String& path, int pin) {
    if (Firebase.getString(firebaseData, path)) { // Đọc giá trị dưới dạng string
        String trangThaiStr = firebaseData.stringData();
        int trangThai = 0;
        if (trangThaiStr == "1") { // Chuyển đổi "1" (string) thành 1 (int)
            trangThai = 1;
        } else if (trangThaiStr == "0") { // Chuyển đổi "0" (string) thành 0 (int)
            trangThai = 0;
        } else {
            Serial.printf("Giá trị không hợp lệ từ Firebase cho %s: %s\n", path.c_str(), trangThaiStr.c_str());
            return; // Thoát nếu giá trị không phải "0" hoặc "1"
        }

        digitalWrite(pin, trangThai ? HIGH : LOW);
        Serial.printf("%s: %s\n", path.c_str(), trangThai ? "Bat" : "Tat");
    } else {
        Serial.printf("Loi doc %s: %s\n", path.c_str(), firebaseData.errorReason().c_str());
    }
}
// nhận lệnh điều khiển thiết bị từ Firebase (đọc dưới dạng int)
void nhanLenhTuFirebase(const String& path, int pin) {
    if (Firebase.getInt(firebaseData, path)) {
        int trangThai = firebaseData.intData();
        digitalWrite(pin, trangThai ? HIGH : LOW);
        Serial.printf("%s: %s\n", path.c_str(), trangThai ? "Bat" : "Tat");
    } else {
        Serial.printf("Loi doc %s: %s\n", path.c_str(), firebaseData.errorReason().c_str());
    }
}