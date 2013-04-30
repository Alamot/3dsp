#/*
# * Makefile:
# * 	Copyright (c) 2007 3DSP Corporation.
# *		All rights Reserved.
# *
# * created:
# * 	24/10/2007				Initial Coding
# *
# * modified:
# *
# * description:
# * 	This is the final Makefile which, contains all the libs used
# *		in the stack, links all the libs and create the output file. 
# *
# * exports:
# * 	make_config
# *
# * make'ing:
# *
# * !! Warning !!
# * 	No extraneoius space allowed 
# *
# * TODO:
# * 
# * see also:
# *
# * This source to be viewed with tabspace 4 (":se ts=4" for ex/vi)
# *
# */
EXTRA_CFLAGS = -fno-common


ifeq ($(shell getconf LONG_BIT),32)
TdspUsbWlanLib=3dspusbwlanlib.a
else
TdspUsbWlanLib=3dspusbwlanlib64.a
endif

obj-m := 3dspusbwlanpriv.o 

3dspusbwlanpriv-objs := usbwlan_entry.o\
			$(TdspUsbWlanLib)


PWD := $(shell pwd)
KDIR ?= /lib/modules/$(shell uname -r)/build 

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
        
clean:
	-rm -f $(PWD)/*.o	
	-rm -f $(PWD)/*.ko
	-rm -f $(PWD)/*.order
	-rm -f $(PWD)/*.symvers
	-rm -f $(PWD)/*.mod.c

