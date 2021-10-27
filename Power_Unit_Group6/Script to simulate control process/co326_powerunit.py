import paho.mqtt.client as mqtt
import json
import time
import random

# Define Variables
MQTT_HOST = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_KEEPALIVE_INTERVAL = 45

#Topics to get data from simulation nodes
MQTT_TOPIC_1 = "/CO326/2021/GH1"
MQTT_TOPIC_2 = "/CO326/2021/GH2"
MQTT_TOPIC_3 = "/CO326/2021/GH3"
MQTT_TOPIC_4 = "/CO326/2021/GH4"
MQTT_TOPIC_5 = "/CO326/2021/POWERUNIT"

#Topics to publish data to main scada
MQTT_TOPIC_6 = "/CO326/2021/PWRUNIT/greenhouse1"
MQTT_TOPIC_7 = "/CO326/2021/PWRUNIT/greenhouse2"
MQTT_TOPIC_8 = "/CO326/2021/PWRUNIT/greenhouse3"
MQTT_TOPIC_9 = "/CO326/2021/PWRUNIT/greenhouse4"
MQTT_TOPIC_10 = "/CO326/2021/PWRUNIT/solar_grid"
# MQTT_TOPIC_11 = "/CO326/2021/PWRUNIT/grid"

#Green house states are stored in this global dictionaries
greenHouse1 = dict()
greenHouse2 = dict()
greenHouse3 = dict()
greenHouse4 = dict()
solar_grid  = dict()

greenHouse1['state'] = 1
greenHouse2['state'] = 1
greenHouse3['state'] = 1
greenHouse4['state'] = 1

greenHouse1['TP'] = 0
greenHouse2['TP'] = 0
greenHouse3['TP'] = 0
greenHouse4['TP'] = 0

greenHouse1['noness_power'] = 0
greenHouse2['noness_power'] = 0
greenHouse3['noness_power'] = 0
greenHouse4['noness_power'] = 0

greenHouse1['ess_power'] = 0
greenHouse2['ess_power'] = 0
greenHouse3['ess_power'] = 0
greenHouse4['ess_power'] = 0

solar_grid['is_solar'] = 1

ALL_greenhouses = (greenHouse1,greenHouse2,greenHouse3,greenHouse4)

# Define functions

#to update green house states using received values from simulation scada
def setGreenHouse(payload):
    print('setGreenHouse function invoked')
    id = payload["ID"]

    if(id == 1 and greenHouse1['state'] == 1):
        greenHouse1['ess_current'] = payload['essentialcurrent']
        greenHouse1['noness_current'] = payload['normalcurrent']
        greenHouse1['ess_voltage'] = payload['essentialvoltage']
        greenHouse1['noness_voltage'] = payload['normalvoltage']
        greenHouse1["ess_power"] = payload['essentialcurrent']*payload['essentialvoltage']
        greenHouse1["noness_power"] = payload['normalcurrent']*payload['normalvoltage']
        greenHouse1['TP'] = greenHouse1['noness_power'] + greenHouse1['ess_power']

    elif(id == 2 and greenHouse2['state'] == 1):
        greenHouse2['ess_current'] = payload['essentialcurrent']
        greenHouse2['noness_current'] = payload['normalcurrent']
        greenHouse2['ess_voltage'] = payload['essentialvoltage']
        greenHouse2['noness_voltage'] = payload['normalvoltage']
        greenHouse2["ess_power"] = payload['essentialcurrent']*payload['essentialvoltage']
        greenHouse2["noness_power"] = payload['normalcurrent']*payload['normalvoltage']
        greenHouse2['TP'] = greenHouse2['noness_power'] + greenHouse2['ess_power']

    elif(id == 3 and greenHouse3['state'] == 1):
        greenHouse3['ess_current'] = payload['essentialcurrent']
        greenHouse3['noness_current'] = payload['normalcurrent']
        greenHouse3['ess_voltage'] = payload['essentialvoltage']
        greenHouse3['noness_voltage'] = payload['normalvoltage']
        greenHouse3["ess_power"] = payload['essentialcurrent']*payload['essentialvoltage']
        greenHouse3["noness_power"] = payload['normalcurrent']*payload['normalvoltage']
        greenHouse3['TP'] = greenHouse3['noness_power'] + greenHouse3['ess_power']

    elif(id == 4 and greenHouse4['state'] == 1):
        greenHouse4['ess_current'] = payload['essentialcurrent']
        greenHouse4['noness_current'] = payload['normalcurrent']
        greenHouse4['ess_voltage'] = payload['essentialvoltage']
        greenHouse4['noness_voltage'] = payload['normalvoltage']
        greenHouse4["ess_power"] = payload['essentialcurrent']*payload['essentialvoltage']
        greenHouse4["noness_power"] = payload['normalcurrent']*payload['normalvoltage']
        greenHouse4['TP'] = greenHouse4['noness_power'] + greenHouse4['ess_power']

#function to update data get from power sources from simulation scada
def powersource(payload):
    print('powersource function invoked')
    solar_grid['solar_current'] = payload['solar_current']
    solar_grid['solar_voltage'] = payload['solar_voltage']
    solar_grid['solar_power']   = payload['solar_voltage']*payload['solar_current']

    solar_grid['is_grid']   = payload['is_grid']
    solar_grid['grid_current'] = payload['grid_current']
    solar_grid['grid_voltage'] = payload['grid_voltage']
    solar_grid['grid_power']   = payload['grid_voltage']*payload['grid_current']
 
