#!/bin/sh
echo "-------------------------------------------------" 
echo "Setting prerequisites..."
echo "> Configuring virtual network interface"
ifconfig eno1:1 192.168.10.11 netmask 255.255.255.0
echo "> Configuring buffer"
sysctl -w net.core.rmem_max=50000000 
sysctl -w net.core.wmem_max=2500000
#echo "Discovering USRP..."
#sleep 5
#uhd_find_devices
