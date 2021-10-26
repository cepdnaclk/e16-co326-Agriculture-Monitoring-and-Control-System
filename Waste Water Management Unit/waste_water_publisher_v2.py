import json
import paho.mqtt.client as mqtt
import random


def on_connect(mqttc, obj, flags, rc):
    print("rc: " + str(rc))


def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))


def on_publish(mqttc, obj, mid):
    print("mid: " + str(mid))


def setValveStates(tmp, ph, lvl, ec):
    valves = ["off", "off", "off"]
    if lvl < 75:
        valves[0] = "on"

    if tmp < 40 and 8.5 > ph > 4.5 and 4 > ec > 1:
        valves[1] = "on"

    else:
        valves[2] = "on"

    return valves


if __name__ == '__main__':
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_publish = on_publish

    client.connect("test.mosquitto.org", 1883, 60)
    client.loop_start()

    temp = random.randint(0, 50)
    phVal = random.randint(4, 9)
    elecConduc = random.randint(0, 5)
    tankLvl = random.randint(0, 100)

    valStates = setValveStates(temp, phVal, tankLvl, elecConduc)

    valve1 = valStates[0]
    valve2 = valStates[1]
    valve3 = valStates[2]

    # temp_json = {"value": temp}
    # phVal_json = {"value": phVal}
    # elecConduc_json = {"value": elecConduc}
    # tankLvl_json = {"value": tankLvl}
    # valveStates_json = {"stateV1": valStates[0], "stateV2": valStates[1]}

    topic = "CO326/2021/WWM/"

    infot = client.publish(topic + "1", temp, qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "2", phVal, qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "3", elecConduc, qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "4", tankLvl, qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "v1", valve1, qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "v2", valve2, qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "v3", valve3, qos=1)
    infot.wait_for_publish()
