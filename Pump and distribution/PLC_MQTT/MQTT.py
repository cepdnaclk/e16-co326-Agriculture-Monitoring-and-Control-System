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
plc = PLC()
plc.debug = True
def on_disconnect(client, userdata, message):
    print("disconnected")

def on_connect(client, a, b, c):
    print("connect")

def on_message(client, userdata, message):
    global pump, tlvl, rlvl, gh, hp, warn

    data = message.payload.decode('utf-8')
    warn = ''
    try:
        if message.topic == IN_PUMP_CONTROL:

            if rlvl < 10:
                warn = "Resevoir LOW !!"
                client.publish(OUT_WARNINGS, warn)
                return

            if tlvl > 95:
                warn = "Tank is full!"
                client.publish(OUT_WARNINGS, warn)
                return
            

            if data == 'ON':
                plc.control_pump(1)
                pump = 'ON'
            elif data == 'OFF':
                plc.control_pump(0)
                pump = 'OFF'
      
        if message.topic == IN_GH_SUPPLY:
            if tlvl < 10:
                warn = "Tank is LOW !!"
                client.publish(OUT_WARNINGS, warn)
                return

            if data == 'ON':
                gh = "ON"
                plc.control_distrib_pump(0,1) # parameter : (pump idx, value)
            elif data == 'OFF':
                gh = "OFF"
                plc.control_distrib_pump(0,0) # parameter : (pump idx, value)
            show(pump, tlvl, rlvl, gh, hp, warn)

        if message.topic == IN_HYDROPN_SUPPLY:
            if tlvl < 10:
                warn = "Tank is LOW !!"
                client.publish(OUT_WARNINGS, warn)
                return

            if data == 'ON':
                hp = "ON"
                plc.control_distrib_pump(1,1) # parameter : (pump idx, value)
            elif data == 'OFF':
                hp = "OFF"
                plc.control_distrib_pump(1,0) # parameter : (pump idx, value)
            show(pump, tlvl, rlvl, gh, hp, warn)

    except Exception as e:
        print(e)

show(pump, tlvl, rlvl, gh, hp, warn)
print("Initiating the client")
# client Name
client = mqtt.Client("PDU_021554") 
client.on_message = on_message # attach function to callback
client.on_disconnect = on_disconnect
client.on_connect = on_connect
print("connecting to broker")
client.connect(broker_address, 1883, 60) #connect to broker

client.loop_start() 

client.subscribe(IN_GH_SUPPLY)
client.subscribe(IN_HYDROPN_SUPPLY)
client.subscribe(IN_PUMP_CONTROL)

rlvl = 40
tlvl = 100

# client.publish(TOPIC_SEVER_BOT_POS, aesEncrypt(data),)
p_pump = "NONE"
p_tlvl = 0
p_rlvl = 0

while True:
    tlvl = plc.get_tank_lvl()
    rlvl = plc.get_resv_lvl()
    states = plc.get_pumps()

    if tlvl != p_tlvl:
        client.publish(OUT_LVL_TANK, tlvl)
        p_tlvl = tlvl

    if rlvl != p_rlvl:
        client.publish(OUT_LVL_SOURCE, rlvl)
        p_rlvl = rlvl
    
    if p_pump != states[0]:
        client.publish(OUT_PUMP_STATE, states[0])
        p_pump = states[0]
        
    pump = states[0]
    gh = states[1]
    hp = states[2]
    show(pump, tlvl, rlvl, gh, hp, warn)
    time.sleep(0.05)