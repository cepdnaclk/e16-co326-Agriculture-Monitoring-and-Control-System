#!/bin/bash

# comments start with a '#"

while :
do
	echo "Successfully Predictions Sent"
	influx -username 'admin' -password 'password' -database PROJECT -execute 'SELECT * FROM soilPH' -format csv > test.csv
  	sleep 60
done