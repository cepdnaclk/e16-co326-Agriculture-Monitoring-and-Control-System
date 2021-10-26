# -*- coding: utf-8 -*-
# Commented out IPython magic to ensure Python compatibility.
import numpy as np
import pandas as pd

import warnings
warnings.filterwarnings('ignore')

import time
from datetime import datetime
from datetime import timedelta

import paho.mqtt.client as mqtt_client
import random
import json
import time

# %matplotlib inline

lag_value=18

DATAPATH = 'test2.csv'

data = pd.read_csv(DATAPATH)

# data.head()

"""## extract power usage values for control unit"""
sum=0
for i in data['control_unit_power']:
    sum+=float(i[8:12])

DATAPATH = 'test3.csv'

data = pd.read_csv(DATAPATH)

"""## extract power usage values for water quality unit"""

new_ph=[]
for i in data['water_quality_unit_power']:
    sum+=float(i[8:12])

broker = 'broker.hivemq.com'
port = 1883
topic = "CO326/2021/Cloud/soil_ph_predictions"
client_id = f'co326-mqtt-{random.randint(0, 1000)}'

publish_period = 0.1  # in seconds

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    def on_disconnect(client, userdata, rc):
        print("Disconnected, return code %d\n", rc)
        global isConnected
        isConnected = False

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.connect(broker, port)
    client.loop_start()
    return client


def publish(client, msg):
    result = client.publish(topic, msg, qos=2)
    status = result[0]
    if status == 0:
        print(f"Send `{msg}` to topic `{topic}`")
    else:
        print(f"Failed to send message to topic {topic}")


if __name__ == '__main__':
    client = connect_mqtt()
    # for i in range(prediction_days):
    publish(client, json.dumps({ "total_pwer": str(sum) }))



import time

time.time()

type(time.time())

from datetime import datetime

now = datetime.now()

current_time = now.strftime("%D %H:%M:%S")
print("Current Time =", current_time)
