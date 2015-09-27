ifneq ($(KERNELRELEASE),)
	obj-m  := task01.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

endif