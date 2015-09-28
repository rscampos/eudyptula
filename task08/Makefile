ifneq ($(KERNELRELEASE),)
	obj-m  := debugfs.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

endif
