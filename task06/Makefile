ifneq ($(KERNELRELEASE),)
	obj-m  := misc_char.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

endif