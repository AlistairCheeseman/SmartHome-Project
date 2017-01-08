#/bin/bash


#Firstly unset CFLAGS
unset CFLAGS
#the root location to be installed to
export ROOTDIR=/data/embedded/BBB


#the system that we will be creating a toolchain for
export TARGET=arm-unknown-linux-gnueabihf

#the target architechure
export ARCH=arm
#the arm architechure
export ARM_ARCH=armv7-a
#the float point architechure
export FPU=neon
#the float point type
export FLOAT=hard


export PACKAGEFS=FALSE
