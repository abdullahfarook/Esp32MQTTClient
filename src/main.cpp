#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

const char* ssid = "Dure";
const char* pass = "dure786*";
   // MQTT Broker
//    const char *mqtt_broker = "broker.emqx.io";
   const char *mqtt_broker = "iot.coreflux.cloud";
   const char *topic = "test/esp32";
   const char *mqtt_username = "emqx";
   const char *mqtt_password = "public";
   const int mqtt_port = 1883;
const int ledPin = 4; // GPIO2 is usually the onboard LED on ESP32

WiFiClient espClient;
PubSubClient client(espClient);
void setupWifi(){
  delay(100);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("-");
  }

  Serial.print("\nConnected to ");
  Serial.println(ssid);
  Serial.print("\nIP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {
  // Loop until you are reconnected
  while (!client.connected()) {
    Serial.print("\nAttempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("topic/name1", "Calculating....");
      // ... and resubscribe
      client.subscribe("topic/name2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '0') {
    digitalWrite(ledPin, LOW);   // Turn the LED off 
  } else {
    digitalWrite(ledPin, HIGH);  // Turn the LED on 
  }
}
void setupMQTT(){
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
       String client_id = "esp32-client-";
       client_id += String(WiFi.macAddress());
       Serial.printf("The client %s is conneting..... to the public mqtt broker\n", client_id.c_str());
       if (client.connect(client_id.c_str())) {
           Serial.println("Public emqx mqtt broker connected");
       } else {
           Serial.print("failed with state ");
           Serial.println(client.state());
           delay(2000);
       }
   }
}
void setup() {
    Serial.begin(9600);
    setupWifi();
    setupMQTT();
    pinMode(ledPin, OUTPUT);
    client.subscribe("test/#");
    client.publish(topic, "Hi EMQX I'm ESP32 ^^");
// write your initialization code here
}

void loop() {
    if (!client.connected()) {
        reconnect();
    } 
    client.loop();
    delay(1000);
    // client.publish(topic, "Hi EMQX I'm ESP32 ^^");
}
