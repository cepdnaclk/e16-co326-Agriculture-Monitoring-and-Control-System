import paho.mqtt.client as mqtt_client
import random
import json
import time
import cv2
import base64

broker = 'broker.hivemq.com'
port = 1883
topic = "/CO326/2021/SS/1/image_feedback"
client_id = f'co326-mqtt-{random.randint(0, 1000)}'

publish_period = 15 # in seconds

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
    result = client.publish(topic, msg, qos=0)
    status = result[0]
    if status == 0:
        print(f"Send `{msg}` to topic `{topic}`")
    else:
        print(f"Failed to send message to topic {topic}")


if __name__ == '__main__':
    client = connect_mqtt()
    img_counter = 0

    while True:
        with open("Image_Feedback_{}.png".format(img_counter), "rb") as img:
            encoded_string = base64.b64encode(img.read())
            
        print("Image_Feedback_{}.png".format(img_counter))
        publish(client, encoded_string)
        img_counter += 1
        time.sleep(publish_period)