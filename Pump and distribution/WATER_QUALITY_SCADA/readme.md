### This is the water quality dashbord and mqtt connection implementation


##### Contribution

* Thushara K A R :: E/16/369 :: [e16369@eng.pdn.ac.lk](e16369@eng.pdn.ac.lk)
* Github :: [thusharakart](https://github.com/thusharakart)


#### How to configure

    > First you need install node-red in your local mechine.

    > Then, run node-red and go to http://127.0.0.1:1880/ 

    > Then click on Water Quality Dashbord tab

    > Then click the Deploy button on top right corner

    > You will see the dashbord on http://127.0.0.1:1880/ui/



#### Check the mqtt connection

    > install mqtt in your machine

    > run the publisher.py file

    > you will see the real time dashboard outputs



#### How to subscribe

    > start a new command window

    > type the command : mosquitto_sub -h "broker.hivemq.com" -t "water_quality/sensor/#"

    > change the status of the controlls in the dashbord 

    > controll outputs will be received from the subscriber window


##### Water Quality Dashboard

![dashboard](https://github.com/thusharakart/e16-co326-Agriculture-Monitoring-and-Control-System/blob/main/Pump%20and%20distribution/WATER_QUALITY_SCADA/images/water_quality%20controll%20dashboard.PNG?raw=true)


##### Configuration and debugging in node-red

![node-red](https://github.com/thusharakart/e16-co326-Agriculture-Monitoring-and-Control-System/blob/main/Pump%20and%20distribution/WATER_QUALITY_SCADA/images/node%20red%20configuration%20and%20debuging.PNG?raw=true)


##### Control outputs for subscribers

![mqtt](https://github.com/thusharakart/e16-co326-Agriculture-Monitoring-and-Control-System/blob/main/Pump%20and%20distribution/WATER_QUALITY_SCADA/images/controll%20outputs%20for%20subscribers.PNG?raw=true)

