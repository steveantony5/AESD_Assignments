LDIR=$(HOME)/Build_root/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220/

CDIR=$(HOME)/Build_root/buildroot/output/host/bin/


obj-m +=timer_sys.o

all:
	make ARCH=arm CROSS_COMPILE=$(CDIR)arm-buildroot-linux-uclibcgnueabihf- -C $(LDIR) M=$(shell pwd) modules

clean:
	make -c $(LDIR) M=$(shell pwd) clean
