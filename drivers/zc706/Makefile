MODULES := basic.o

#guest architecture
ARCH := arm
CROSS_COMPILE := arm-xilinx-linux-gnueabi-
obj-m := $(MODULES)

#path of the arm compiled linux kernel
ROOTDIR := 

MAKEARCH := $(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

all: modules

modules:
	    $(MAKEARCH) -C $(ROOTDIR) M=${shell pwd} modules
clean:
		    $(MAKEARCH) -C $(ROOTDIR) M=${shell pwd} clean

