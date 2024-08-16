#!/bin/bash

make

# Load kernel module
sudo /sbin/insmod driver.ko

# Run user space program
sudo ./test

# Unload kernel module
sudo /sbin/rmmod driver

make clean
