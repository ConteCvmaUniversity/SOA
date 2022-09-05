#!/bin/bash

echo "Program for add a device  [SOA project]"

echo "Insert device name"
read DEVICE

echo "Insert driver Major number"
read DRIVER

echo "Insert device Minor number"
read MINOR


mknod /dev/${DEVICE:='test'} c $DRIVER $MINOR
echo /dev/${DEVICE:='test'} >> devices.txt
echo "device path /dev/${DEVICE:='test'}"