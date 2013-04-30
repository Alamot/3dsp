

TARGET_MATCHED = false


ifneq ($(KERNELRELEASE),)
obj-m := 3dspusbbus.o
3dspusbbus-objs	:= businit.o	\
	    busioctl.o		

else
	KDIR ?= /lib/modules/$(shell uname -r)/build/
	PWD  := $(shell pwd)

all:
	@echo "This driver support:"
	$(MAKE) -w -C $(KDIR) SUBDIRS=$(PWD) modules
endif


clean:
	-rm -f tags
	-rm -f *.o *.ko
	-rm -f *.order *.symvers
	-rm -f .*.cmd *.mod.c
	-rm -rf .tmp_versions
	-rm -rf *~

