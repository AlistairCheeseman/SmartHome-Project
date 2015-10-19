#!/bin/bash
#last tested 19-jan-15
set -e

export DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#Firstly unset CFLAGS
unset CFLAGS
#the system that we will be creating a toolchain for.
export TARGET=arm-unknown-linux-gnueabihf
#the root location to be installed to
export ROOTDIR=/data/BBB
#the source directory we will be storing downloaded files.
export SRCDIR=${ROOTDIR}/tools/src
#the directory we want our build tools installed into.
export PREFIX=${ROOTDIR}/tools/build
#where the final library files will be stored
export SYSROOT=${PREFIX}/sysroot
#the target(see near beginning) architechure
export ARCH=arm
#the prefix that will be before all cross compiled programs
export CROSS_COMPILE=${TARGET}-
#update the path to include the compiled binary programs
export PATH=$PATH:${PREFIX}/bin
#set the host machine architechure
export HOST=$(echo ${MACHTYPE} | sed "s/-[^-]*/-cross/")
#from the target, some more details
# the target architecure
export ARCH=arm
#target arm architecture
export ARM_ARCH=armv7-a
#the float point architechure
export FPU=neon
#the float point type
export FLOAT=hard

export MAKEINFO_LOC=$(which makeinfo)



#create the sysroot directory and link usr to the local folder to ensure everything is installed in root
chmod -R 777 $ROOTDIR
mkdir -p $SRCDIR
mkdir -p $SYSROOT

ln -s . $SYSROOT/usr

cd $SRCDIR
#wget ftp://sourceware.org/pub/binutils/snapshots/binutils-2.24.51.tar.bz2
wget http://ftp.gnu.org/gnu/binutils/binutils-2.25.tar.bz2
wget ftp://ftp.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-4.9.1/gcc-4.9.1.tar.bz2
wget http://ftp.gnu.org/gnu/glibc/glibc-2.19.tar.bz2
wget https://gmplib.org/download/gmp/gmp-6.0.0a.tar.bz2
wget http://www.mpfr.org/mpfr-current/mpfr-3.1.3.tar.bz2
wget ftp://ftp.gnu.org/gnu/mpc/mpc-1.0.2.tar.gz
wget http://www.mr511.de/software/libelf-0.8.13.tar.gz

echo "Extracting Files"
tar -xf binutils-2.25.tar.bz2
tar -xf gcc-4.9.1.tar.bz2
tar -xf glibc-2.19.tar.bz2
tar -xf gmp-6.0.0a.tar.bz2
tar -xf mpfr-3.1.3.tar.bz2
tar -xf mpc-1.0.2.tar.gz
tar -xf libelf-0.8.13.tar.gz
echo "Cleaning Directory Structure"
mv -v binutils-2.25 binutils
mv -v gcc-4.9.1 gcc-src
mv -v glibc-2.19 glibc
mv -v gmp-6.0.0 gmp
mv -v mpfr-3.1.3 mpfr
mv -v mpc-1.0.2 mpc
mv -v libelf-0.8.13 libelf

mv mpfr gcc-src/mpfr
mv gmp gcc-src/gmp
mv mpc gcc-src/mpc
mv libelf gcc-src/libelf


rm binutils-2.25.tar.bz2
rm gcc-4.9.1.tar.bz2
rm glibc-2.19.tar.bz2
rm gmp-6.0.0a.tar.bz2
rm mpfr-3.1.3.tar.bz2
rm mpc-1.0.2.tar.gz
rm libelf-0.8.13.tar.gz
echo "Getting LINUX Kernel from GIT"
git config --global user.email “ali@kmxsoftware.co.uk”

#repository moved.
#git clone https://github.com/beagleboard/kernel.git
git clone https://github.com/beagleboard/linux.git
#cd kernel
cd linux
#with new git repository this may not be needed anymore
#git checkout origin/3.13 -b 3.13
#git checkout origin/3.14 -b 3.14
#
#./patch.sh
#cp configs/beaglebone kernel/arch/arm/configs/beaglebone_defconfig
#wget http://arago-project.org/git/projects/?p=am33x-cm3.git\;a=blob_plain\;f=bin/am335x-pm-firmware.bin\;hb=HEAD -O kernel/firmware/am335x-pm-firmware.bin
wget http://arago-project.org/git/projects/?p=am33x-cm3.git\;a=blob_plain\;f=bin/am335x-pm-firmware.bin\;hb=HEAD -O firmware/am335x-pm-firmware.bin
#kernel
echo "Building Kernel Headers"
cd $SRCDIR/linux
#make ARCH=arm beaglebone_defconfig  2>&1 | tee make.out
make ARCH=arm bb.org_defconfig
make ARCH=arm INSTALL_HDR_PATH=$SYSROOT headers_install  2>&1 | tee -a make.out
echo "Building BinUtils"
#binutils
mkdir -pv $SRCDIR/binutils-build
cd $SRCDIR/binutils-build
../binutils/configure --prefix=${PREFIX} --target=${TARGET} --with-sysroot=${SYSROOT} --disable-nls --disable-multilib 2>&1 | tee configure.out
make configure-host 2>&1 | tee -a configure.out
#there is a bug in binutils that incorrectly reports makeinfo is not installed so it must manually be input.
#http://ubuntuforums.org/showthread.php?t=708309
make MAKEINFO=${MAKEINFO_LOC} 2>&1 | tee make.out
make install 2>&1 | tee -a make.out

 

#gcc pass 1
echo "GCC - Pass 1"
mkdir -pv $SRCDIR/gcc-build-1
 cd $SRCDIR/gcc-build-1
 ../gcc-src/configure \
