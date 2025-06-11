EXTRA_CFLAGS = -Wall
obj-m = file-opt.o

KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=`pwd` clean