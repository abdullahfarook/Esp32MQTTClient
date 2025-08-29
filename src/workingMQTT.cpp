// #include <WiFi.h>
// #include <WebSocketsClient.h>
// #include <MQTTPubSubClient.h>
// #include <ArduinoJson.h>
// #include "gsm.h"

// // const char* ssid = "Dure";
// // const char* password = "dure786*";
// // const char* mqtt_host = "192.168.100.11";
// // const uint16_t mqtt_port = 5001;
// // const char* mqtt_path = "/mqtt";


// const char* mqtt_host = "test.mosquitto.org";
// const uint16_t mqtt_port = 8080;
// const char* mqtt_path = "/mqtt";

// WebSocketsClient wsClient;
// MQTTPubSub::PubSubClient<25006> mqtt;  // constructor overload exists


// void connectToMQTT() {
//   Serial.println("Connecting to MQTT broker...");
//   while (!mqtt.connect("ESP32Clientasdfasdfasdfasdfasdfw3r2werafdsfasefd")) {
//     Serial.print(".");
//     delay(1000);
//   }
//   Serial.println(" MQTT broker connected!");
// }


// void setup() {
//   Serial.begin(9600);
//   // WiFi.mode(WIFI_STA);
//   // WiFi.begin(ssid, password);
//   WiFi.begin("Wokwi-GUEST", "");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected!");

//   // Setup WebSocket connection first
//   wsClient.begin(mqtt_host, mqtt_port, mqtt_path, "mqtt");
//   wsClient.setReconnectInterval(2000);

//   // Attach MQTT over WebSocket
//   mqtt.begin(wsClient);
//   mqtt.setOptions(10, true, 1000);
//   mqtt.setWill("esp32/test", "offline", 1, true);

//   mqtt.setKeepAliveTimeout(60); // Set keep-alive interval to 60 seconds
// mqtt.setTimeout(10000); // Set timeout to 10 seconds

//   // Try connecting to broker
//   // while (!mqtt.connect("gsm-esp32")) {
//   //   Serial.println("Attempting MQTT connection...");
//   //   delay(1000);
//   // }
//   // Serial.println("Connected to MQTT broker");
//   connectToMQTT();
  
//   mqtt.subscribe("test/topicesp32", [](const String& payload, const size_t size) {
//     Serial.println("Received message: " + payload);
//   });
//   // mqtt.subscribe(callback);
//   // mqtt.subscribe("test/hello", [](const String& payload, const size_t size) {
//   //   Serial.print("test/hello");
//   //   Serial.println(payload);

//   //   ArduinoJson::DynamicJsonDocument doc(256);  // ArduinoJson v7 requires capacity in constructor
//   //   ArduinoJson::DeserializationError err = deserializeJson(doc, payload);
//   //   if (err) {
//   //     Serial.print("JSON parse error: ");
//   //     Serial.println(err.c_str());
//   //     return;
//   //   }

//   //   const char* phone = doc["phoneNumber"] | "";
//   //   const char* text = doc["text"] | "";

//   //   if (strlen(phone) == 0 || strlen(text) == 0) {
//   //     Serial.println("Invalid SMS payload");
//   //     return;
//   //   }

//   //   bool ok = sendSms(String(phone), String(text));
//   //   Serial.println(ok ? "SMS sent" : "SMS failed");
//   // });

//   // Debug: subscribe to all topics to verify delivery
//   // mqtt.subscribe("#", [](const String& payload, const size_t size) {
//   //   Serial.print("# ");
//   //   Serial.println(payload);
//   // });
// }
// void callback(char *topic, byte *payload, unsigned int length) {
//   Serial.print("Message arrived in topic: ");
//   Serial.println(topic);
//   Serial.print("Message:");
//   for (int i = 0; i < length; i++) {
//       Serial.print((char) payload[i]);
//   }
//   Serial.println();
//   Serial.println("-----------------------");
// }

// void loop() {
//   Serial.println("loop");
//   //wsClient.loop();   // must run WebSocket
//   //mqtt.update();     // must run MQTT
//   mqtt.update();
//   Serial.println("mqtt.update");
//   wsClient.loop();
//   Serial.println("wsClient.loop");

//   // static unsigned long lastMsg = 0;
//   // if (millis() - lastMsg > 5000) {
//   //   lastMsg = millis();
//   //   mqtt.publish("esp32/test", "Hello via WebSocket!");
//   //   Serial.println("Published message");
//   // }

//   if (!mqtt.isConnected()) {
//     Serial.println("MQTT not connected, reconnecting...");
//     connectToMQTT();
//   }

//   if(!wsClient.isConnected()){
//     Serial.println("WS not connected, reconnecting...");
//   }

//   static uint32_t lastPublishTime = millis();
//   if (millis() - lastPublishTime > 1000) {
//     lastPublishTime = millis();
//     Serial.println("Publishing message...");
//     mqtt.publish("test/topicesp32", "Hello from ESP32");
//     Serial.println("Published message");
//   }
//   delay(1000);
// }