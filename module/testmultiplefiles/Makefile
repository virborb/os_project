#  Makefile for LKM project group 9
#  add this to make external libs compile with VSJMod
#  VSJModule-y := VSJModule.o kvdb.o
#  KBUILD_EXTRA_SYMBOLS=$(PWD)/Module.symvers

#CONFIG_MODULE_SIG=n
ifneq ($(KERNELRELEASE),)
obj-m := ebbchar.o
ebbchar-y := ebbchara.o ebbcharb.o

else
# normal makefile
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$PWD clean

# Module specific targets
endif

#all:
#	make -C /lib/modules/`uname -r`)/build M=$PWD	modules
#	gcc testebbchar.c -o test
#
