## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = uwb

uwb_SOURCES = \
	main.c main.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	pixmap.h

uwb_LDADD = @PACKAGE_LIBS@ -lnotify $(INTLLIBS)
DEFS+=$(RITOWFLAGS)
