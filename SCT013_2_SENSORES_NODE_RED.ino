#include <WiFi.h>
#include <PubSubClient.h>
#include <EmonLib.h>

#define WIFI_SSID "E****l"
#define WIFI_PASSWORD "******"
#define MQTT_SERVER "****"
#define MQTT_PORT 1883
#define MQTT_TOPIC1 "corrente/fase1"
#define MQTT_TOPIC2 "corrente/fase2"

#define SENSOR1_PIN 34
#define SENSOR2_PIN 35

#define ADC_BITS    12;


EnergyMonitor emon1;
EnergyMonitor emon2;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  delay(1000);

  emon1.current(SENSOR1_PIN, 90.91);
  emon2.current(SENSOR2_PIN, 90.91);
}

void loop() {
  WiFiClient client;
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  double current1 = emon1.calcIrms(1480); // Período de amostragem em microssegundos (1480 = 60Hz, 1860 = 50Hz)
  double current2 = emon2.calcIrms(1480);

  char payload[100];
  snprintf(payload, sizeof(payload), "%.6f", current1);
  mqttClient.publish(MQTT_TOPIC1, payload);
  Serial.println(payload);

  snprintf(payload, sizeof(payload), "%.6f", current2);
  mqttClient.publish(MQTT_TOPIC2, payload);
  Serial.println(payload);

  delay(3000);
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
