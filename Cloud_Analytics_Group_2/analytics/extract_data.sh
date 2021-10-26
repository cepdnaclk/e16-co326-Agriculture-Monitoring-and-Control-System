#!/bin/bash

# comments start with a '#"

while :
do
	echo "Successfully Predictions Sent"
	influx -username 'admin' -password 'password' -database PROJECT -execute 'SELECT * FROM soilPH' -format csv > test.csv
	influx -username 'admin' -password 'password' -database PROJECT -execute 'SELECT * FROM controlUnitPower' -format csv > test2.csv
	influx -username 'admin' -password 'password' -database PROJECT -execute 'SELECT * FROM waterQualityUnitPower' -format csv > test3.csv
  	sleep 60
done