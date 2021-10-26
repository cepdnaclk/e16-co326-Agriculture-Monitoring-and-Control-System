#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

// #define LED D4
#define SOIL_MOIST A0
#define WATER_H D0
#define WATER_M D1
#define WATER_L D2
#define FERT_H D3
#define FERT_M D4
#define FERT_L D5
#define CONTROL_W D6
#define CONTROL_F D7



const char* SSID = "Dialog 4G merlin";
const char* PWD = "6EF14Cc3";
int soil;
int tm;
// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic1 = "CO326/2021/GH/1/demo/soil_water_fertilizer";
const char *topic2 = "CO326/2021/GH/1/demo/add_water_fertilizer";
const char *init_msg = "Hello"; 
const int mqtt_port = 1883;
const char*msg = ""; 
const char water[26] = "HHMMLLMMHHMLLMMMLLLLMMHHH";
// const char* fert  = "MMHHMMMMHHMMLLMMMMLLLLMMH";

WiFiClient espClient;
PubSubClient client(espClient);

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
void handleMessage(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (unsigned int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
     
 }
 Serial.println();
 Serial.println("-----------------------");
if (length == 2){
  if (payload[0] == 48)
    digitalWrite(CONTROL_W,LOW);
  else if (payload[0] == 49)
    digitalWrite(CONTROL_W,HIGH);
  
  if (payload[1] == 48)
    digitalWrite(CONTROL_F,LOW);
  else if (payload[1] == 49)
    digitalWrite(CONTROL_F,HIGH);
  
}

}



void setup() {
  Serial.begin(9600);
  connectToWiFi();

  tm = 0;
  pinMode(SOIL_MOIST, INPUT);
  pinMode(WATER_H, INPUT);
  pinMode(WATER_M, INPUT);
  pinMode(WATER_L, INPUT);
  pinMode(FERT_H, INPUT);
  pinMode(FERT_M, INPUT);
  pinMode(FERT_L, INPUT);
  pinMode(CONTROL_W, OUTPUT);
  pinMode(CONTROL_F,OUTPUT);

  digitalWrite(CONTROL_F,LOW);
  digitalWrite(CONTROL_W,LOW);

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
 client.publish(topic1,init_msg);
 client.subscribe(topic2);
//  client.publish("CO326/proj/soil",topic);
 

}

void loop() {
  char message_[7];
  soil = analogRead(SOIL_MOIST);
  soil = map(soil,0,1024,0,100);
  if(soil==100){
    message_[0] = '1';
    message_[1] = '0';
    message_[2] = '0';
  }
  else{
    message_[0] = '0';
    message_[2] = 48+ soil%10;
    message_[1] = 48 + soil/10;
  }
  if (digitalRead(WATER_H) == 1)
  {
    message_[4] = 'H';
  }else if (digitalRead(WATER_M) == 1)
  {
    message_[4] = 'M';
  }else if (digitalRead(WATER_L) == 1)
  {
    message_[4] = 'L';
  }else 
    message_[4] = 'X';

  if (digitalRead(FERT_H) == 1)
  {
    message_[6] = 'H';
  }else if (digitalRead(FERT_M) == 1)
  {
    message_[6] = 'M';
  }else if (digitalRead(FERT_L) == 1)
  {
    message_[6] = 'L';
  }else 
    message_[6] = 'X';  
  message_[3] = ',';
  message_[5] = ',';
 
  message_[4] = water[tm]; 
  message_[6] = water[tm+2];
  tm++;
  if (tm == 23)
  {
    tm = 0;
  }
  

  // Serial.printf("\nsoil moisture_ : %d\n",soil);
  client.publish(topic1,message_,2);
  delay(5000);
  client.loop();
}