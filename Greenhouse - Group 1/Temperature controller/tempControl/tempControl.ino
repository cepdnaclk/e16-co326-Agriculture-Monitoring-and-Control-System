#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <string> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define LED1 D0
#define LED2 D1
#define LED3 D2

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
boolean initialized = false;



int mySensVals[12] = { 14, 13, 11, 22, 23, 24, 27, 28, 29, 37, 42, 39}; // test values
int value = 0;
// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "CO326/2021/GH/1/demo/fan";
const char *topic2 = "CO326/2021/GH/1/demo/temp_reading";
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

const int mqtt_port = 1883;
char * message = "00";
int interval = 10000;
int minT = 0;
int maxT = 50;
int temp = 0;
int sensorValue = 0;  // value read from the pot
unsigned long time1 = 0;
WiFiClient espClient;
PubSubClient client(espClient);


char webpage2[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
<h1>My First Heading</h1>
<p>My first paragraph.</p>
</body>
</html>
)=====";

// GUI for initialize temperature sensor with sensor number(If multiple sensors are used). This data will be stored at EEPROM of microcontroller
char webpage[] PROGMEM = R"=====(
<html>
<head>
  <style>
      html, body {
      display: flex;
      justify-content: center;
      height: 100%;
      }
      body, div, h1, form, input, p { 
      padding: 0;
      margin: 0;
      outline: none;
      font-family: Roboto, Arial, sans-serif;
      font-size: 16px;
      background: linear-gradient(to right, #4776e6, #8e54e9);
      }
      h1 {
      padding: 10px 0;
      font-size: 32px;
      font-weight: 300;
      text-align: center;
      }
      p {
      font-size: 20px;
      }
      hr {
      color: #a9a9a9;
      opacity: 0.3;
      }
      .main-block {
      max-width: 540px; 
      min-height: 500px; 
      padding: 10px 0;
      margin: auto;
      border-radius: 5px; 
      border: solid 1px #ccc;
      box-shadow: 1px 2px 5px rgba(0,0,0,.31); 
      background: linear-gradient(to right, #4776e6, #8e54e9);
      }
      form {
      margin: 0 30px;
      }
     
      label#icon {
      margin: 0; 
      border-radius: 5px 0 0 5px;
      }
      
     
      
      input[type=text] {
      width: calc(100% - 57px);
      height: 50px;
      margin: 13px 0 0 -5px;
      padding-left: 10px; 
      border-radius: 0 5px 5px 0;
      border: solid 1px #cbc9c9; 
      box-shadow: 1px 2px 5px rgba(0,0,0,.09); 
      background: #fff; 
       font-size: 20px;
      }
     
    
      .btn-block {
      margin-top: 10px;
      text-align: center;
      }
      button {
      width: 100%;
      padding: 10px 0;
      margin: 10px auto;
      border-radius: 5px; 
      border: none;
      background: #1c87c9; 
      font-size: 20px;
      font-weight: 600;
      color: #fff;
      }
      button:hover {
      background: #26a9e0;
      }
    </style>
</head>
 <body>
    <div class="main-block">
      <h1><b>Device Setup!</b></h1>
      <br>
      <form>
        <hr>        
        <hr>
        <br>
        <label  for="Type"></i></label>
        <input type="text" name="Type" id="Type" placeholder="Type" required/>
        
        <label  for="Sensor"></i></label>
        <input type="text" name="Sensor" id="Sensor" placeholder="Sensor No" required/>
        <br>
        <br>
        <hr>
        <hr>
        <br>
        <div class="btn-block">
          <p>Please provide the Sensor number and credentials</a>.</p>
          <br>
          
        </div>
        <button onclick="myFunction()">Submit</button>
      </form>
    </div>
  </body>
<script>
function myFunction()
{
  /*
  console.log("button was clicked!");
  var xhr = new XMLHttpRequest();
  var url = "/set";
  xhr.onreadystatechange = function() 
  {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("led-state").innerHTML = this.responseText;
    }
  };
  xhr.open("GET", url, true);
  xhr.send();
   */
  //var data = {}; 
  var Type = document.getElementById("Type").value;
  var no = document.getElementById("Sensor").value;
 
  var json = JSON.stringify({ Type, no });
   console.log(json);
  var xhr = new XMLHttpRequest();
    var URL = '/set';
    xhr.open("POST", URL, true);
    xhr.setRequestHeader('Content-type','application/json; charset=utf-8');
    
  xhr.onload = function()
   {
      alert("Sensor " + no + " for " + Type + " is initialized.");
      window.close();
      //self.close();
   }
   xhr.send(json);
  
    
  
};
</script>
</html>
)=====";



