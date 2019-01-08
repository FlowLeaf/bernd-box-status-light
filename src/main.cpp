#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "PROTOHAUS";         //replace this with your WiFi network name
const char *password = "PH-Wlan-2016#"; //replace this with your WiFi network password
const char *mqtt_client_id = "bernd_box_status_light_1";
const char *measurement_report_active_topic = "tele/bernd_box_1/measurement_report_active";
const char *pump_active_topic = "tele/bernd_box_1/pump_active";

int status_led_pin = 2;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void measurement_report_active_callback(bool)
{
  Serial.println("measurement_report_active_callback");
}

void pump_active_callback(bool)
{
  Serial.println("pump_active_callback");
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.printf("Message arraived [%s]", topic);

  bool active;

  char received = (char)payload[0];
  if (received == 't' || received == 'T')
  {
    Serial.println("Got a T");
    // LED output is inverter
    digitalWrite(status_led_pin, LOW);
    active = true;
  }
  else
  {
    Serial.println("Got a not T");
    // LED output is inverter
    digitalWrite(status_led_pin, LOW);
  }

  if (strcmp(measurement_report_active_topic, topic) == 0)
  {
    measurement_report_active_callback(active);
  }
  if (strcmp(pump_active_topic, topic) == 0)
  {
    pump_active_callback(active);
  }
}

void reconnect()
{
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(mqtt_client_id))
    {
      mqttClient.subscribe(mqtt_measurement_report_active_topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer("192.168.0.33", 1883);
  mqttClient.setCallback(callback);

  pinMode(status_led_pin, OUTPUT);
  digitalWrite(status_led_pin, HIGH);

  reconnect();
  mqttClient.publish("hi_there", "what_up");
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
}