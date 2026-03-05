#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "SJSU_guest";
const char* password = "";
const char* serverUrl = "http://sce.sjsu.edu/sound"; // Your server's IP

const int micPin = 34;
const int sampleWindow = 5000; // Sample window width in mS (50 mS = 20Hz)
const String API_KEY = "API_KEY_HERE"; // Replace with your actual API key

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 4095;

  while (millis() - startMillis < sampleWindow) {
    int sample = analogRead(micPin);
    Serial.println(sample);
    if (sample < 4095) {
      if (sample > signalMax) signalMax = sample;
      else if (sample < signalMin) signalMin = sample;
    }
  }
  
  peakToPeak = signalMax - signalMin;
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("X-API-Key", "YOUR_API_KEY");
    
    String httpRequestData = "level=" + String(peakToPeak);
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode > 0) {
      Serial.print("Data Sent. Level: ");
      Serial.println(peakToPeak);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(peakToPeak);
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  
  delay(300000); // Wait 5 minutes before next sample
}