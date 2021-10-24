#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define DEBUG 1
#ifdef DEBUG
#define PRINT Serial.printf
#else
#define PRINT(...)
#endif

#define MSG_BUFFER_SIZE (25)

// ledPin refers to ESP32 GPIO 23
const int ledPin = 2;

const char *ssid = "SLT-Fiber-8E73";
const char *password = "thilniwp";


// implementation topics
const char *power_topic = "/CO326/2021/IVCU/PWR";
const char *soil_moist_topic = "/CO326/2021/IVCU/MS";
const char *Pump_topic = "/CO326/2021/IVCU/WP ";

const char *Valve_topic="/CO326/2021/IVCU/VS ";

//  Publishing topics
// demo topics
const char *control_signals_COM = "/CO326/2021/IVCU/control_signals_COM";
const char *thresholds = "/CO326/2021/IVCU/thresholds";


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

  if (!strcmp(topic, thresholds))
  {
    sscanf(buf, "%d,",  &soil_moisture_limit);
    PRINT("Limits updated Soil_moist=%d\t", soil_moisture_limit);
  }

  if (!strcmp(topic, sensor_readings_COM))
  {
    sscanf(buf, "%d",&soil_moisture);
    PRINT("Readings updated Soil_moist = %d\t",  soil_moisture);

    
    itoa(soil_moisture, msg, 10);
    client.publish(soil_moist_topic, msg);
   

    
    client.publish(sensor_readings_SCADA, buf, 2);
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
      client.subscribe(sensor_readings_COM);
      client.subscribe(thresholds);
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


void Task1code(void *parameter)
{
  for (;;)
  {
    if (!client.connected())
    {
      reconnect();
    }
    // client.loop();
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

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1",   /* Name of the task */
      10000,     /* Stack size in words */
      NULL,      /* Task input parameter */
      0,         /* Priority of the task */
      &Task1,    /* Task handle. */
      0);        /* Core */
}




