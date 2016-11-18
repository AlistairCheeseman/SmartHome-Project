#!/bin/bash
#last tested 19-jan-15
set -e

export DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source config.sh


#the source directory we will be storing downloaded files.
export SRCDIR=${ROOTDIR}/tools/src
#the directory we want our build tools installed into.
export PREFIX=${ROOTDIR}/tools/build
#where the final library files will be stored
export SYSROOT=${PREFIX}/sysroot

#the prefix that will be before all cross compiled programs
export CROSS_COMPILE=${TARGET}-
#update the path to include the compiled binary programs
export PATH=$PATH:${PREFIX}/bin
#set the host machine architechure
export HOST=$(echo ${MACHTYPE} | sed "s/-[^-]*/-cross/")


export MAKEINFO_LOC=$(which makeinfo)

export USE_MIRROR=TRUE
# if using newer versions of GCC (v6) on the host to build the toolchain then we need to force gnu++98 to allow it to build successfully
export CXX='g++ -std=gnu++98'

#create the sysroot directory and link usr to the local folder to ensure everything is installed in root
mkdir -p $ROOTDIR
chmod -R 777 $ROOTDIR
rm -Rf $ROOTDIR/*
mkdir -p $SRCDIR
mkdir -p $SYSROOT

ln -s . $SYSROOT/usr

cd $SRCDIR

export BINUTILS_VER=2.25
export GCC_VER=4.9.1
export GLIBC_VER=2.19
export GMP_VER=6.0.0a
export GMP_DIR=6.0.0
export MPFR_VER=3.1.3
export MPC_VER=1.0.2
export LIBELF_VER=0.8.13
export NCURSES_VER=5.9
export LIBAIO_VER=0.3.109
export CARES_VER=1.10.0
export OPENSSL_VER=1.0.1i
export ZLIB_VER=1.2.8


if [ $USE_MIRROR == 'FALSE' ]; then

wget ftp://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VER}.tar.gz
wget ftp://ftp.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-${GCC_VER}/gcc-${GCC_VER}.tar.gz
wget ftp://ftp.gnu.org/gnu/glibc/glibc-2.23.tar.gz
wget https://gmplib.org/download/gmp/gmp-6.1.0.tar.bz2
wget http://www.mpfr.org/mpfr-current/mpfr-3.1.4.tar.gz
wget ftp://ftp.gnu.org/gnu/mpc/mpc-1.0.3.tar.gz
wget http://www.mr511.de/software/libelf-0.8.13.tar.gz

wget http://ftp.gnu.org/pub/gnu/ncurses/ncurses-6.0.tar.gz
wget https://git.fedorahosted.org/cgit/libaio.git/snapshot/libaio-0.3.110-1.tar.gz
wget http://c-ares.haxx.se/download/c-ares-1.11.0.tar.gz
wget ftp://ftp.openssl.org/source/openssl-1.0.2h.tar.gz
wget http://zlib.net/zlib-1.2.8.tar.gz

wget http://arago-project.org/git/projects/?p=am33x-cm3.git\;a=blob_plain\;f=bin/am335x-pm-firmware.bin\;hb=HEAD -O am335x-pm-firmware.bin
else

wget --no-check-certificate https://filestore.kmxsoftware.co.uk/binutils-${BINUTILS_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/gcc-${GCC_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/glibc-${GLIBC_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/gmp-${GMP_VER}.tar.bz2
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/mpfr-${MPFR_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/mpc-${MPC_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/libelf-${LIBELF_VER}.tar.gz

wget --no-check-certificate https://filestore.kmxsoftware.co.uk/ncurses-${NCURSES_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/libaio-${LIBAIO_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/c-ares-${CARES_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/openssl-${OPENSSL_VER}.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/zlib-${ZLIB_VER}.tar.gz

wget --no-check-certificate https://filestore.kmxsoftware.co.uk/am335x-pm-firmware.bin -O am335x-pm-firmware.bin
fi


echo "Extracting Files"
tar -xf binutils-${BINUTILS_VER}.tar.gz
tar -xf gcc-${GCC_VER}.tar.gz
tar -xf glibc-${GLIBC_VER}.tar.gz
tar -xf gmp-${GMP_VER}.tar.bz2
tar -xf mpfr-${MPFR_VER}.tar.gz
tar -xf mpc-${MPC_VER}.tar.gz
tar -xf libelf-${LIBELF_VER}.tar.gz

tar -xf zlib-${ZLIB_VER}.tar.gz
tar -xf openssl-${OPENSSL_VER}.tar.gz
tar -xf c-ares-${CARES_VER}.tar.gz
tar -xf libaio-${LIBAIO_VER}.tar.gz
tar -xf ncurses-${NCURSES_VER}.tar.gz

echo "Cleaning Directory Structure"
mv -v binutils-${BINUTILS_VER} binutils
mv -v gcc-${GCC_VER} gcc-src
mv -v glibc-${GLIBC_VER} glibc
mv -v gmp-${GMP_DIR} gmp
mv -v mpfr-${MPFR_VER} mpfr
mv -v mpc-${MPC_VER} mpc
mv -v libelf-${LIBELF_VER} libelf
mv -v zlib-${ZLIB_VER} zlib
mv -v openssl-${OPENSSL_VER} openssl
mv -v c-ares-${CARES_VER} c-ares
mv -v libaio-${LIBAIO_VER} libaio
mv -v ncurses-${NCURSES_VER} ncurses

 
mv mpfr gcc-src/mpfr
mv gmp gcc-src/gmp
mv mpc gcc-src/mpc
mv libelf gcc-src/libelf


rm binutils-${BINUTILS_VER}.tar.gz
rm gcc-${GCC_VER}.tar.gz
rm glibc-${GLIBC_VER}.tar.gz
rm gmp-${GMP_VER}.tar.bz2
rm mpfr-${MPFR_VER}.tar.gz
rm mpc-${MPC_VER}.tar.gz
rm libelf-${LIBELF_VER}.tar.gz

rm zlib-${ZLIB_VER}.tar.gz
rm openssl-${OPENSSL_VER}.tar.gz
rm ncurses-${NCURSES_VER}.tar.gz
rm c-ares-${CARES_VER}.tar.gz
rm libaio-${LIBAIO_VER}.tar.gz

echo "Getting LINUX Kernel from GIT"
git config --global user.email “ali@kmxsoftware.co.uk”

if [ $USE_MIRROR == 'FALSE' ]; then
git clone https://github.com/beagleboard/linux.git
else
#git clone git@git.kmxsoftware.co.uk:extern/linux.git
GIT_SSH=${DIR}/ssh_helper git clone git@git.kmxsoftware.co.uk:extern/linux.git
fi
#cd kernel
cd linux
#with new git repository this may not be needed anymore
#git checkout origin/3.13 -b 3.13
git checkout origin/3.14 -b 3.14
mv $SRCDIR/am335x-pm-firmware.bin firmware/am335x-pm-firmware.bin
#kernel
echo "Building Kernel Headers"
echo -e '\033]2;Building Kernel Headers\007'
cd $SRCDIR/linux
#make ARCH=arm beaglebone_defconfig  2>&1 | tee make.out
make ARCH=arm bb.org_defconfig
make ARCH=arm INSTALL_HDR_PATH=$SYSROOT headers_install  2>&1 | tee -a make.out
echo "Building BinUtils"
echo -e '\033]2;Building Binutils\007'
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
echo -e '\033]2;GCC Pass 1\007'
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
#echo "press enter to continue..."
#read -rsp $'Press any key to continue...\n' -n1 key
echo "GLibC"
echo -e '\033]2;GLIBC\007'
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
echo -e '\033]2;GCC Pass 2\007'
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
echo -e '\033]2;ZLIB\007'
 cd $SRCDIR/zlib
 ./configure  2>&1 | tee configure.out
 make  2>&1 | tee make.out
 make install prefix=${SYSROOT}
echo "OpenSSL"
echo -e '\033]2;OpenSSL\007'
 ##OPENSSL is needed for opennsh and mosquitto
 unset CROSS_COMPILE
 cd $SRCDIR/openssl
##MUST BE BUILT WITH -fPIC otherwise OpenSSH fails.
#if CROSS_COMPILE has been set:
#CC="gcc -fPIC" ./Configure dist --prefix=${SYSROOT} zlib-dynamic  2>&1 | tee configure.out
#other wise
#CC="${TARGET}-gcc -fPIC" ./Configure dist --prefix=${SYSROOT} zlib-dynamic  2>&1 | tee configure.out
CC="${TARGET}-gcc -fPIC" ./Configure dist --prefix=${SYSROOT} zlib-dynamic  2>&1 | tee configure.out
make  2>&1 | tee make.out
make install  2>&1 | tee -a make.out




echo "C-Ares"
echo -e '\033]2;C-Ares\007'
#C-ARES IS NEEDED FOR MOSQUITTO
cd $SRCDIR/c-ares
./configure --host=$TARGET --prefix=/  2>&1 | tee configure.out
make  2>&1 | tee make.out
make install DESTDIR=${SYSROOT} 2>&1 | tee -a make.out


echo "Libaio"
echo -e '\033]2;Libaio\007'
cd $SRCDIR/libaio
make 2>&1 | tee make.out
#ignore warnings about git. as this was not pulled from a git repo
make install prefix=$SYSROOT 2>&1 | tee -a make.out

echo "ncurses"
echo -e '\033]2;ncurses\007'
cd $SRCDIR/ncurses
./configure --prefix=/ --with-shared --host=$TARGET  2>&1 | tee configure.out
make  2>&1 | tee make.out
make install DESTDIR=$SYSROOT  2>&1 | tee -a make.out

#link libcurses to libncurses to enable older apps to work
cd $SYSROOT/lib
ln -sfv libncurses.so libcurses.so
ln -sfv libncurses.a libcurses.a

