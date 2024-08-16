# Makefile

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := driver.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	gcc test.c -o test

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -f test
