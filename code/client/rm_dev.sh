#!/bin/bash


file=$(cat devices.txt)
DEVICE="initial"

echo "Program for remove a device  [SOA project]"

while getopts 'a' flag
do
    case "${flag}" in
        a) DEVICE="all";;
        ?) echo "script usage: $(basename $0) [-a]" >&2
      	   exit 1 ;;
    esac
done

shift "$(($OPTIND -1))"

echo $DEVICE
if [ ${DEVICE} != 'all' ]; then 

    echo "Insert device name"
    read DEVICE

    if grep -Fxq "/dev/$DEVICE" devices.txt
    then
        rm /dev/$DEVICE
        echo "Removed $DEVICE"
    else
        echo "file not found"
    fi
else
    for line in $file
    do
        rm $line
        rm devices.txt
    echo "Removed $DEVICE"
    done
fi