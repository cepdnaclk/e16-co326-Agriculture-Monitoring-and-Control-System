# Smart Irrigation System for Open Field Cultivations

- Initially, the Soil moisture-controlling unit identifies the moisture level has decreased. 
- Then it signals the irrigation valve controlling unit to turn the valve on. 
- After that the corresponding MQTT messages will be published to the MQTT broker. 
- Then an MQTT subscriber which is the pump control unit sends a signal to turn the pressure pump on. 
- Soon after that, the pump is powered up. 
- Eventually, when the moisture level reaches the desired level, the soil moisture control unit again signals to the valve controlling unit. 
- Then it signals back to the MQTT broker. 
- Then the pump control unit receives the signal to turn the pump off. 
- Finally the irrigation valve controlling unit turn off the valve and eventually the water supply stops automatically. 