--prefix=${PREFIX} \
--target=${TARGET} \
--host=${HOST} \
--build=${HOST} \
--with-sysroot=${SYSROOT} \
--disable-nls \
--disable-shared \
--without-headers \
--with-newlib \
--disable-decimal-float \
--disable-libgomp \
--disable-libmudflap \
--disable-libssp \
--disable-libatomic \
--disable-libquadmath \
--disable-threads \
--enable-languages=c \
--disable-multilib \
--with-mpfr-include=$(pwd)/../gcc-src/mpfr/src \
--with-mpfr-lib=$(pwd)/mpfr/src/.libs \
--with-arch=${ARM_ARCH} \
--with-float=${FLOAT} \
--with-fpu=${FPU} \
--with-mpc-include=$(pwd)/../gcc-src/mpc/src \
--with-mpc-lib=$(pwd)/mpc/src/.libs \
 2>&1 | tee configure.out

make 2>&1 | tee make.out
make install 2>&1 | tee -a make.out
echo "GLibC"
#glibc-build
mkdir $SRCDIR/glibc-build
cd $SRCDIR/glibc-build
#glibc
BUILD_CC=gcc CC=${CROSS_COMPILE}gcc \
AR=${CROSS_COMPILE}ar \
RANLIB=${CROSS_COMPILE}ranlib \
AS=${CROSS_COMPILE}as \
LD=${CROSS_COMPILE}ld \
../glibc/configure \
--prefix=/ \
--build=$HOST \
--host=$TARGET \
--target=$TARGET \
--without-__thread \
 --with-headers=${SYSROOT}/include \
 2>&1 | tee configure.out
 make 2>&1 | tee make.out
 make install install_root=$SYSROOT 2>&1 | tee -a make.out


echo "GCC - Pass 2"
  mkdir $SRCDIR/gcc-build-2
 cd $SRCDIR/gcc-build-2
 #gcc-build-2
 ../gcc-src/configure \
 --prefix=${PREFIX} \
 --build=${HOST} \
 --host=${HOST} \
 --target=${TARGET} \
 --with-sysroot=${SYSROOT} \
 --disable-nls \
 --enable-languages=c,c++ \
 --enable-c99 \
 --enable-long-long \
 --disable-libmudflap \
 --disable-multilib \
 --with-mpfr-include=$(pwd)/../gcc-src/mpfr/src \
 --with-mpfr-lib=$(pwd)/mpfr/src/.libs \
 --with-arch=${ARM_ARCH} \
 --with-float=${FLOAT} \
 --with-fpu=${FPU} \
 --with-mpc-include=$(pwd)/../gcc-src/mpc/src \
 --with-mpc-lib=$(pwd)/mpc/src/.libs \
 2>&1 | tee configure.out
 make 2>&1 | tee make.out
 make install 2>&1 | tee -a make.out

export CC=arm-unknown-linux-gnueabihf-gcc


echo "ZLIB"
 cd $SRCDIR
 wget http://zlib.net/zlib-1.2.8.tar.gz
 tar -xf zlib-1.2.8.tar.gz
rm zlib-1.2.8.tar.gz
 cd zlib-1.2.8
 ./configure  2>&1 | tee configure.out
 make  2>&1 | tee make.out
 make install prefix=${SYSROOT}
echo "OpenSSL"
 ##OPENSSL is needed for opennsh and mosquitto
 unset CROSS_COMPILE
 cd $SRCDIR
wget http://www.openssl.org/source/openssl-1.0.1i.tar.gz
tar -xf openssl-1.0.1i.tar.gz
rm openssl-1.0.1i.tar.gz
cd openssl-1.0.1i
##MUST BE BUILT WITH -fPIC otherwise OpenSSH fails.
#if CROSS_COMPILE has been set:
#CC="gcc -fPIC" ./Configure dist --prefix=${SYSROOT} zlib-dynamic  2>&1 | tee configure.out
#other wise
#CC="${TARGET}-gcc -fPIC" ./Configure dist --prefix=${SYSROOT} zlib-dynamic  2>&1 | tee configure.out
CC="${TARGET}-gcc -fPIC" ./Configure dist --prefix=${SYSROOT} zlib-dynamic  2>&1 | tee configure.out
make  2>&1 | tee make.out
make install  2>&1 | tee -a make.out




echo "C-Ares"
#C-ARES IS NEEDED FOR MOSQUITTO
cd $SRCDIR
wget http://c-ares.haxx.se/download/c-ares-1.10.0.tar.gz
tar -xf c-ares-1.10.0.tar.gz
rm c-ares-1.10.0.tar.gz
cd c-ares-1.10.0
./configure --host=$TARGET --prefix=/  2>&1 | tee configure.out
make  2>&1 | tee make.out
make install DESTDIR=${SYSROOT} 2>&1 | tee -a make.out


echo "Libaio"
cd $SRCDIR
wget https://git.fedorahosted.org/cgit/libaio.git/snapshot/libaio-0.3.109.tar.gz
tar -xf libaio-0.3.109.tar.gz
rm libaio-0.3.109.tar.gz
cd libaio-0.3.109
make 2>&1 | tee make.out
#ignore warnings about git. as this was not pulled from a git repo
make install prefix=$SYSROOT 2>&1 | tee -a make.out

echo "ncurses"
cd $SRCDIR
wget http://ftp.gnu.org/pub/gnu/ncurses/ncurses-5.9.tar.gz
tar -xf ncurses-5.9.tar.gz
rm ncurses-5.9.tar.gz
cd ncurses-5.9
./configure --prefix=/ --host=$TARGET  2>&1 | tee configure.out
make  2>&1 | tee make.out
make install DESTDIR=$SYSROOT  2>&1 | tee -a make.out
