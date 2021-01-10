#!/bin/bash
#set architectute
export ARCH=arm
#set cross compiller path /toolchain
export CROSS_COMPILE=~/alps/q0_alps_zte_a476/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-
#set directory for build output
export  KBUILD_OUTPUT=out32
#set defconfig
make zte_blade_a476_defconfig
#start compile
make zImage-dtb -j5 2>&1 | tee out32/build.log
#make menuconfig
