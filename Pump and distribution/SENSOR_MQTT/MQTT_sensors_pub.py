import os
import time
import sys
import paho.mqtt.client as mqtt
import json
import random

# MQTT broker
MQTT_Broker = 'broker.mqttdashboard.com'

# topics 

TOPIC_HUMIDITY      = "/CO326/2021/PDU/water_quality_humidity"
TOPIC_PH            = "/CO326/2021/PDU/water_quality_ph"
TOPIC_TEMPERATURE   = "/CO326/2021/PDU/water_quality_temperature"
TOPIC_OPACITY       = "/CO326/2021/PDU/water_quality_opacity"

# Data capture and upload interval in seconds. 
INTERVAL=2

sensor_data = {'temperature': 0, 'humidity': 0, 'pH_Value': 0, 'opacity': 0 }

next_reading = time.time() 

client = mqtt.Client()

# Connect to broker
client.connect(MQTT_Broker, 8000, 60)

client.loop_start()

print('\n*****Reading sensor data*****\n')

def on_disconnect(client, userdata, message):
    print("disconnected")

try:
    while True:
           
        humidity = round(random.uniform(10.5, 5.54), 2)
        temperature = round(random.uniform(17.4, 23.7), 2)
        pH_Value = round(random.uniform(5.6, 6.2), 2)
        opacity = random.randint(2,10)
        
        # appending read data to an array
        sensor_data['temperature'] = temperature
        sensor_data['humidity'] = humidity
        sensor_data['pH_Value'] = pH_Value
        sensor_data['opacity'] = opacity
        
        # displaying the time of sensor reading
        t = time.localtime()
        current_time = time.strftime("Time : %H:%M:%S ", t,)
        
        # Printing sensor data
        print(current_time,'Temperature : ',json.dumps(sensor_data['temperature']),'°C','\tpH_Value : ',json.dumps(sensor_data['pH_Value']),'\tOpacity : ',json.dumps(sensor_data['opacity']),'NTU','\tHumidity : ',json.dumps(sensor_data['humidity']),'mm')
             
        # Sending sensor data to MQTT broker
        client.publish(TOPIC_HUMIDITY, json.dumps(sensor_data['humidity']), 1)
        client.publish(TOPIC_PH, json.dumps(sensor_data['temperature']), 1)
        client.publish(TOPIC_TEMPERATURE, json.dumps(sensor_data['pH_Value']), 1)
        client.publish(TOPIC_OPACITY, json.dumps(sensor_data['opacity']), 1)        
        
        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)
            
except KeyboardInterrupt:
    pass

client.loop_stop()
client.on_disconnect = on_disconnect

