#include <WiFi.h>
#include <WebSocketsClient.h>
#include <MQTTPubSubClient.h>

const char* ssid = "Dure";
const char* password = "dure786*";
const char* mqtt_host = "192.168.100.11";
const uint16_t mqtt_port = 5001;
const char* mqtt_path = "/mqtt";

WebSocketsClient wsClient;
MQTTPubSubClient mqtt;  // constructor overload exists
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

  mqtt.subscribe("/hello", [](const String& payload, const size_t size) {
    Serial.print("/hello ");
    Serial.println(payload);
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