#!/bin/bash

allUwbFiles="3dspusbWB.desktop 
3dspusbWB.png 
AUTHORS 
ChangeLog 
INSTALL 
Makefile.in 
NEWS 
README 
config.h.in 
configure 
configure.in 
depcomp 
install-sh 
mkinstalldirs 
wbusb.conf 
po/Makefile.in.in 
po/POTFILES.in 
po/uwb.pot 
po/uwb.mo 
src/Makefile.in 
src/callbacks.c 
src/callbacks.h 
src/interface.c 
src/interface.h 
src/main.c 
src/main.h 
src/pixmap.h 
src/support.c 
src/support.h  "
#po/Makefile.in 
#src/uwb-pre 
#src/uwbdriverupdate.bin " 

allWbFiles="
3dspWB.desktop 
3dspWB.png 
ChangeLog 
INSTALL 
Makefile.in 
AUTHORS 
NEWS 
README 
config.h.in 
configure 
configure.in 
depcomp 
install-sh 
mkinstalldirs 
wb.conf 
po/Makefile.in.in 
po/POTFILES.in 
po/wb.mo 
po/wb.pot 
src/Makefile.in 
src/callbacks.c 
src/callbacks.h 
src/interface.c 
src/interface.h 
src/main.c 
src/myhead.h 
src/pixmap.h 
src/support.c 
src/support.h "
#po/Makefile.in 
#src/wb-pre 
#src/wbdriverupdate.bin "

### 根据第一个参数，设置源目录，目标目录，归档文件名 ###
if [ "$1" = "wb" ] 
then
	pName="wb"
	allFiles=${allWbFiles}
	sDir="$(pwd)/l${pName}/"
else
	pName="uwb"
	allFiles=${allUwbFiles}
	sDir="$(pwd)/${pName}/"
fi

pName="uwb"
sDir="$(pwd)/"
dDir="${TmpDir:-/tmp}/"

grep -nH 'ac_default_prefix[\t ]*=' ${sDir}configure --color=auto
grep -nH -A 8 'uninstall-local[\t ]*:' ${sDir}Makefile.in --color=auto|| isExit=1
grep -nH '@list=.$.bin_PROGRAMS' ${sDir}src/Makefile.in --color=auto
#ls -1 ${sDir}po/${pName}.mo ${sDir}src/${pName}-pre ${sDir}src/${pName}driverupdate.bin --color=auto|| isExit=1
[ "${isExit}" = '1' ] && exit 65
#exit 0

rm -rf ${dDir}${pName} ${pName}.tar.gz
#ls ${dDir}
mkdir ${dDir}${pName} 
[ -d "${dDir}${pName}/" ] && dDir="${dDir}${pName}/" || exit 1
mkdir ${dDir}src
mkdir ${dDir}po

for ff in ${allFiles}
do
	cp -f ${sDir}${ff} ${dDir}${ff} || { echo -e "\n\n\tERROR: FAIL to copy ${sDir}${ff}, exit.\n" ; exit 66 ;  }
done


cd 	${dDir}/..
tar zcf ${pName}.tar.gz ${pName}
mv -f ${pName}.tar.gz ${sDir}
cd -
#mv ${dDir}/../${pName}.tar.gz .
	
	
