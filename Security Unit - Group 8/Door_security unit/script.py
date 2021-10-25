import paho.mqtt.client as mqtt_client
import random
import json
import datetime
import time

datastore = { "Ghouse": {
    "Door1": [
      {
      "Name": "kumar",
      "RFID": "2346987499",
      "State": 'out'
    },
     {
      "Name": "kamal",
      "RFID": "2346987500",
      "State": 'out'
    },
      {
      "Name": "roy",
      "RFID": "2346987501",
      "State": 'out'
    },
     {
      "Name": "sunil",
      "RFID": "2346987502",
      "State": 'In'
    },
      {
      "Name": "guyan",
      "RFID": "2346987503",
      "State": 'In'
    }
    ],
    "Door2":[ {
      "Name": "kamal",
      "RFID": "2346987498",
      "State": 'In'
    },
    {
      "Name": "nirmal",
      "RFID": "2346987498",
      "State": 'In'
    }

    ]
  }
}


broker = 'test.mosquitto.org'
port = 1883
topic = "/CO326/2021/SS/unit/Door"
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
    time.sleep(2)
    while True :
        state=False
        RFid=input('Please Scane the RFID tag : ')

        for data in datastore['Ghouse']['Door1']:
            if data['RFID']==RFid :
                state=True
                ct = datetime.datetime.now()
                print('You are good to go..')
                publish(client, json.dumps({ "val": [{'state':True},{'time':str(ct) },{'RFID':data['RFID']},{'User':data['Name']}]}))
        if state==False:
            print('An unauthorized person trying to enter')
            ct = datetime.datetime.now()
            publish(client, json.dumps({ "val": [{'state':True},{'time':str(ct) },{'RFID':RFid},{'User':'Invalid'}]}))

