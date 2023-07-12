# TOOLCHAIN_DIR?=../aarch64-buildroot-linux-gnu_sdk-buildroot/bin/
# CROSS_COMPILE?=

# PATH := $(TOOLCHAIN_DIR):$(PATH)

ARCH=arm64

CFLAGS_mfrc522_module.o := -O0 -g -c
#cflags-y += -O0 -g -c -std=gnu11
#LDFLAGS:= -T plat/$(PLAT)/link.ld

# CC=$(CROSS_COMPILE)gcc
# LD=$(CROSS_COMPILE)ld
# AS=$(CROSS_COMPILE)as
# OBJCOPY=$(CROSS_COMPILE)objcopy
# OBJDUMP=$(CROSS_COMPILE)objdump
