##########################################
#
# 3DSP USB card Linux driver Makefile
#
##########################################

export DEFS

ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS := $(DEFS) -Wno-unused-variable -Wno-parentheses -Wno-format -Wnonnull -Wstrict-prototypes
EXTRA_CFLAGS += -I$(src)/../private/new_bluetooth_priv/
EXTRA_CFLAGS += -g
#LD += -S

OBJS := tdsp_memory.o tdsp_event.o tdsp_mutex.o \
        tdsp_timer.o tdsp_tasklet.o tdsp_usb.o  \
        tdsp_work.o tdsp_skb.o tdsp_file.o      \
        tdsp_bus.o lbluetooth_entry.o


obj-m := 3dspusbbt.o
3dspusbbt-y	:=  $(OBJS)

else
	KDIR ?= /lib/modules/$(shell uname -r)/build/
#	KDIR ?= /usr/src/kernels/linux9260
all:
	@echo DEFS=$(DEFS)
	$(MAKE) -C $(KDIR) M=`pwd` modules
endif

clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
