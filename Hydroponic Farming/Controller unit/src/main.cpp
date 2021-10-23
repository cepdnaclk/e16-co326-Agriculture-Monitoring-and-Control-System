#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#include <iostream>
#include <string>

#define DHTTYPE    DHT11     // DHT 11
#define DHTPIN D2
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
//#define LED D0
sensors_event_t event;

//LED for the resevior unit
#define LED_RM D1 //R to F motor
//#define LED_RP D2//Pump
#define LED_RD D3 //for water drain

//LED for the Fertilezer Unit
#define LED_F D4

//Groing Chamber LEDs
#define LED_GP D5 //Ph value
#define LED_GD D0//DO value

//Light intesity control
#define LED_L D7

//Temperature Control Unit
#define LED_T D8 //for water drain

//humidity control unit
#define LED_H D6

const char* SSID = "Dialog 4G 304";
const char* PWD = "subnewnew";

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "CO326/proj";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

//connect to the wifi
//connect to the wifi
void connectToWiFi() {
  Serial.print("Connectiog");
 
  WiFi.begin(SSID, PWD);
  //Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
}

//handling the incomming message
void handleMessage(char *topic, byte *payload, int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
     
 }
 Serial.println();
 Serial.println("-----------------------");

 /*
 ============================             Control Units   ===============================
 R1 - Resevior to Fertilizer Motort   - LED_RM D1
 R2 - Resevior Levl Low               - LED_RP D2
 R3 - Resevior Level High             - LED_RP D2
 R4 - Resevior Drain                  - LED_RD D3
 F1 - Fertilizer Unit                 - LED_F  D4
 GP - Growing Chamber Ph Value        - LED_GP D5 
 GD - Growing Chamber DO value        - LED_GD D0
 GW - Growing Chamber Water Level     - LED_GP D5 
 L1 - Light intensity Control Unit    - LED_L  D6
 T1 - Temperature and Airflow control - LED_T  D7
 H1 - Humidity Control Unit           - LED_H  D8
 */

 /*
 =================================  The fertilizer unit ================================
 1. Check whether the first character = F (70), second character = 1 (49) 
 2. Settig the F1_Ph_value by combining two numbers in 3rd,4th character , Deduct 48 to get the int value from ascii values
 */
//++++++++++++++++++++++++ F1 +++++++++++++++++++++++++++++++++++++++++++
//issue found : manual mode also going to the automatic if condition
//Solution : && payload[2]<58 & payload[2]>47 (only for 10 numbers)

 if(payload[0] == 70 &&  payload[1] == 49 && payload[2]<58 && payload[2]>47){//automatic mode
    int F1_Ph_value  = ((int)payload[2]-48)*10+(int)(payload[3])-48;
    Serial.println(F1_Ph_value);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/ph1", itoa(F1_Ph_value,sub_topic,10));
    Serial.println(itoa(F1_Ph_value,sub_topic,10));
      if(F1_Ph_value<55 && F1_Ph_value >0 ){//turn on LED
      digitalWrite(LED_F, HIGH);
      }
      else if(F1_Ph_value > 55 && F1_Ph_value < 90 ){
      digitalWrite(LED_F, LOW);
      }
  }
  if(payload[0] == 70 &&  payload[1] == 49 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_F, LOW);
    }
  
    else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_F, HIGH);
    }
  }

  /*
  ==============================  Resevior Unit   =====================================
  R1 - Resevior Motor
  R2 - Resevior Levl Low  - RP
  R3 - Resevior Level High  - RP
  R4 - Drain
  */

  //++++++++++++++++++++++++++++++   R1 Resevior Motor +++++++++++++++++++++++++++++++++++
  
  if(payload[0] == 82 &&  payload[1] == 49 && payload[2]<58 && payload[2]>47){   //automatic mode
    int SendToF  = ((int)payload[2])-48;
    Serial.println(SendToF);
      if(SendToF == 1 ){   //turn on LED
      digitalWrite(LED_RM, HIGH);
      }
      if(SendToF == 0 ){   //turn on LED
      digitalWrite(LED_RM, LOW);
      }
  }
  if(payload[0] == 82 &&  payload[1] == 50 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_RM, LOW);
    }
  
    if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_RM, HIGH);
    }
  }
   /*
  +++++++++++++++++++++  R2 - Resevior Level low   ++++++++++++++++++++++
  1. Check whether the first character = R (82), second character = 2 (50)
  if water lvel is low turn on pump
  if water level is high turn off pump
  */
  if(payload[0] == 82 &&  payload[1] == 50 && payload[2]<58 && payload[2]>47){   //automatic mode
    int water_low  = ((int)payload[2])-48;
    Serial.println(water_low);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/r1", itoa(water_low,sub_topic,10));
      if(water_low == 1 ){   //turn on LED
      //digitalWrite(LED_RP, HIGH);
      Serial.println("Warning : Water Level is Low");
      }
  }
  if(payload[0] == 82 &&  payload[1] == 50 &&  payload[2] == 95){//manual mode
    /*if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_RL, LOW);
    }*/
  
    if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      //digitalWrite(LED_RP, HIGH);
      Serial.println("Warning : Water Level is Low");
    }
  }
  
  /*
  +++++++++++++++++++++  R3 - Resevior Level High   ++++++++++++++++++++++
  1. Check whether the first character = R (82), second character = 3 (51)
  */
  if(payload[0] == 82 &&  payload[1] == 51 && payload[2]<58 && payload[2]>47){//automatic mode
    int water_high  = ((int)payload[2]-48);
    Serial.println(water_high);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/r2", itoa(water_high,sub_topic,10));
      if(water_high == 1 ){//turn on LED
      //digitalWrite(LED_RP, LOW);
      Serial.println("Warning : Water Level is High");
      }
      /*else if(water_high == 0){
      digitalWrite(LED_RH, LOW);
      }*/
  }
  if(payload[0] == 82 &&  payload[1] == 51 &&  payload[2] == 95 && payload[2]<58 && payload[2]>47){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      //digitalWrite(LED_RP, LOW);
      Serial.println("Warning : Water Level is High");
    }
  
    /*else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_RH, HIGH);
    }*/
  }

  /*
  +++++++++++++++++++++  R4 - Drain   ++++++++++++++++++++++
  1. Check whether the first character = R (82), second character = 4 (52)
  */
  if(payload[0] == 82 &&  payload[1] == 52 && payload[2]<58 && payload[2]>47){//automatic mode
    int drain  = ((int)payload[2]-48);
    Serial.println(drain);
      if(drain == 1 ){//turn on LED
      digitalWrite(LED_RD, HIGH);
      }
      else if(drain == 0){
      digitalWrite(LED_RD, LOW);
      }
  }
  if(payload[0] == 82 &&  payload[1] == 52 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_RD, LOW);
    }
  
    else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_RD, HIGH);
    }
  }

  /*
  =============================  Grouwing Chamber   =====================================
  GP - Ph Value
  GD - DO value
  */
  /*
  +++++++++++++++++++++++++++++   GP(71,80) - Chamber Ph value  ++++++++++++++++++++++++++++++++
  */
  if(payload[0] == 71 &&  payload[1] == 80 && payload[2]<58 && payload[2]>47){//automatic mode
    int GP_Ph_value  = ((int)payload[2]-48)*10+(int)(payload[3])-48;
    Serial.println(GP_Ph_value);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/ph2", itoa(GP_Ph_value,sub_topic,10));
      if(GP_Ph_value<55 && GP_Ph_value >0 ){//turn on LED
        digitalWrite(LED_GP, HIGH);
      }
      else if(GP_Ph_value > 55 && GP_Ph_value < 90 ){
        digitalWrite(LED_GP, LOW);
      }
  }
  if(payload[0] == 71 &&  payload[1] == 80 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_GP, LOW);
    }
  
    else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_GP, HIGH);
    }
  }

  /*
  +++++++++++++++++++++++++++++   GD(71,68) - Chamber DO value  ++++++++++++++++++++++++++++++++
  */

  if(payload[0] == 71 &&  payload[1] == 68 && payload[2]<58 && payload[2]>47){//automatic mode
    int GP_Ph_value  = ((int)payload[2]-48)*10+(int)(payload[3])-48;
    Serial.println(GP_Ph_value);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/ph2", itoa(GP_Ph_value,sub_topic,10));
      if(GP_Ph_value<50 && GP_Ph_value >0 ){//turn on LED
      digitalWrite(LED_GD, HIGH);
      }
      else if(GP_Ph_value > 50 && GP_Ph_value < 90 ){
      digitalWrite(LED_GD, LOW);
      }
   }
  if(payload[0] == 71 &&  payload[1] == 68 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_GD, LOW);
    }
  
    else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_GD, HIGH);
    }
  }
  
  /*
  =============================  Light Intensity Control Unit   =====================================
  */
  /*
  +++++++++++++++++++++++++++++  L1 (76,49) - Light Intensity  ++++++++++++++++++++++++++++++++++++++++++++
  */

  if(payload[0] == 76 &&  payload[1] == 49 && payload[2]<58 && payload[2]>47){//automatic mode
    int Light_intensity_value = ((int)payload[2]-48)*100+((int)(payload[3])-48)*10 + (int)(payload[4])-48;
    Serial.println(Light_intensity_value);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/lightintensity", itoa(Light_intensity_value,sub_topic,10));
      if(400 > Light_intensity_value  ){//turn on LED
        digitalWrite(LED_L , HIGH);
      }
      else if((700 >Light_intensity_value) && (400 < Light_intensity_value)){
        digitalWrite(LED_L, LOW);
      }
   }
  if(payload[0] == 76 &&  payload[1] == 49 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_L, LOW);
    }
  
    else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_L, HIGH);
    }
  }

  /*
  ============================= Temperature and Airflow control Unit ======================
  T1 (84,49)
  */
  if(payload[0] == 84 &&  payload[1] == 49 && payload[2]<58 && payload[2]>47){//automatic mode
    int inside_temperature_value = ((int)payload[2]-48)*10+((int)(payload[3])-48);
    int outside_temperature_value = ((int)payload[4]-48)*10+((int)(payload[5])-48);
    Serial.println(inside_temperature_value);
    Serial.println(outside_temperature_value);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/temp", itoa(inside_temperature_value,sub_topic,10));
      if((28 < inside_temperature_value) && ( 28 > outside_temperature_value)  ){//turn on LED
        digitalWrite(LED_T, HIGH);
      }
      else if((19 < inside_temperature_value) && ( 28 > inside_temperature_value)){
        digitalWrite(LED_T, LOW);
      }
   }
  if(payload[0] == 84 &&  payload[1] == 49 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_T, LOW);
    }
  
    else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_T, HIGH);
    }
  }

  /*
  ============================= Humidity COntrol Unit ======================
  H1 (72,49)
  */
  if(payload[0] == 72 &&  payload[1] == 49  && payload[2]<58 && payload[2]>47 ){//automatic mode
    int humidity_value = ((int)payload[2]-48)*10+((int)(payload[3])-48);
    Serial.println(humidity_value);
    char sub_topic[16];
    client.publish("CO326/proj/hf/1/sensor/humidity", itoa(humidity_value,sub_topic,10));
      if(60 > humidity_value ){//turn on LED
      digitalWrite(LED_H, HIGH);
      }
      else if((70 > humidity_value) && (60 < humidity_value )){
      digitalWrite(LED_H, LOW);
      }
   }
  if(payload[0] == 72 &&  payload[1] == 49 &&  payload[2] == 95){//manual mode
    if(payload[3] == 111 && payload[4] == 102 && payload[5] == 102){//turn off LED manually
      digitalWrite(LED_H, LOW);
    }
  
    else if(payload[3] == 111 && payload[4] == 110 ){//turn on LED manually
      digitalWrite(LED_H, HIGH);
    }
  }

}



