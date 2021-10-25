
#include <ESP8266WiFi.h>    //library to connect with wifi
#include <PubSubClient.h>   //MQTT communication library
#include <WiFiManager.h>    //For changing connected wifi network 
#include <DNSServer.h>
#include <ESP8266WebServer.h>

WiFiManager wifiManager;          //initiate wifimanager 
WiFiClient espClient;             //create a client
PubSubClient client(espClient);   //assign the client as a mqtt client

//const char *mqtt_server = "broker.emqx.io";         // MQTT broker used EMQX public broker
const char *mqtt_server = "broker.hivemq.com";         // MQTT broker used HiveMQ public broker

String mac_add = ""; //take the mac address to a string(optional)

//subscribed topic  Pump control signal 1/0 to turn the motor on and off
const char *pump_control_signal =  "CO326/2021/IVCU/MC";  

/* ============================---connect to WiFi----=============================== */
void setup_wifi() {

  delay(100);   
  Serial.println();
  Serial.print("Connecting to ");

  wifiManager.autoConnect("Pump Control", "12345678");    // make the local network, if previous network is not available(flash memory keeps record of last known network)

  randomSeed(micros());                                   // make a random seed
  
 
  /* ======== convert string ---> char array ===========*/
  //if need can specify topics with the mac address added.(added uniqueness to topic)
  mac_add = WiFi.macAddress();
  //char mac_address[50] = "";
  /*
  int i = 0;
  for (i = 0; i < 50; i++) {
    mac_address[i] = mac_add[i];			    //Take the mac address of the device(esp8266) 
  }
  */
  /* ===================================================*/

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(mac_add);
}
/*====================================================================================*/

/*==================================--- callback ---=================================*/
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] \n");
  String s = "";

  //capture the recieved message to a string
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.print("\n");
    s = s + (char)payload[i];
  }

  int input_signal = s.toInt();   //convert to integer
  
  switch (input_signal)
  {
    case 1:                               // make built-in led ON (send HIGH signal to the relay to turn socket ON)
      digitalWrite(13, HIGH);
      break;
    case 0:                               // make built-in led OFF(send LOW signal to the relay to turn socket OFF)
      digitalWrite(13, LOW);
      break;
  }
}

/*=====================================================================================*/

/*=================================--- reconnect process ---===================================*/
void reconnect() {

  while (!client.connected()) {                     // Loop until reconnected                  
 
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {         // c_str gives a pointer to invoking String
      Serial.println("connected");
      client.subscribe(pump_control_signal);        //subscribe to the topic to recieve pump control on/off signals from the Irrigation valve control unit
    } else {
      Serial.print("failed, reconnecting...");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds");

      // Wait 5 seconds before retrying
      int count = 0;
      while (count <= 5)                           // if WiFi is not avilaible
      {
        digitalWrite(4, HIGH);                     // blink the green LED (indicate trying to connect to WiFi)
        delay(500);
        digitalWrite(4, LOW);
        delay(500);
        count++;
      }
    }
  }
  digitalWrite(4, LOW);                            // turn on the green LED when WiFi is avalible
}

/*======================================================================================*/

/*==============================--- setup mode---===========================================*/
void setup() {
  pinMode(13, OUTPUT);                       // Configure Pin 13(D7) as a Output
  pinMode(4, OUTPUT);                        // Configure Pin 4(D2) as a Output
  digitalWrite(13, LOW);                     // turn off the power socket at powering up trigger the relay
  int count = 0;

  Serial.begin(115200);
  while (count <= 5)                              // if WiFi is not avilaible
  {
    digitalWrite(4, LOW);                        // blink the green LED
    delay(500);
    digitalWrite(4, HIGH);
    delay(500);
    count++;
  }

  //wifiManager.resetSettings();

  setup_wifi();                                  // setup the WiFi connection

  client.setServer(mqtt_server, 1883);           // set the mqtt server
  client.setCallback(callback);                  // set the callback function
}

/*======================================================================================*/

/*====================================--- loop process---======================================*/
void loop() {

  if (!client.connected()) {                    //if MQTT is disconnected re-connect using reconnect() function
    reconnect();                                                    
  }
  
  client.loop();                              //loop through the process

}
/*=====================================================================================*/
