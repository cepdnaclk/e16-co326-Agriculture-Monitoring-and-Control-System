#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define DEBUG 1
#define MSG_BUFFER_SIZE (25)

#ifdef DEBUG
  #define PRINT Serial.printf
#else
  #define PRINT(...)
#endif

// Wifi SSID and password
const char *ssid = "Dialog 4G 348";
const char *password = "E16389_Nadun";

// MQTT broker
const char *mqtt_server = "broker.hivemq.com";

// Subcribing topics
const char *control_signals_CONT = "CO326/2021/GH/1/demo/control_signals_COM";
const char *sr_temperature = "CO326/2021/GH/1/demo/temp_reading";
const char *sr_soil_water_fertilizer = "CO326/2021/GH/1/demo/soil_water_fertilizer";
const char *sr_humidity = "CO326/2021/GH/1/demo/humidity_reading";
const char *sr_light_intensity = "CO326/2021/GH/1/demo/light_intensity_reading";

//  Publishing topics
const char *sensor_readings_CONT = "CO326/2021/GH/1/demo/sensor_readings_COM";
const char *fan_topic = "CO326/2021/GH/1/demo/fan";
const char *water_fertilizer_topic = "CO326/2021/GH/1/demo/add_water_fertilizer";
const char *humidifier_topic = "CO326/2021/GH/1/demo/humidifier";
const char *lights_topic = "CO326/2021/GH/1/demo/lilghts";
// const char *watering_topic = "CO326/2021/GH/1/demo/watering";
// const char *fertilizer_topic = "CO326/2021/GH/1/demo/add_fertilizer";

// Sensor readings
int temperature = 0;
int soil_moisture = 100;
int humidity = 100;
int light_intensity = 100;
char water_tank[2] = "M";
char fertilizer_tank[2] = "M";

// Actuator readings
int fan_pre = -1, fan = 0;
int watering_pre = -1, watering = 0;
int fertilizer_pre = -1, fertilizer = 0;
// int water_fertilizer_pre = -1, water_fertilizer = 0;
int humidifier_pre = -1, humidifier = 0;
int light_pre = -1, light = 0;

// For triggering changes and buffering
int sensors_modified = 0;
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
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  char buf[messageTemp.length()];
  messageTemp.toCharArray(buf, sizeof(buf) + 1);

  // Update the control signal
  if (!strcmp(topic, control_signals_CONT))
  {
    sscanf(buf, "%d,%d,%d,%d,%d", &fan, &watering, &fertilizer, &humidifier, &light);
    PRINT("Control Signals Updated: Fan = %d\tWatering = %d\tFertilizer = %d\tHumidifier = %d\tLights = %d\n", fan, watering, fertilizer, humidifier, light);
    
    // Control signal to the fan
    if(fan_pre != fan)
    {
      itoa(fan, msg, 10);
      client.publish(fan_topic, msg, 2);
      fan_pre = fan;
    }

    // // Control signal to the Water Pump
    // if(watering_pre != watering)
    // {
    //   itoa(watering, msg, 10);
    //   client.publish(watering_topic, msg, 2);
    //   watering_pre = watering;
    // }
    
    // // Control signal to the Fertilizer Pump
    // if(fertilizer_pre != fertilizer)
    // {
    //   itoa(fertilizer, msg, 10);
    //   client.publish(fertilizer_topic, msg, 2);
    //   fertilizer_pre = fertilizer;
    // }

    // Control signal to the Water and Fertilizer Pump
    if((watering_pre != watering) || (fertilizer_pre != fertilizer))
    {
      snprintf(msg, MSG_BUFFER_SIZE, "%d%d", watering, fertilizer);
      client.publish(water_fertilizer_topic, msg, 2);
      fertilizer_pre = fertilizer;
    }

    // Control signal to the Humidifier
    if(humidifier_pre != humidifier)
    {
      itoa(humidifier, msg, 10);
      client.publish(humidifier_topic, msg, 2);
      humidifier_pre = humidifier;
    }

    // Control signal to the Lights
    if(light_pre != light)
    {
      itoa(light, msg, 10);
      client.publish(lights_topic, msg, 2);
      light_pre = light;
    }
  }

  // Update on temperature reading
  if (!strcmp(topic, sr_temperature))
  {
    sscanf(buf, "%d", &temperature);
    PRINT("Temperature Updated = %d\n", temperature);
    sensors_modified = 1;
  }

  // Update on Soil-Moisture reading, Water tank level and Fertilizer tank level
  if (!strcmp(topic, sr_soil_water_fertilizer))
  {
    sscanf(buf, "%d,%c,%c", &soil_moisture, &water_tank, &fertilizer_tank);
    PRINT("Moisture Updated = %d\tWater level = %c\tFertilizer level = %c\n", soil_moisture, water_tank[0], fertilizer_tank[0]);
    sensors_modified = 1;
  }

  // Update on humidity reading
  if (!strcmp(topic, sr_humidity))
  {
    sscanf(buf, "%d", &humidity);
    PRINT("Humidity Updated = %d\n", humidity);
    sensors_modified = 1;
  }

  // Update on light intensity reading
  if (!strcmp(topic, sr_light_intensity))
  {
    sscanf(buf, "%d", &light_intensity);
    PRINT("Light Intensity Updated = %d\n", light_intensity);
    sensors_modified = 1;
  }
}


void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Greenhouse Communicator"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe(control_signals_CONT);
      client.subscribe(sr_temperature);
      client.subscribe(sr_soil_water_fertilizer);
      client.subscribe(sr_humidity);
      client.subscribe(sr_light_intensity);
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
      0);        /* Core where the task should run */
}


void loop()
{
  //Send Sensor Readings to the Controller
  if (sensors_modified && client.connected())
  {
    snprintf(msg, MSG_BUFFER_SIZE, "%d,%d,%d,%d,%c,%c", temperature, soil_moisture, humidity, light_intensity, water_tank[0], fertilizer_tank[0]);
    int status = client.publish(sensor_readings_CONT, msg, 2);
    PRINT("Published to Controller: %s\n", (status) ? "Success" : "Failed");
    sensors_modified = 0;
  }

  client.loop();

  delay(10);
}