#total power demand calculation from four green houses
def totalPower():
    return (greenHouse1['noness_power'] + greenHouse1['ess_power'] + greenHouse2['noness_power'] + greenHouse2['ess_power'] + greenHouse3['noness_power'] + greenHouse3['ess_power'] + greenHouse4['noness_power'] + greenHouse4['ess_power'])


#function which decides which to cutoff to match demand to supply
def cutoff():
    print('cutoff function invoked')
    demand = totalPower()
    print("demand ")
    print(demand)
    print("solar supply upper threshold")
    print(solar_grid['solar_power'] * 0.95)
    print("solar supply lower threshold")
    print(solar_grid['solar_power'] * 0.75)

    if(solar_grid['is_grid'] == 0 and demand > (solar_grid['solar_power'] * 0.95)):  #when thers grid offline and solar is also not able to give full supply we try to shut someone down
        print("option1")
        solar_grid['is_solar'] = 1                #turn over the control solar
        max_pwr = 0
        greenhouse = {}
        counter = 0
        for GH in ALL_greenhouses:
            if(GH['noness_power'] > max_pwr and GH['state'] == 1):
                max_pwr = GH['noness_power']
                greenhouse = GH
                counter = counter+1
        if counter == 0:        # this means all the non essentials have been shut down but still solar is not able to give enough power
                                # so power loss indication sent to local scada
            solar_grid['is_solar'] = 0       
            return                               ######### 
        greenhouse['state'] = 0
        greenhouse['noness_current']   = 0
        greenhouse['noness_voltage']   = 0
        greenhouse['noness_power']   = 0

        print('cutoff greenhouse ')
        print(greenhouse)
        demand = totalPower()
        cutoff()
    elif(solar_grid['is_grid'] == 0 and demand < (solar_grid['solar_power'] * 0.75)):   #when thers grid offline and solar_grid regains enough energey to powerup turned off green house
        print("option2")
        solar_grid['is_solar'] = 1                #turn over the control solar
        min_pwr = 65000
        greenhouse = {}
        for GH in ALL_greenhouses:
            if(GH['noness_power'] < min_pwr and GH['state'] == 0):
                min_pwr = GH['noness_power']
                greenhouse = GH
        greenhouse['state'] = 1
        print('turned on greenhouse ')
        print(greenhouse)
        demand = totalPower()
    elif(solar_grid['is_grid'] == 1 and demand < (solar_grid['solar_power'] * 0.75)): #when thers grid and solar both and solar can give total supply
        print("option3")
        solar_grid['is_solar'] = 1
        greenHouse1['state'] = 1
        greenHouse2['state'] = 1
        greenHouse3['state'] = 1
        greenHouse4['state'] = 1
    elif(solar_grid['is_grid'] == 1 and demand > (solar_grid['solar_power'] * 0.95)):  #when thers grid and solar is not enough to give full supply
        print("option4")
        solar_grid['is_solar'] = 0
        greenHouse1['state'] = 1
        greenHouse2['state'] = 1
        greenHouse3['state'] = 1
        greenHouse4['state'] = 1




#MQTT communication
while True:
    # Define on_publish event function
    def on_publish(client, userdata, mid):
        print("Message Published...")


    def on_connect(client, userdata, flags, rc):
        client.subscribe(MQTT_TOPIC_1)
        client.subscribe(MQTT_TOPIC_2)
        client.subscribe(MQTT_TOPIC_3)
        client.subscribe(MQTT_TOPIC_4)
        client.subscribe(MQTT_TOPIC_5)

        send_msg_GH1 = json.dumps(greenHouse1)
        send_msg_GH2 = json.dumps(greenHouse2)
        send_msg_GH3 = json.dumps(greenHouse3)
        send_msg_GH4 = json.dumps(greenHouse4)
        send_msg_solar_grid = json.dumps(solar_grid)
        # send_msg_grid = json.dumps(grid)

        client.publish(MQTT_TOPIC_6, send_msg_GH1)
        client.publish(MQTT_TOPIC_7, send_msg_GH2)
        client.publish(MQTT_TOPIC_8, send_msg_GH3)
        client.publish(MQTT_TOPIC_9, send_msg_GH4)
        client.publish(MQTT_TOPIC_10, send_msg_solar_grid)
        # client.publish(MQTT_TOPIC_11, send_msg_grid)

    def on_message(client, userdata, msg):
        print(msg.topic)
        payload = json.loads(msg.payload) # you can use json.loads to convert string to json
        print(payload) # then you can check the value
        client.disconnect() # Got message then disconnect

        if(msg.topic == "/CO326/2021/POWERUNIT"):
            powersource(payload)
        else:
            setGreenHouse(payload)
            cutoff()

        

    # Initiate MQTT Client
    mqttc = mqtt.Client()

    # Register publish callback function
    mqttc.on_publish = on_publish
    mqttc.on_connect = on_connect
    mqttc.on_message = on_message

    # Connect with MQTT Broker
    mqttc.connect(MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE_INTERVAL)

    # time.sleep(5)

    # Loop forever
    mqttc.loop_forever()