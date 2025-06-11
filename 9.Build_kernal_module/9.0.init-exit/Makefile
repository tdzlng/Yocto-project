# Hello world kernel module

EXTRA_CFLAGS = -Wall
obj-m = hello.o

KDIR = /lib/modules/`uname -r`/build
# KDIR = /usr/src/linux-headers-`uname -r`

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=`pwd` clean