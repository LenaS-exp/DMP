obj-m += dmp.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules