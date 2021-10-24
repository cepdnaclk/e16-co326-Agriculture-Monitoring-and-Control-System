
import os
import time
import sys
import paho.mqtt.client as mqtt
import json
import random
import time

broker="broker.mqttdashboard.com" #IP of your Raspberry Pi

def on_message(message):
    print("Message \'" + message.message + "\' received under topic \'" + message.topic + "\'");


client= mqtt.Client()
INTERVAL=2

next_reading = time.time() 

TOPIC_HUMIDITY      = "/CO326/2021/PDU/water_quality_humidity"
TOPIC_PH            = "/CO326/2021/PDU/water_quality_ph"
TOPIC_TEMPERATURE   = "/CO326/2021/PDU/water_quality_temperature"
TOPIC_OPACITY       = "/CO326/2021/PDU/water_quality_opacity"


print("Connecting to broker...")
print("Subscribing...")
client.connect(broker)

# subscribing to the sensor topic
client.subscribe(TOPIC_HUMIDITY)
print("Successfully subscribed to ",TOPIC_HUMIDITY)
time.sleep(2)
client.subscribe(TOPIC_PH)
print("Successfully subscribed to ",TOPIC_PH)
time.sleep(2)
client.subscribe(TOPIC_TEMPERATURE)
print("Successfully subscribed to ",TOPIC_TEMPERATURE)
time.sleep(2)
client.subscribe(TOPIC_OPACITY)
print("Successfully subscribed to ",TOPIC_OPACITY)

while True:
    client.on_message=on_message  
    t = time.localtime()
    current_time = time.strftime("@ %H:%M:%S: ", t,)
    #print(current_time,"received data")
    next_reading += INTERVAL
    sleep_time = next_reading-time.time()
    if sleep_time > 0:
        time.sleep(sleep_time)
        
client.loop_forever()

