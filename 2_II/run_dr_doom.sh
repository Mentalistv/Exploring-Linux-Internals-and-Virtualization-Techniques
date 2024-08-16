#!/bin/bash

## Building the applications
make 

## Build your ioctl driver and load it here
sudo /sbin/insmod ioctl_driver.ko
sudo chmod 777 /dev/ioctl_driver

###############################################

# Launching the control station
./control_station &
c_pid=$!
echo "Control station PID: $c_pid"

# Launching the soldier
./soldier $c_pid &
echo "Soldier PID: $!"

sleep 10
kill -9 $c_pid

## Remove the driver here
sudo /sbin/rmmod ioctl_driver

