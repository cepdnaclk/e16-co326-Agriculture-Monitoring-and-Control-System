"""
    mqtt publisher for water quality topics
    @authur:thusharakart (Rusiru Thushara E/16/369)

"""

import sys
import paho.mqtt.client as mqtt

# connection message
def on_connect(mqttc, obj, flags, rc):
    print("Connedted :: " + str(rc))

# message
def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

# publish
def on_publish(mqttc, obj, mid):
    print("published : " + str(mid))

# initaiate a mqqtt client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish

# connect to the public brocker
client.connect("broker.hivemq.com", 1883, 8000)
client.loop_start()

import random
import time

while(True):

    # generating random data and publishing to the topics
    time.sleep(.1)
    tmp = 25 + 0.1*(random.randrange(-40,40))
    ph = random.randrange(4,9)
    light =  random.randrange(20,80)
    turbidity = random.randrange(2,10)

    infot = client.publish("CO326/2021/PDU/water_quality_temp/", tmp, qos=2)
    infot = client.publish("CO326/2021/PDU/water_quality_ph/", ph, qos=2)
    infot = client.publish("CO326/2021/PDU/water_quality_lightintensity/", light, qos=2)
    infot = client.publish("CO326/2021/PDU/water_quality_turbidity/", turbidity, qos=2)
    
    infot.wait_for_publish()


 
 
 