// initialize values
void setValues()
{
 
      String message = "Body received:\n";
             message += webServer.arg("plain");
             message += "\n";
 
      webServer.send(200, "text/plain", message);
      Serial.println(message);
  
  Serial.println("Now true");
  
  delay(2000);
  webServer.close();
  initialized = true;
}

// process requests at DNS server
void serveReq()
{
  while(!initialized)
  {
    dnsServer.processNextRequest();
    webServer.handleClient();
  
  }

}

// configure DNS server to setup a network to initialize sensors
void configDevice()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Temperature Sensor Setup!");
  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, "www.tmpsensor.com", apIP);

  Serial.println("Server Up!");

  webServer.onNotFound([]() 
  {
    webServer.send(200, "text/html", webpage);
  });

  webServer.on("/set", setValues);
  webServer.begin();
  serveReq();
}

// connect to WiFi using this GUI. WiFi credentials will be stored at EEPROM. Therefore it will connect automatically when restarting or disconnection happened
void connect()
{ 
  Serial.println("**********************************Connecting to wifi*************************************"); 
  WiFiManager wifiManager;
  wifiManager.setCustomHeadElement("<style>button{border-radius: 36px; background: black;color: white;} body{ margin: 20px 40px;font-size: 1.2rem;letter-spacing: 1px;background: linear-gradient(to right, #4776e6, #8e54e9);background-size: auto;}</style>");
  wifiManager.autoConnect("Ventilation Controller");
  Serial.println("**********************************Connected to wifi*************************************");
  
}

//handling the incomming message from communication module
void handleMessage(char *topic, byte *payload, int length) 
{
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 //Serial.print("Message:");

// This is for demostration purpose. For actual implementation outputs are analog. Relays will be used to work with Fans

if((char) payload[0] == '0')// fans off
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  Serial.println("0 arrived");
}
if((char) payload[0] == '1') // fans at level 1
{
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  Serial.println("1 arrived");
}
if((char) payload[0] == '2') // fans at level 2
{
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, LOW);
  Serial.println("2 arrived");
}
if((char) payload[0] == '3')  // fans at level 3
{
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  Serial.println("3 arrived");
}


 
 for (int i = 0; i < length; i++) // print the message received from brocker
 {
     Serial.print((char) payload[i]);   
 }
 Serial.println();
 Serial.println("-----------------------");

 
}


// setting up the nodemcu. 
void setup() 
{
  Serial.begin(9600);
  

pinMode(LED1, OUTPUT);    // LED pin as output.
pinMode(LED2, OUTPUT);    // LED pin as output.
pinMode(LED3, OUTPUT);    // LED pin as output.

digitalWrite(LED1, LOW);
digitalWrite(LED2, LOW);
digitalWrite(LED3, LOW);


Serial.println("*******************************************************************************Device Setup!**************************************************************");

configDevice(); // to configure sensors
connect(); // connet to wifi
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(handleMessage);
 
 while (!client.connected()) 
 {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     
     if (client.connect(client_id.c_str())) 
     {
         Serial.println("Public emqx mqtt broker connected");
     } 
     else 
     {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }

 }

 // publish and subscribe
 //client.publish(topic, message);
 client.subscribe(topic);

}

void loop() 
{  

  if(millis() - time1 > 5000) // 1 sensor reading per 5s
  {   
      time1= millis();
      temp = mySensVals[value]; // simulation reading
      //sensorValue = analogRead(analogInPin); // actual reading 
      //temp = map(sensorValue, 0, 1024, minT, maxT);
      String str = (String)(temp);
      str.toCharArray(message, 3);
      Serial.printf("The temperature: %s \n", message);
      client.publish(topic2, message);
      value++;
      
      if(value == 12) // if all pre setup values are outputed, start from begining again
      {
        value = 0;
      }
  }
  if (!(WiFi.status() == WL_CONNECTED) ) // if wifi is disconnected, reconnect using credentials in EEPROM
  {
     connect();
  }
  
  client.loop();  // handle incoming messages
 
}
