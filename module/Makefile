#  Makefile for LKM project group 9
#  add this to make external libs compile with VSJMod
#  VSJModule-y := VSJModule.o kvdb.o
#  KBUILD_EXTRA_SYMBOLS=$(PWD)/Module.symvers 


obj-m += VSJModule.o
VSJModule-y := VSJModule.o kvdb.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc testebbchar.c -o test 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean