#!/bin/sh

set -e

# export PATH=$(pwd)/gistre24-dril-sdk-v2/dril-sdk/buildroot-dril-sdk/aarch64-buildroot-linux-gnu_sdk-buildroot/bin/:"$PATH"
# export KDIR=$(pwd)/gistre24-dril-sdk-v2/dril-sdk/buildroot-dril-sdk/linux-build/
export KDIR=/home/shebour/Documents/linux/
export KBUILD_EXTRA_SYMBOLS=/home/shebour/Documents/linux/Module.symvers
export ARCH=arm64
export KBUILD_DUPLICATED_EXPORTS_NO_WARN=1
# export CROSS_COMPILE=aarch64-buildroot-linux-gnu-
make modules -C $(pwd)/groupe15/src

