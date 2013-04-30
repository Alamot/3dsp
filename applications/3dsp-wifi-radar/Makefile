#
# $Id: Makefile,v 1.4 2009/11/20 05:27:34 hank.wang Exp $
#  vi:set filetype=make:
#

SHELL = /bin/sh

#
# These settings are what I would expect for most modern Linux distros, 
# and are what work for me unmodified on Ubuntu. -BEF-
# 
appname		= 3dsp-wifi-radar
package 	= wifi-radar
sbindir 	= /usr/sbin
initdir 	= /etc/init.d
sysconfdir 	= /etc/wifi-radar
docdir		= /usr/share/doc/${appname}
pixmapsdir 	= /usr/share/pixmaps
appsdir 	= /usr/share/applications
pamdir		= /etc/pam.d
consec_dir 	= /etc/security/console.apps

srcdir = .
top_srcdir = .
top_builddir = .

VERSION = $(shell cat VERSION)

TOPDIR := $(CURDIR)


.PHONY: all
all:	wifi-radar.localized

.PHONY: install
install:	all
	test -d ${sysconfdir} || install -d -m 755 ${sysconfdir}
	test -e ${sysconfdir}/wifi-radar.conf || install -m 644 wifi-radar.conf	${sysconfdir}
	test -e ${sysconfdir}/Makefile || install -m 644 Makefile	${sysconfdir}
	
	test -d ${sbindir} || install -d -m 755 ${sbindir}
	install -m 755 wifi-radar.localized	${sbindir}/${appname}
	rm wifi-radar.localized
	
	test -d ${docdir} || install -d -m 755 ${docdir}
	install -m 644 doc/${appname}.html 		${docdir}
	
	test -d ${docdir}/${appname}.files || install -d -m 755 ${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image001.jpg 		${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image002.gif 		${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image003.gif 		${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image004.jpg 		${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image005.jpg 		${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image006.jpg 		${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image007.jpg 		${docdir}/${appname}.files
	install -m 644 doc/${appname}.files/image008.jpg 		${docdir}/${appname}.files
	
	test -d ${initdir} || install -d -m 755 ${initdir}
	install -m 755 init_script 		${initdir}/wifi-radar
	
	test -d ${pixmapsdir} || install -d -m 755 ${pixmapsdir}
	install -m 644 pixmaps/wifi-radar.svg	${pixmapsdir}
	install -m 644 pixmaps/wifi-radar.png	${pixmapsdir}
	install -m 644 pixmaps/wifi_radar_32x32.png	${pixmapsdir}
	install -m 644 pixmaps/connected1.png	${pixmapsdir}
	install -m 644 pixmaps/connected2.png	${pixmapsdir}
	install -m 644 pixmaps/connected3.png	${pixmapsdir}
	install -m 644 pixmaps/connected4.png	${pixmapsdir}
	
	test -d ${appsdir} || install -d -m 755 ${appsdir}
	install -m 644 wifi-desktop.tmp		${appsdir}/wifi-radar.desktop
	rm -f wifi-desktop.tmp
	
	test -e /sbin/wpa_supplicant || test -d $(pamdir) || install -d -m 755 $(pamdir)
	test -e /sbin/wpa_supplicant || install -m 644 wifi-radar-pam.tp2 $(pamdir)/3dsp-wifi-radar-pam
	test ! -e wifi-radar-pam.tp2 || rm wifi-radar-pam.tp2
	
	test -e /sbin/wpa_supplicant || test -d $(consec_dir) || install -d -m 755 $(consec_dir)
	test -e /sbin/wpa_supplicant || install -m 644 wifi-radar-pam.tmp $(consec_dir)/3dsp-wifi-radar-pam
	test ! -e wifi-radar-pam.tmp || rm wifi-radar-pam.tmp

	test -e /sbin/iwconfig || test ! -d packages || test ! -e packages/libiw28_28-1+etchnhalf.1_i386.deb || dpkg -i -GE packages/libiw28_28-1+etchnhalf.1_i386.deb
	test -e /sbin/iwconfig || test ! -d packages || test ! -e packages/wireless-tools_28-1+etchnhalf.1_i386.deb || dpkg -i -GE packages/wireless-tools_28-1+etchnhalf.1_i386.deb
	test -e /usr/bin/python || test ! -d packages || test ! -e packages/python-notify_0.1.1-2+b1_i386.deb || dpkg -i -GE packages/python-notify_0.1.1-2+b1_i386.deb

wifi-radar.localized:	wifi-radar
	cp wifi-radar wifi-radar.tmp
	perl -pi -e 's#^WIFI_RADAR_VERSION\s+=.*#WIFI_RADAR_VERSION = "${VERSION}"#' wifi-radar.tmp
	perl -pi -e 's#^CONF_FILE\s+=.*#CONF_FILE = "${sysconfdir}/wifi-radar.conf"#' wifi-radar.tmp
	test -e /sbin/wpa_supplicant || perl -pi -e 's#^WPA_SUPPLICANT_COMMAND\s+=.*#WPA_SUPPLICANT_COMMAND = "/usr/sbin/wpa_supplicant"#' wifi-radar.tmp
	mv wifi-radar.tmp wifi-radar.localized
	
	cp wifi-radar.desktop wifi-desktop.tmp
	test ! -e /usr/bin/su-to-root || test ! -e /usr/bin/gksudo || perl -pi -e 's|^Exec=.*|Exec=su-to-root -X -c /usr/sbin/3dsp-wifi-radar|' wifi-desktop.tmp
	test -e /usr/bin/su-to-root || test -e /usr/bin/gksudo || perl -pi -e 's|^Exec=.*|Exec=3dsp-wifi-radar-pam|' wifi-desktop.tmp
	ln -sf /usr/bin/consolehelper /usr/sbin/3dsp-wifi-radar-pam
	
	echo USER=root > wifi-radar-pam.tmp
	echo PROGRAM=/usr/sbin/3dsp-wifi-radar >> wifi-radar-pam.tmp
	echo SESSION=true >> wifi-radar-pam.tmp
	
	echo "#%PAM-1.0" > wifi-radar-pam.tp2
	echo "auth       sufficient   pam_rootok.so" >> wifi-radar-pam.tp2
	echo "auth       sufficient   pam_timestamp.so" >> wifi-radar-pam.tp2
	echo "#auth       required     pam_stack.so service=system-auth" >> wifi-radar-pam.tp2
	echo "auth       required     pam_console.so" >> wifi-radar-pam.tp2
	echo "session    required     pam_permit.so" >> wifi-radar-pam.tp2
	echo "session    optional     pam_xauth.so" >> wifi-radar-pam.tp2
	echo "session    optional     pam_timestamp.so" >> wifi-radar-pam.tp2
	echo "account    required     pam_permit.so" >> wifi-radar-pam.tp2

.PHONY: tarball
tarball: $(TOPDIR)/tmp/${package}-$(VERSION).tar.bz2.sign
$(TOPDIR)/tmp/${package}-$(VERSION).tar.bz2.sign: $(TOPDIR)/tmp/${package}-$(VERSION).tar.bz2
	cd $(TOPDIR)/tmp && gpg --detach-sign -a --output ${package}-$(VERSION).tar.bz2.sign ${package}-$(VERSION).tar.bz2
	cd $(TOPDIR)/tmp && gpg --verify ${package}-$(VERSION).tar.bz2.sign

$(TOPDIR)/tmp/${package}-$(VERSION).tar.bz2:
	@echo
	@echo "I will now do an 'svn up' to make sure we're packaging the most current"
	@echo "version of this code."
	@echo
	@echo 'Hit <ctrl>+<c> now to cancel!!!  or hit <Enter> to proceed.'
	@read i
	svn up
	rm -fr $(TOPDIR)/tmp
	mkdir -p $(TOPDIR)/tmp/
	svn export . $(TOPDIR)/tmp/${package}-$(VERSION)
	svn log > $(TOPDIR)/tmp/${package}-$(VERSION)/CHANGE.LOG
	find $(TOPDIR)/tmp/${package}-$(VERSION) -type f -exec chmod ug+r  {} \;
	find $(TOPDIR)/tmp/${package}-$(VERSION) -type d -exec chmod ug+rx {} \;
	chmod 755 $(TOPDIR)/tmp/${package}-$(VERSION)/wifi-radar
	cd $(TOPDIR)/tmp && tar -ch ${package}-$(VERSION) | bzip2 > ${package}-$(VERSION).tar.bz2
	ls -l $(TOPDIR)/tmp/

.PHONY: clean
clean:
	test ! -e wifi-radar.localized || rm -f  wifi-radar.localized
	rm -fr tmp/

.PHONY: distclean
distclean: clean

uninstall:
	test ! -d ${docdir} || rm -rf ${docdir}
	
	test ! -e ${initdir}/wifi-radar || rm ${initdir}/wifi-radar
	
	test ! -e ${pixmapsdir}/wifi-radar.svg || rm ${pixmapsdir}/wifi-radar.svg	
	test ! -e ${pixmapsdir}/wifi-radar.png || rm ${pixmapsdir}/wifi-radar.png
	test ! -e ${pixmapsdir}/wifi_radar_32x32.png || rm ${pixmapsdir}/wifi_radar_32x32.png
	test ! -e ${pixmapsdir}/connected1.png || rm ${pixmapsdir}/connected1.png
	test ! -e ${pixmapsdir}/connected2.png || rm ${pixmapsdir}/connected2.png
	test ! -e ${pixmapsdir}/connected3.png || rm ${pixmapsdir}/connected3.png
	test ! -e ${pixmapsdir}/connected4.png || rm ${pixmapsdir}/connected4.png
	
	test ! -e ${appsdir}/wifi-radar.desktop || rm ${appsdir}/wifi-radar.desktop
	test ! -d ${consec_dir} || test ! -e ${consec_dir}/3dsp-wifi-radar-pam || rm ${consec_dir}/3dsp-wifi-radar-pam
	test ! -d ${pamdir} || test ! -e ${pamdir}/3dsp-wifi-radar-pam || rm ${pamdir}/3dsp-wifi-radar-pam
	
	test ! -e ${sbindir}/${appname} || rm ${sbindir}/${appname}
	test ! -e ${sysconfdir}/wifi-radar.conf || rm ${sysconfdir}/wifi-radar.conf
	test ! -e ${sysconfdir}/Makefile || rm ${sysconfdir}/Makefile
