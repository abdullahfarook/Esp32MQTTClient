#include <WiFi.h>
#include <WebSocketsClient.h>
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>
#include "gsm.h"

const char* ssid = "Dure";
const char* password = "dure786*";
const char* mqtt_host = "192.168.100.11";
const uint16_t mqtt_port = 5001;
const char* mqtt_path = "/mqtt";

WebSocketsClient wsClient;
MQTTPubSubClient mqtt;  // constructor overload exists

struct SpiRamAllocator : ArduinoJson::Allocator {
  void* allocate(size_t size) override {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) override {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) override {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // Setup WebSocket connection first
  wsClient.begin(mqtt_host, mqtt_port, mqtt_path, "mqtt");
  wsClient.setReconnectInterval(2000);

  // Attach MQTT over WebSocket
  mqtt.begin(wsClient);

  // Try connecting to broker
  while (!mqtt.connect("ESP32Client")) {
    Serial.println("Attempting MQTT connection...");
    delay(1000);
  }
  Serial.println("Connected to MQTT broker");

  mqtt.subscribe("sms/outbound", [](const String& payload, const size_t size) {
    Serial.print("sms/outbound ");
    Serial.println(payload);

    SpiRamAllocator allocator;
    JsonDocument doc(&allocator);

    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
      Serial.print("JSON parse error: ");
      Serial.println(err.c_str());
      return;
    }

    const char* phone = doc["phoneNumber"] | "";
    const char* text = doc["text"] | "";

    if (strlen(phone) == 0 || strlen(text) == 0) {
      Serial.println("Invalid SMS payload");
      return;
    }

    bool ok = sendSms(String(phone), String(text));
    Serial.println(ok ? "SMS sent" : "SMS failed");
  });
}

void loop() {
  wsClient.loop();   // must run WebSocket
  mqtt.update();     // must run MQTT

  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    mqtt.publish("esp32/test", "Hello via WebSocket!");
    Serial.println("Published message");
  }
}