KDIR:=/home/arc/RPi/linux-rpi
#PWD:= $(shell pwd)

CC = arm-bcm2708hardfp-linux-gnueabi-gcc
ARCH = arm
CROSS_COMPILE = arm-bcm2708hardfp-linux-gnueabi-
SROOT=/home/arc/RPi/tools-master/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/arm-bcm2708hardfp-linux-gnueabi/sysroot



obj-m:= ledmatrix.o

all:
	make ARCH=arm CROSS_COMPILE=arm-bcm2708hardfp-linux-gnueabi- -C $(KDIR) M=$(PWD) modules

clean:
	rm -f *.ko
	rm -f *.o
	rm -f Module.symvers
	rm -f modules.order
	rm -f *.mod.c
	rm -rf .tmp_versions
	rm -f *.mod.c
	rm -f *.mod.o
	rm -f \.*.cmd
	rm -f Module.markers
	rm -f $(APP) 
