#include <Arduino.h>
#include <WiFi.h>
#include <ESP8266WiFi.h>    //library to connect with wifi
#include <PubSubClient.h>   //MQTT communication library
#include <WiFiManager.h>    //For changing connected wifi network 
#include <DNSServer.h>


#define MSG_BUFFER_SIZE (25)

const int ledPin = 2;

const char *ssid = "SLT-Fiber-8E73";
const char *password = "thilniwp";

// MQTT broker
const char *mqtt_server = "broker.hivemq.com";


// implementation topics
const char *power_topic = "/CO326/2021/IVCU/PWR";
const char *soil_moist_topic = "/CO326/2021/IVCU/MS";
const char *Water_Pump_topic = "/CO326/2021/IVCU/WP ";
const char *Valve_topic="/CO326/2021/IVCU/VS ";
const char *Pump_Control_topic="/CO326/2021/IVCU/VS ";

//  Publishing topics
// demo topics
const char *signals_soil_moist = "/CO326/2021/IVCU/soil_moist";
const char *signals_Pump_topic = "/CO326/2021/IVCU/Pump_topic";


int soil_moisture_limit = 50;
char msg[MSG_BUFFER_SIZE];

WiFiClient espClient;
PubSubClient client(espClient);
TaskHandle_t Task1;

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}

// on MQTT message
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    // Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  char buf[messageTemp.length()];
  messageTemp.toCharArray(buf, sizeof(buf) + 1);

  if (!strcmp(topic, t))
  {
    sscanf(buf, "%d,",  &soil_moisture_limit);
    PRINT("Soil_moist=%d\t", soil_moisture_limit);
  }

  if (!strcmp(topic, Water_Pump_topic))
  {
    sscanf(buf, "%d",&soil_moisture);
    PRINT("Readings updated Soil_moist = %d\t",  soil_moisture);

    
    itoa(soil_moisture, msg, 10);
    client.publish(signals_soil_moist);
    client.publish(Water_Pump_topic, buf, 2);
  }
}


void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("IVCU Controller"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe(signals_soil_moist);
      client.subscribe(signals_Pump_topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup()
{
  // initialize digital pin ledPin as an output.
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);

  // delete old config
  WiFi.disconnect(true);

  delay(3000);

  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.println("Wait for WiFi... ");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
}




