## Process this file with automake to produce Makefile.in

SUBDIRS = src po

EXTRA_DIST = \
	autogen.sh \
	wbusb.conf \
	3dspusbWB.desktop \
	3dspusbWB.png \
	README

install-data-local:
	@$(NORMAL_INSTALL)
	install -c -D -m 666 $(top_srcdir)/wbusb.conf /usr/local/3DSP/usb/wbusb.conf
	$(INSTALL_DATA) $(top_srcdir)/3dspusbWB.desktop /usr/share/applications/3dspusbWB.desktop
	$(INSTALL_DATA) $(top_srcdir)/3dspusbWB.png /usr/share/pixmaps/3dspusbWB.png
	$(INSTALL_DATA) -D $(top_srcdir)/po/uwb.mo /usr/share/locale-langpack/zh_CN/LC_MESSAGES/uwb.mo

uninstall-local:
	rm -f /usr/bin/uwb
	rm -f /usr/local/3DSP/usb/wbusb.conf
	rm -f /usr/share/applications/3dspusbWB.desktop
	rm -f /usr/share/pixmaps/3dspusbWB.png
	rm -f /usr/share/locale-langpack/zh_CN/LC_MESSAGES/uwb.mo

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

