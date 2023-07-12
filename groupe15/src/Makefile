MODULE_PATH?=$(shell pwd)

$(info $(ARCH) $(CROSS_COMPILE) $(KDIR) $(MODULE_PATH))

include $(MODULE_PATH)/config.mk

obj-m += mfrc522.o

mfrc522-objs += mfrc522_module.o \
				mfrc522_spi.o \

# KDIR ?= /lib/modules/`uname -r`/build

PWD  := $(shell pwd)

ENV = PATH=$(PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) MODULE_PATH=$(shell pwd)
MAKE_KO = $(ENV) $(MAKE) -C $(KDIR) M=$(PWD) LLVM=1

.PHONY: modules
modules:
	$(MAKE_KO) modules

.PHONY: install
install:
	$(MAKE_KO) modules_install

.PHONY: clean
clean:
	rm -f *.ko *.cmd *.symvers *.order .*.cmd *.o *.mod.c *.mod *.d
