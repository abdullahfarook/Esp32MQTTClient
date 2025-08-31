#include <WiFi.h>
#include <WebSocketsClient.h>
#include <MQTTPubSubClient.h>
#include <ArduinoJson.h>
#include "gsm.h"
#include "utils.h"

// WiFi & MQTT Configuration
const char *ssid = "Dure";
const char *password = "dure786*";
// const char *mqtt_host = "192.168.100.11";
// const uint16_t mqtt_port = 5001;

const char *mqtt_host = "api-red.metaphorltd.com";
const uint16_t mqtt_port = 443;

const char *mqtt_path = "/mqtt";

// Alternative broker example
// const char* mqtt_host = "test.mosquitto.org";
// const uint16_t mqtt_port = 8080;
// const char* mqtt_path = "/mqtt";

// Globals
WebSocketsClient wsClient;
MQTTPubSubClient mqtt;
// WiFiClientSecure wifiClient;
const char* figerPrint = "";
// Amazon's root CA. This should be the same for everyone.
const char* rootCACert = R"EOF(
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
)EOF";


// ---------- Domain Functions ---------- //

void handleSMS(const String &payload)
{

  Serial.println("Handling SMS");
  // SpiRamAllocator allocator;
  JsonDocument doc;

  DeserializationError err = deserializeJson(doc, payload);
  if (err)
  {
    Serial.print("JSON parse error: ");
    Serial.println(err.c_str());
    return;
  }

  const char *phone = doc["phoneNumber"] | "";
  const char *text = doc["text"] | "";

  if (strlen(phone) == 0 || strlen(text) == 0)
  {
    Serial.println("Invalid SMS payload");
    return;
  }

  bool ok = sendSms(String(phone), String(text));
  Serial.println(ok ? "SMS sent" : "SMS failed");
  if(ok){
    // send back to mqtt
    Serial.println("Sending SMS acknowledgement");
    mqtt.publish("sms/ack", payload);
  }

}

// ---------- Utility Functions ---------- //

void connectWiFi()
{
  // wifiClient.setCACert(root_ca);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void configureWebSocket()
{
  // wsClient.begin(mqtt_host, mqtt_port,mqtt_path, "mqtt");
  // wsClient.beginSSL(mqtt_host, 443,"/mqtt",SSL_FINGERPRINT_NULL, "mqtt");
  wsClient.beginSslWithCA(mqtt_host, mqtt_port,mqtt_path,rootCACert, "mqtt");
  //wsClient.setExtraHeaders("Host: e5836210af71.ngrok-free.app\r\n");

  // wsClient.beginSSL(mqtt_host, 443,"/mqtt", "mqtt");
  wsClient.setReconnectInterval(2000);
  delay(2000);
}

void configureMQTT()
{
  mqtt.begin(wsClient);
  mqtt.setOptions(10, true, 1000);
  mqtt.setWill("esp32/test", "offline", 1, true);
  mqtt.setKeepAliveTimeout(120);
  mqtt.setTimeout(20000);
}

void connectToMQTT()
{
  Serial.println("Connecting to MQTT broker...");
  while (!mqtt.connect("ESP32Clientasdfasdfasdfasdfasdfw3r2werafdsfasefd"))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" MQTT broker connected!");
}

void subscribeTopics()
{
  Serial.println("Subscribing to topics");

  mqtt.subscribe([](const String &topic, const String &payload, const size_t size)
                 { 
                  Serial.println("Received Data => Topic: " + topic);
                  if (topic == "sms/outbound"){
                    handleSMS(payload);
                  }
                });

  mqtt.subscribe("#", [](const String &payload, const size_t size)
                 {
    Serial.print("# ");
    Serial.println(payload); });

  Serial.println("Topics subscribed");
}

void reconnectWebSocketAndMQTT()
{
  Serial.println("WS not connected, restarting...");
  delay(1000);

  if (!wsClient.isConnected())
  {
    configureWebSocket();
    configureMQTT();
    connectToMQTT();
    subscribeTopics();
  }
}



// ---------- Arduino Standard Functions ---------- //

void setup()
{
  Serial.begin(9600);

  connectWiFi();
  configureWebSocket();
  configureMQTT();
  connectToMQTT();
  subscribeTopics();
  setupGSM();
}

void loop()
{
  mqtt.update();
  wsClient.loop();
  loopGSM();

  if (!wsClient.isConnected())
  {
    reconnectWebSocketAndMQTT();
  }
}
