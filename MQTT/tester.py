# CO326 project: PLC MQTT bridge

import paho.mqtt.client as mqtt 
from PLC import PLC
from show import show
import time

broker_address   = "broker.hivemq.com"
OUT_LVL_TANK     = "/CO326/2021/PDU/levels/tank"
OUT_LVL_SOURCE   = "/CO326/2021/PDU/levels/source"
OUT_QLTY_PH      = "/CO326/2021/PDU/water_quality_ph"
OUT_QLTY_OPACITY = "/CO326/2021/PDU/water_quality_opacity"
OUT_WARNINGS     = "/CO326/2021/PDU/warnings"
OUT_PUMP_STATE   = "/CO326/2021/PDU/pump/state"

IN_PUMP_CONTROL   = "/CO326/2021/PDU/pump/control"
IN_GH_SUPPLY      = "/CO326/2021/GH/GreenHouseID/water_supply_on_off"
IN_HYDROPN_SUPPLY = "/CO326/2021/GH/HidroID/water_supply_on_off"

pump = "NONE"
tlvl = "NONE"
rlvl = "NONE"
gh = "NONE"
hp = "NONE"
warn = "NONE"


file = open("tester_temp.txt")

text = ""

def show(pump, tlvl, rlvl, warn):
    print("\033c")
    tlvl = tlvl+ "%"
    rlvl = rlvl+ "%"
    print(text%(pump+" "*(32-len(pump)),\
          tlvl+" "*(32-len(tlvl)), \
          rlvl+" "*(32-len(rlvl)), \
          warn+" "*(44-len(warn))))    


for line in file:
    text = text + line

def on_disconnect(client, userdata, message):
    print("disconnected")

def on_connect(client, a, b, c):
    print("connect")

def on_message(client, userdata, message):
    global pump, tlvl, rlvl, gh, hp, warn

    data = message.payload.decode('utf-8')
    warn = ''
    try:
        if message.topic == OUT_PUMP_STATE:
            pump = data
      
        if message.topic == OUT_LVL_TANK:
            tlvl = data

        if message.topic == OUT_LVL_SOURCE:
            rlvl = data
        
        if message.topic == OUT_WARNINGS:
            warn = data
        show(pump, tlvl, rlvl, warn)
    except Exception as e:
        print(e)

print("Initiating the client")
# client Name
client = mqtt.Client("PDU_0215465554") 
client.on_message = on_message # attach function to callback
client.on_disconnect = on_disconnect
client.on_connect = on_connect
print("connecting to broker")
client.connect(broker_address, 1883, 60) #connect to broker

client.loop_start() 

client.subscribe(OUT_PUMP_STATE)
client.subscribe(OUT_LVL_TANK)
client.subscribe(OUT_LVL_SOURCE)
client.subscribe(OUT_WARNINGS)

while True:

    time.sleep(0.05)
    show(pump, tlvl, rlvl, warn)
    i = input(">")

    if i == "pump on":
        client.publish(IN_PUMP_CONTROL, "ON")
    if i == "pump off":
        client.publish(IN_PUMP_CONTROL, "OFF")

    if i == "gh on":
        client.publish(IN_GH_SUPPLY, "ON")
    if i == "gh off":
        client.publish(IN_GH_SUPPLY, "OFF")

    if i == "hydro on":
        client.publish(IN_HYDROPN_SUPPLY, "ON")
    
    if i == "hydro off":
        client.publish(IN_HYDROPN_SUPPLY, "OFF")