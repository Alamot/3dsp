#!/bin/bash
#
# This shell script is used to configure some BlueZ services.
#
# 3DSP corporation
# 2008.11.18
#

echo "Start rfcomm/pand server......"
VAR1=`ps ax | grep -w rfcomm | grep -v grep | wc -l`
if [ ${VAR1} -eq 0 ]; then
    rfcomm -i hci0 watch rfcomm1 1 &
fi

VAR2=`ps ax | grep -w pand | grep -v grep | wc -l`
if [ ${VAR2} -eq 0 ]; then
    IFROOT=TRUE
    if [ "$(whoami)" != 'root' ]; then
        IFROOT=FALSE
    fi
    
    #pand --device hci0 --master --role GN --listen
    #pand --device hci0 --listen --role PANU
    if [ "$1" = "" -o "$1" = "0" ]; then
        if [ ${IFROOT} = "FALSE" ]; then
            #gnome-terminal -e "sudo pand --device hci0 --master --role NAP --listen"
            sleep 5
            gksudo -k -m "3DSP WB is attempting to perform an action that requires privileges. Authentication as [${USER}] is required." "pand --device hci0 --master --role NAP --listen"
        else
            sudo pand --device hci0 --master --role NAP --listen
        fi
    else
        sudo pand --device hci0 --master --role NAP --listen
    fi
fi


echo "Add SDP services......"
SERVICENAME=("Serial Port")
SERVICE=(SP)
for((i=0;i<${#SERVICENAME[@]};i++))
do
    VAR=`sdptool browse local | grep -i "${SERVICENAME[$i]}" | wc -l`
    if [ ${VAR} -eq 0 ]; then
        sdptool add ${SERVICE[$i]}
    fi
done


if [ ! -e /root/.asoundrc ]; then
    echo "make file .asoundrc in root directory"
    echo "pcm.bluetooth{" >> /root/.asoundrc
    echo "    type bluetooth" >> /root/.asoundrc
    echo "    #device 00:11:22:33:44:55 #the headset bd_addr which you want to connect" >> /root/.asoundrc
    echo "    profile \"auto\"" >> /root/.asoundrc
    echo "}" >> /root/.asoundrc
fi
if [ ! -e ~/.asoundrc ]; then
    echo "make file .asoundrc file in user home directory"
    echo "pcm.bluetooth{" >> ~/.asoundrc
    echo "    type bluetooth" >> ~/.asoundrc
    echo "    #device 00:11:22:33:44:55 #the headset bd_addr which you want to connect" >> ~/.asoundrc
    echo "    profile \"auto\"" >> ~/.asoundrc
    echo "}" >> ~/.asoundrc
fi
