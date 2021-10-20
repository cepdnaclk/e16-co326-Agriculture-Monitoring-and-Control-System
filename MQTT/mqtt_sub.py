import paho.mqtt.client as mqtt_client
import random
import json

broker = 'test.mosquitto.org'
port = 1883
topic = "CO326/2021/GH/1/temp_reading"
client_id = f'co326-mqtt-{random.randint(0, 1000)}'

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    def on_disconnect(client, userdata, rc):
        print("Disconnected, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.connect(broker, port)
    return client


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload}` from `{msg.topic}` topic")

    def on_sub(client, userdata, mid, granted_qos):
        print(f"Subscribes to {topic}` topic with qos={granted_qos}")

    client.subscribe(topic, qos=1)
    client.on_message = on_message
    client.on_subscribe = on_sub

if __name__ == '__main__':
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()