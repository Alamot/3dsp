#!/bin/bash
#This script will REMOVE the drivers and utilities for 3DSP card.
#

#Check the current user is root or not.
if [ "$(whoami)" != 'root' ]; then
    echo "Notice: Permission denied. You MUST be root user."
    exit 1
fi

echo -n "Remove the drivers and utilities for 3DSP card?(y/n) "
read answer
case "$answer" in
    "y" | "Y")
     ;;
    *  )
     echo "Exit."
     exit 0;;
esac

PWD=$(pwd)
LOGFILE="${PWD}/logfile"

function PRINTONLYSTD {
  exec 1>&6
  exec 2>>$LOGFILE
}
function PRINTOFF {
  exec 2>>$LOGFILE
  exec 1>>$LOGFILE
}

exec 6>&1
exec 7>&2

#Remove /etc/init.d/tdspusbdriver and ln for auto-startup
echo -n " * Removing the init scripts..."
echo ""

rm -f /etc/rc2.d/S98tdspusbcardinit
rm -f /etc/rc3.d/S98tdspusbcardinit
rm -f /etc/rc4.d/S98tdspusbcardinit
rm -f /etc/rc5.d/S98tdspusbcardinit
#rm -f /etc/init.d/tdspusbcardinit

#Remove 3dsp-wifi-radar
echo -n " * Removing the 3dsp-wifi-radar..."
echo ""
PRINTOFF
killall -q 3dsp-wifi-radar; sleep 1

[ -d /usr/local/3DSP/pci ] || {
	make -f /etc/wifi-radar/Makefile uninstall
	rm  -rf /etc/wifi-radar
}


#Remove utilities
PRINTONLYSTD
echo -n " * Removing the WB and wbtool..."
echo ""
PRINTOFF
killall -q uwb

uwbtool -r; sleep 1

rm -f /usr/share/applications/3dspusbWB.desktop
rm -f /usr/share/pixmaps/3dspusbWB.png
rm -f /etc/xdg/autostart/3dspusbWB.desktop
rm -f /usr/bin/uwb
rm -f /usr/bin/uwbtool


#Remove the DIR
if [ -d /usr/local/3DSP/pci ];then
  rm -rf /usr/local/3DSP/usb
else
  rm -rf /usr/local/3DSP
fi

#Unload modules for 3DSP card.
PRINTONLYSTD
echo -n " * Unloading the modules..."
echo ""
PRINTOFF
/etc/init.d/tdspusbcardinit stop
rm -f /etc/init.d/tdspusbcardinit


[ `lsb_release -i|awk -F'\t' '{print $2}'|tr  'A-Z' 'a-z'` != "ubuntu" ] && exit 0
[ `lsb_release -r|awk '{print $2}'` != "10.04" ] && exit 0


if ! [ -w '/usr/share/applications/' ] 
then
	echo "Permission denied"
	exit 1
fi


Binfile="/var/lib/dpkg/info/python-gmenu.postinst"
if [ -f "${Binfile}" ] 
then
	echo "update desktop-file cache"
	${Binfile} triggered 
else
	echo "get rid of desktop-file cache"
	eval `locale|grep LC_MESSAGES |sed 's/\.UTF-8/\.utf8/'`
	rm -f /usr/share/applications/desktop.$LC_MESSAGES.cache
fi

PRINTONLYSTD
echo "Uninstallation completed. Exit."
exit 0
