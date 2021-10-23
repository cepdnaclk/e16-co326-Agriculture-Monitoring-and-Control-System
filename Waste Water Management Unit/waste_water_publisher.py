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
    valves = ["off", "off"]
    if lvl < 7:
        valves[0] = "on"

    if tmp < 40 and 9 > ph > 5 and ec < 50:
        valves[1] = "on"

    return valves


if __name__ == '__main__':
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_publish = on_publish

    client.connect("test.mosquitto.org", 1883, 60)
    client.loop_start()

    temp = random.randint(10, 70)
    phVal = random.randint(0, 14)
    elecConduc = random.randint(1, 100)
    tankLvl = random.randint(0, 10)

    valStates = setValveStates(temp, phVal, tankLvl, elecConduc)

    temp_json = {"value": temp}
    phVal_json = {"value": phVal}
    elecConduc_json = {"value": elecConduc}
    tankLvl_json = {"value": tankLvl}
    valveStates_json = {"stateV1": valStates[0], "stateV2": valStates[1]}

    topic = "CO326/2021/WWM/"

    infot = client.publish(topic + "1", json.dumps(temp_json), qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "2", json.dumps(phVal_json), qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "3", json.dumps(elecConduc_json), qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "4", json.dumps(tankLvl_json), qos=1)
    infot.wait_for_publish()

    infot = client.publish(topic + "5", json.dumps(valveStates_json), qos=1)
    infot.wait_for_publish()