void setup() {
  Serial.begin(9600);
  connectToWiFi();

  pinMode(LED_F, OUTPUT);
  digitalWrite(LED_F, LOW);
  pinMode(LED_RM, OUTPUT);
  digitalWrite(LED_RM, LOW);
  pinMode(LED_RD, OUTPUT);
  digitalWrite(LED_RD, LOW);
  pinMode(LED_GP, OUTPUT);
  digitalWrite(LED_GP, LOW);

  pinMode(LED_GD, OUTPUT);
  digitalWrite(LED_GD, LOW);
  pinMode(LED_L, OUTPUT);
  digitalWrite(LED_L, LOW);
  pinMode(LED_T, OUTPUT);
  digitalWrite(LED_T, LOW);
  pinMode(LED_H, OUTPUT);
  digitalWrite(LED_H, LOW);

 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(handleMessage);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str())) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }

 }

 // publish and subscribe
 //client.publish(topic, "Hi EMQ X I'm ESP32");
 client.subscribe(topic);

 dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  
}

float pre_tem = 1;
float pre_hum = 1;

void loop() {
  client.loop();

    delay(delayMS);
  // Get temperature event and print its value.
  
  dht.temperature().getEvent(&event);
  if(pre_tem != event.temperature )  {
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    }
    
    else {
      pre_tem  = event.temperature;
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));
      char sub_topic[16];
      client.publish("CO326/proj/hf/1/sensor/lightintensity", itoa(event.temperature,sub_topic,10));

      //client.publish("CO326/proj/hf/1/sensor/temp", event.temperature);
    }
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if(pre_hum != event.relative_humidity )  {
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    }
    else {
      pre_hum  = event.relative_humidity;
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
      if(60 > event.relative_humidity ){//turn on LED
      digitalWrite(LED_H, HIGH);
      char sub_topic[16];
      client.publish("CO326/proj/hf/1/sensor/lightintensity", itoa(event.relative_humidity,sub_topic,10));
      }
      else if( 60 < event.relative_humidity ){
      digitalWrite(LED_H, LOW);
      }
   





    }
  }

}