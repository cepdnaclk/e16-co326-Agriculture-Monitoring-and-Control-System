# CO326 project: PLC MQTT bridge

import paho.mqtt.client as mqtt 
from PLC import PLC

broker_address   = "broker.mqttdashboard.com"
OUT_LVL_TANK     = "/CO326/2021/PDU/levels/tank"
OUT_LVL_SOURCE   = "/CO326/2021/PDU/levels/source"
OUT_QLTY_PH      = "/CO326/2021/PDU/water_quality_ph"
OUT_QLTY_OPACITY = "/CO326/2021/PDU/water_quality_opacity"
OUT_WARNINGS     = "/CO326/2021/PDU/warnings"
OUT_PUMP_STATE   = "/CO326/2021/PDU/pump/state"


IN_PUMP_CONTROL   = "/CO326/2021/PDU/pump/control"
IN_GH_SUPPLY      = "/CO326/2021/GH/#GreenHouseID/water_supply_on_off"
IN_HYDROPN_SUPPLY = "/CO326/2021/GH/#HidroID/water_supply_on_off"

plc = PLC()

def on_disconnect(client, userdata, message):
    print("disconnected")

def on_message(client, userdata, message):
    
    try:
        if message.topic == IN_PUMP_CONTROL:
            if message[1] == 'ON':
                plc.control_pump(1)
            elif message[1] == 'OFF':
                plc.control_pump(0)

        if message.topic == IN_GH_SUPPLY:
            if message[1] == 'ON':
                plc.control_distrib_pump(0,1) # parameter : (pump idx, value)
            elif message[1] == 'OFF':
                plc.control_distrib_pump(0,0) # parameter : (pump idx, value)

        if message.topic == IN_HYDROPN_SUPPLY:
            if message[1] == 'ON':
                plc.control_distrib_pump(1,1) # parameter : (pump idx, value)
            elif message[1] == 'OFF':
                plc.control_distrib_pump(1,0) # parameter : (pump idx, value)
    except Exception as e:
        print(e)




print("Initiating the client")
# client Name
client = mqtt.Client("PDU_021554") 
client.on_message = on_message # attach function to callback
client.on_disconnect = on_disconnect
print("connecting to broker")
client.connect(broker_address, 8000, 60) #connect to broker

client.loop_start() 

# subscribing to the current postion topic
client.subscribe("swarm/{}/currentPos".format(SWARM_ID))
client.subscribe(TOPIC_COM)
client.subscribe(TOPIC_SEVER_COM)


client.publish(TOPIC_SEVER_BOT_POS, aesEncrypt(data),)