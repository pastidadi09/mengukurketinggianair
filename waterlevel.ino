#include <WiFi.h>
#include <NewPing.h>
#include <HTTPClient.h>

const char* ssid = "Pasti Dadi";
const char* password = "bejo0808";
const char* serverUrl = "http://192.168.145.154/mengukurair/savedata.php"; // Ganti dengan URL server Anda



#define TRIGGER_PIN 12
#define ECHO_PIN 13
#define MAX_DISTANCE 100
#define SENSOR_HEIGHT 3 // Jarak sensor dari permukaan air (dalam cm)
#define MAX_WATER_HEIGHT 10 // Tinggi maksimal air dari dasar hingga permukaan (dalam cm)

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  delay(1000);

  unsigned int distance = sonar.ping_cm();

  if (distance == 0) {
    Serial.println("No object detected");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    float waterLevelHeight = MAX_WATER_HEIGHT - (distance - SENSOR_HEIGHT);
    waterLevelHeight = constrain(waterLevelHeight, 0, MAX_WATER_HEIGHT);
    int waterLevelPercentage = (waterLevelHeight / MAX_WATER_HEIGHT) * 100;

    sendData(waterLevelPercentage);
     Serial.println(waterLevelPercentage);
  }
}

void sendData(int waterLevelPercentage) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String data = "level=" + String(waterLevelPercentage);
    // String data = "idsensor=" + String(id) + "&status=" + String(status) + "&jumlah=" + String(jumlah);
    Serial.println("Sending data: " + data);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(data);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response: " + response);
    } else {
      Serial.println("Error sending data to server");
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected. Unable to send data.");
  }
}