#/bin/bash

#Firstly unset CFLAGS
unset CFLAGS
#the system that we will be creating a toolchain for
export TARGET=arm-unknown-linux-gnueabihf
#the root location to be installed to
export ROOTDIR=/data/BBB
#the source directory we will be storing downloaded files
export SRCDIR=${ROOTDIR}/sources
#set the build tool location
export BUILDTOOLSYSDIR=${ROOTDIR}/tools/build
#the prefix that will be before all the cross compiled programs
export CROSS_COMPILE=${TARGET}-
#update the path to include the compiled binary programs
export PATH=${PATH}:/${BUILDTOOLSYSDIR}/bin
#set the build machine architechure
export BUILD=$(echo ${MACHTYPE} | sed "s/-[^-]*/-cross/")
#the target architechure
export ARCH=arm
#the arm architechure
export ARM_ARCH=armv7-a
#the float point architechure
export FPU=neon
#the float point type
export FLOAT=hard
export CC=${TARGET}-gcc
export CXX=${TARGET}-g++
export AR=${TARGET}-ar
export AS=${TARGET}-as
export LD=${TARGET}-ld
export RANLIB=${TARGET}-ranlib
export READELF=${TARGET}-readelf
export STRIP=${TARGET}-strip
export TARGETFS=${ROOTDIR}/targetfs2
