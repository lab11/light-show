DRIVER = spike

TARGET = ledstrip

obj-m += $(TARGET).o
ledstrip-objs =

# Set this is your linux kernel checkout.
KDIR := /home/bradjc/git/rpi-linux
PWD := $(shell pwd)

default:
		  $(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
		  $(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

.PHONY: clean default

