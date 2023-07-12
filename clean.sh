#!/bin/sh

set -e

export PATH=$(pwd)/gistre24-dril-sdk-v2/dril-sdk/buildroot-dril-sdk/aarch64-buildroot-linux-gnu_sdk-buildroot/bin/:"$PATH"
export KDIR=$(pwd)/gistre24-dril-sdk-v2/dril-sdk/buildroot-dril-sdk/linux-build/
export KBUILD_EXTRA_SYMBOLS=$(pwd)/gistre24-dril-sdk-v2/dril-sdk/mfrc522_emu-sdk/Module.symvers
export ARCH=arm64
export CROSS_COMPILE=aarch64-buildroot-linux-gnu-
make clean -C $(pwd)/groupe15/src
