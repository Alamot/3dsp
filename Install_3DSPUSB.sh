#!/bin/bash
#This script will complie the drivers, install the drivers and utilities for 3DSP card.
#

#Check the current user is root or not.
if [ "$(whoami)" != 'root' ]; then
    echo "Notice: Permission denied. You MUST be root user."
    exit 1
fi

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

#Complie and install the drivers
echo -n " * Compiling the drivers, it will take some minutes..."
echo""
cd driver_src
PRINTOFF
make clean
make
PRINTONLYSTD
echo -n " * Installing the drivers..."
echo""
PRINTOFF
make install
cd ..

#Install uWB and wbtool
PRINTONLYSTD
echo -n " * Installing the WB and wbtool..."
echo""
PRINTOFF
cp -f applications/bin/btusbcfg.sh     /usr/local/3DSP/usb/; chmod 444 /usr/local/3DSP/usb/btusbcfg.sh
cp -f applications/uwb/wbusb.conf      /usr/local/3DSP/usb/; chmod 666 /usr/local/3DSP/usb/wbusb.conf
cp -f applications/uwb/3dspusbWB.desktop    /usr/share/applications/3dspusbWB.desktop
cp -f applications/uwb/3dspusbWB.png        /usr/share/pixmaps/3dspusbWB.png
cp -f applications/uwb/3dspusbWB.desktop    /etc/xdg/autostart/3dspusbWB.desktop
cp -f applications/bin/3dspcode.bin /usr/local/3DSP/usb/3dspcode.bin

if [ `getconf LONG_BIT` -eq "32" ]; then
  cp -f applications/bin/uwb       /usr/bin/uwb; chmod 755 /usr/bin/uwb
  cp -f applications/bin/uwbtool   /usr/bin/uwbtool; chmod 755 /usr/bin/uwbtool
else
  cp -f applications/bin/uwb_64       /usr/bin/uwb; chmod 755 /usr/bin/uwb
  cp -f applications/bin/uwbtool_64   /usr/bin/uwbtool; chmod 755 /usr/bin/uwbtool
fi

#Install 3dsp-wifi-radar
PRINTONLYSTD
echo -n " * Installing the 3dsp-wifi-radar..."
echo""
PRINTOFF
cd applications/3dsp-wifi-radar/
make uninstall
make install
cd ../..

#Make /etc/init.d/tdspusbcardinit for auto-startup
PRINTONLYSTD
echo -n " * Creating the init scripts..."
echo""
PRINTOFF
cp -f tdspusbcardinit /etc/init.d/tdspusbcardinit
ln -sf /etc/init.d/tdspusbcardinit /etc/rc2.d/S98tdspusbcardinit
ln -sf /etc/init.d/tdspusbcardinit /etc/rc3.d/S98tdspusbcardinit
ln -sf /etc/init.d/tdspusbcardinit /etc/rc4.d/S98tdspusbcardinit
ln -sf /etc/init.d/tdspusbcardinit /etc/rc5.d/S98tdspusbcardinit

#copy the uninstall file to the DIR
cp -f Uninstall_3DSPUSB.sh  /usr/local/3DSP/usb/

#Start drivers
PRINTONLYSTD
echo -n " * Loading the modules..."
echo""
PRINTOFF
/etc/init.d/tdspusbcardinit start


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
echo "Installation completed. Startup 3DSP card through Applications->Accessories>3DSP uWB"
echo "Uninstall the drivers and utilities through \"bash /usr/local/3DSP/usb/Uninstall_3DSPUSB.sh\""
exit 0

