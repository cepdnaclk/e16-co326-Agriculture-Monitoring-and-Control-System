import paho.mqtt.client as mqtt_client
import random
import json
import time

broker = 'test.mosquitto.org'
port = 1883
topic = "CO326/2021/GH/1/temp_reading"
client_id = f'co326-mqtt-{random.randint(0, 1000)}'

publish_period = 1 # in seconds

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
    result = client.publish(topic, msg, qos=1)
    status = result[0]
    if status == 0:
        print(f"Send `{msg}` to topic `{topic}`")
    else:
        print(f"Failed to send message to topic {topic}")


if __name__ == '__main__':
    client = connect_mqtt()
    while True:
        publish(client, json.dumps({ "val": random.randint(0, 100)}))
        time.sleep(publish_period)
    