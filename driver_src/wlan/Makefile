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
#CC = arm-linux-gcc
#LD = arm-linux-ld
#CC = mipsel-linux-gcc
#LD = mipsel-linux-ld
#LDFLAGS := test

obj-m := 3dspusbwlan.o 

3dspusbwlan-objs := tdsp_memory.o\
								tdsp_event.o\
								tdsp_mutex.o\
								tdsp_timer.o\
								tdsp_tasklet.o\
								tdsp_usb.o\
								tdsp_bus.o\
								tdsp_file.o\
								lwlan_ioctl.o\
								lwlan_netdev.o\
								lwlan_entry.o


PWD := $(shell pwd)
#KDIR := /usr/local/src/linux-crater_1-0-3/kernel/linux-2.6.20.4/
#KDIR := /usr/local/src/linux-2.6.27-lemote-1_pre4/
KDIR ?= /lib/modules/$(shell uname -r)/build 
#include $(KDIR)/.config

all:
	cp ../bus/Module.symvers .
	cp ../private/wlan_priv/Module.symvers .
	$(MAKE) -C $(KDIR) M=$(PWD) modules
        
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean


