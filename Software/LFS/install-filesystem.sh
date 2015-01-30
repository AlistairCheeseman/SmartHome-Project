#/bin/bash

set -e
export DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ./buildscript.sh

rm -R $TARGETFS
rm -R $SRCDIR
mkdir -pv $TARGETFS
cd $TARGETFS

ln -svf usr/bin  $TARGETFS/bin
ln -svf usr/sbin $TARGETFS/sbin
ln -svr usr/lib  $TARGETFS/lib


#mkdir -pv  $TARGETFS/usr/{,local/}{bin,include,lib,sbin,share,src}
mkdir -pv  $TARGETFS/usr/{,local/}{bin,include,lib,sbin,share,src}
#mkdir -pv  $TARGETFS/{bin,boot,dev,etc,home,lib/{firmware,modules}}
mkdir -pv  $TARGETFS/{boot,dev,etc,home,lib/{firmware,modules}}
#mkdir -pv  $TARGETFS/{mnt,opt,proc,sbin,srv,sys}
mkdir -pv  $TARGETFS/{mnt,opt,proc,srv,sys}
mkdir -pv  $TARGETFS/var/{cache,lib,local,lock,log,opt,run,spool}
install -dv -m 0750  $TARGETFS/root
install -dv -m 1777  $TARGETFS/tmp


ln -svf ../proc/mounts ${TARGETFS}/etc/mtab





cat > ${TARGETFS}/etc/passwd << "EOF"
root::0:0:root:/root:/bin/ash
mosquitto::100:100:mosquitto daemon::
sshd::101:101:SSH Daemon::
httpd::102:102:httpd daemon::
EOF

cat > ${TARGETFS}/etc/group << "EOF"
root:x:0:
bin:x:1:
sys:x:2:
kmem:x:3:
tty:x:4:
tape:x:5:
daemon:x:6:
floppy:x:7:
disk:x:8:
lp:x:9:
dialout:x:10:
audio:x:11:
video:x:12:
utmp:x:13:
usb:x:14:
cdrom:x:15:
mosquitto:x:100:
sshd:x:101:
httpd:x:102:
EOF




touch ${TARGETFS}/var/run/utmp ${TARGETFS}/var/log/{btmp,lastlog,wtmp}
chmod -v 664 ${TARGETFS}/var/run/utmp ${TARGETFS}/var/log/lastlog










mkdir -pv $SRCDIR
cd $SRCDIR






wget ftp://ftp.denx.de/pub/u-boot/u-boot-2014.07.tar.bz2
tar -xf u-boot-2014.07.tar.bz2
rm u-boot-2014.07.tar.bz2
cd u-boot-2014.07
#note CROSS_COMPILE and ARCH must be set for this to work on the target system.
make am335x_evm_config
make


cp MLO $TARGETFS/boot/MLO
cp u-boot.img $TARGETFS/boot/u-boot.img

#mkimage has been built for the build system
cp tools/mkimage ${BUILDTOOLSYSDIR}/bin/mkimage

#git no longer clones to /kernel.
#cp -R ${BUILDTOOLSYSDIR}/../src/kernel  $SRCDIR/kernel
cp -R ${BUILDTOOLSYSDIR}/../src/linux  $SRCDIR/linux

#cd $SRCDIR/kernel/kernel
cd $SRCDIR/linux
#make beaglebone_defconfig
make ARCH=arm bb.org_defconfig
#this bit requires user input.
#reccommended changes:
#Device Drivers > SPI Support > (ENABLE) Debug Support for SPI drivers
# CONFIG_SPI_DEBUG is not set --> CONFIG_SPI_DEBUG=y
#Device Drivers > LED Support > LED Trigger Support > (DISABLE) LED Default ON Trigger
#CONFIG_LEDS_TRIGGER_DEFAULT_ON=y --> #CONFIG_LEDS_TRIGGER_DEFAULT_ON is not set

#make menuconfig
#edit the device tree to allow for UART and SPI Access.
#spi1 can be used when hdmi is still in use, additionally there are two lines to enable serial. ENSURE the *-bone-* is selected for serial access.
#nano arch/arm/boot/dts/am335x-boneblack.dts
make uImage dtbs LOADADDR=0x80008000

make modules
make INSTALL_MOD_PATH=$TARGETFS modules_install

cp arch/arm/boot/uImage $TARGETFS/boot/uImage
cp arch/arm/boot/zImage $TARGETFS/boot/zImage


cp arch/arm/boot/dts/am335x-boneblack.dtb $TARGETFS/boot/am335x-boneblack.dtb


#24-jan-2015 don't need the DTB rebuilder as have solved SPI + UART Issue in the main linux build.
#https://github.com/RobertCNelson/dtb-rebuilder/tree/3.14-ti
#cd $SRCDIR
#git clone https://github.com/RobertCNelson/dtb-rebuilder.git
#cd dtb-rebuilder
#git checkout origin/3.14-ti
#UNCOMMENT SPI1 to read:
#include "am335x-bone-spi1-spidev.dtsi
#nano src/arm/am335x-boneblack.dts
#cp ../linux/arch/arm/boot/dts/am335x-boneblack.dtb src/arm/am335x-boneblack.dtb
#make DTC=../linux/scripts/dtc/dtc src/arm/am335x-boneblack.dtb
#cp src/arm/am335x-boneblack.dtb $TARGETFS/boot/am335x-boneblack.dtb



cat > $TARGETFS/boot/uEnv.txt << EOF
serverip=192.168.3.1
autoload=no
bootargs=console=ttyO0,115200n8 rw ip=dhcp root=/dev/nfs rootfstype=nfs nfsroot=192.168.3.1:/data/arm/targetfs,nolock,vers=3
uenvcmd=dhcp;tftp 0x80200000 uImage;tftp 0x80f80000 am335x-boneblack.dtb;bootm 0x80200000 - 0x80f80000
EOF





cd $SRCDIR
wget http://busybox.net/downloads/busybox-1.22.1.tar.bz2
tar -xf busybox-1.22.1.tar.bz2
rm busybox-1.22.1.tar.bz2
cd busybox-1.22.1
make defconfig
make menuconfig
#remove rpc support as cannot build with against new versions of glibc
#Networking Utilities --> (DISABLE) inetd/Support RPC services
#CONFIG_PREFIX sets the install directory
make CONFIG_PREFIX="/remote/arm/targetfs2"
make CONFIG_PREFIX="/remote/arm/targetfs2" install
chmod +x ${TARGETFS}/bin/busybox


cp examples/depmod.pl ${BUILDTOOLSYSDIR}/bin/depmod.pl
chmod 755 ${BUILDTOOLSYSDIR}/bin/depmod.pl



cd ${SRCDIR}
wget http://sethwklein.net/iana-etc-2.30.tar.bz2
tar -xf iana-etc-2.30.tar.bz2
rm iana-etc-2.30.tar.bz2
cd iana-etc-2.30
make get
make STRIP=yes
make DESTDIR=$TARGETFS install


cp ${DIR}/resources/fstab.nfs ${TARGETFS}/etc/fstab.nfs
cp ${DIR}/resources/fstab.local ${TARGETFS}/etc/fstab.local
cp ${TARGETFS}/etc/fstab.nfs ${TARGETFS}/etc/fstab

cp ${DIR}/resources/mdev.conf ${TARGETFS}/etc/mdev.conf
cp ${DIR}/resources/profile ${TARGETFS}/etc/profile
cp ${DIR}/resources/inittab ${TARGETFS}/etc/inittab
cp ${DIR}/resources/hosts ${TARGETFS}/etc/hosts
cp ${DIR}/resources/shells ${TARGETFS}/etc/shells




cp ${DIR}/resources/rc.shutdown ${TARGETFS}/etc/rc.shutdown
cp ${DIR}/resources/rc.sysinit ${TARGETFS}/etc/rc.sysinit

chmod +x ${TARGETFS}/etc/rc.sysinit
chmod +x ${TARGETFS}/etc/rc.shutdown

echo "beaglebone" > ${TARGETFS}/etc/HOSTNAME

cp ${DIR}/resources/resolv.conf ${TARGETFS}/etc/resolv.conf

mkdir $TARGETFS/etc/init.d

cp ${DIR}/resources/sshd ${TARGETFS}/etc/init.d/sshd
cp ${DIR}/resources/syslog ${TARGETFS}/etc/init.d/syslog
cp ${DIR}/resources/mosquitto ${TARGETFS}/etc/init.d/mosquitto

chmod +x ${TARGETFS}/etc/init.d/sshd
chmod +x ${TARGETFS}/etc/init.d/syslog
chmod +x ${TARGETFS}/etc/init.d/mosquitto

cp ${DIR}/resources/functions ${TARGETFS}/etc/init.d/functions

mkdir $TARGETFS/etc/rcS.d

#//install  sshd function and also syslog etc

#link scripts
cd ${TARGETFS}/etc/rcS.d
ln -sfv ../init.d/syslog S05syslog
ln -sfv ../init.d/sshd S30sshd
ln -sfv ../init.d/mosquitto S40mosquitto



cd $SRCDIR
wget http://www.mirrorservice.org/pub/OpenBSD/OpenSSH/portable/openssh-6.7p1.tar.gz
tar -xf openssh-6.7p1.tar.gz
rm openssh-6.7p1.tar.gz
cd openssh-6.7p1
./configure --host=$TARGET --with-libs --with-zlib=${BUILDTOOLSYSDIR}/sysroot --prefix=/ --with-ssl-dir=${BUILDTOOLSYSDIR}/sysroot LD=$TARGET-gcc AR=$TARGET-ar  --sysconfdir=/etc/ssh 
LD=$TARGET-gcc AR=$TARGET-ar make
make install-nokeys LD=$TARGET-gcc AR=$TARGET-ar DESTDIR=${TARGETFS} STRIP_OPT="-s --strip-program=arm-unknown-linux-gnueabihf-strip"

cd $SRCDIR
wget http://mosquitto.org/files/source/mosquitto-1.3.5.tar.gz
tar -xf mosquitto-1.3.5.tar.gz
rm mosquitto-1.3.5.tar.gz
cd mosquitto-1.3.5
##maybe change prefix install dir as installs in /usr/local/ this is at the bottom of the config.mk
make
make install prefix='' DESTDIR=$TARGETFS

cp $TARGETFS/etc/mosquitto/mosquitto.conf.example $TARGETFS/etc/mosquitto/mosquitto.conf



cd $SRCDIR
wget http://rsync.samba.org/ftp/rsync/src/rsync-3.1.1.tar.gz
tar -xf rsync-3.1.1.tar.gz
rm rsync-3.1.1.tar.gz
cd rsync-3.1.1
./configure --host=$TARGET --prefix=/
make
make install prefix=/ DESTDIR=$TARGETFS



#APR
cd $SRCDIR
wget http://mirror.catn.com/pub/apache/apr/apr-1.5.1.tar.bz2
tar -xf apr-1.5.1.tar.bz2
rm apr-1.5.1.tar.bz2
cd apr-1.5.1
#may have to be installed in src dir.
#have not tested apr_cv_tcp_nodelay_with_cork
./configure --host=$TARGET --prefix=$SRCDIR/apache/apr-build ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_tcp_nodelay_with_cork=no ac_cv_sizeof_struct_iovec=1
#copy the output from the test program as we can't run it on the build machine
cp ${DIR}/resources/apr_escape_test_char.h include/private/
#comment out as we have included the file from the test machine
#include/private/apr_escape_test_char.h: tools/gen_test_char
#        $(APR_MKDIR) include/private
#        tools/gen_test_char > $@
#
#LINK_PROG = $(LIBTOOL) $(LTFLAGS) --mode=link $(COMPILE) $(LT_LDFLAGS) \
#            -no-install $(ALL_LDFLAGS) -o $@

nano Makefile
make
make install
cp -v $SRCDIR/apache/apr-build/lib/*.so* $TARGETFS/lib/



cd ${SRCDIR}
wget http://mirror.catn.com/pub/apache/apr/apr-util-1.5.4.tar.gz
tar -xf apr-util-1.5.4.tar.gz
rm apr-util-1.5.4.tar.gz
cd apr-util-1.5.4
./configure --host=${TARGET} --prefix=${SRCDIR}/apache/apr-util-build --with-apr=${SRCDIR}/apache/apr-build
make
make install
cp -v ${SRCDIR}/apache/apr-util-build/lib/*.so* $TARGETFS/lib/


cd ${SRCDIR}
wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-8.36.tar.gz
tar -xf pcre-8.36.tar.gz
rm pcre-8.36.tar.gz
cd pcre-8.36
./configure --host=${TARGET} --prefix=${SRCDIR}/apache/pcre-build
make
make install
cp -v ${SRCDIR}/apache/pcre-build/lib/*.so* $TARGETFS/lib/


cd ${SRCDIR}
wget http://mirror.catn.com/pub/apache/httpd/httpd-2.4.10.tar.gz
tar -xf httpd-2.4.10.tar.gz
rm httpd-2.4.10.tar.gz
cd httpd-2.4.10
#set the default user
sed -i -e 's/User daemon/User httpd/g' ${SRCDIR}/httpd-2.4.10/docs/conf/httpd.conf.in
sed -i -e 's/Group daemon/Group httpd/g' docs/conf/httpd.conf.in
./configure --prefix=/apache24 --host=${TARGET} --with-apr=${SRCDIR}/apache/apr-build --with-apr-util=${SRCDIR}/apache/apr-util-build  --with-pcre=${SRCDIR}/apache/pcre-build ap_cv_void_ptr_lt_long=no --with-mpm=prefork  --sysconfdir=/apache24/etc
#this will fail, but it must be ran to allow the below replacement to take place. ||true ensures that the broken make will not halt the buildscript
make || true
cp /media/alistair/86D8948AD89479DF/Users/Ali.ALi-PC/Source/Repos/SmartHome-Project/Software/LFS/resources/test_char.h server/test_char.h
make
#bug with cmake that misses out cmake (ONLY NEEDED IF USING EVENT AS MPM)
#http://unix.stackexchange.com/questions/33396/gcc-cant-link-to-pthread
#/remote/arm/sources/apache/apr-build/build-1/libtool --mode=link arm-unknown-linux-gnueabihf-gcc -std=gnu99  -g -O2 -pthread        -o httpd  modules.lo buildmark.o -export-dynamic server/libmain.la modules/#core/libmod_so.la modules/http/libmod_http.la server/mpm/event/libevent.la os/unix/libos.la -L/remote/arm/sources/apache/pcre-build/lib -lpcre     /remote/arm/sources/apache/apr-util-build/lib/libaprutil-1.la -#lexpat /remote/arm/sources/apache/apr-build/lib/libapr-1.la -lrt -lcrypt -ldl 
#make
make DESTDIR=${TARGETFS} install







#need to add libraries from apr, apr-util, pcre
cp -vP /${BUILDTOOLSYSDIR}/sysroot/lib/*.so* ${TARGETFS}/lib/
cp -vP /${BUILDTOOLSYSDIR}/arm-unknown-linux-gnueabihf/lib/*.so* ${TARGETFS}/lib/







cd $SRCDIR
wget ftp://xmlsoft.org/libxml2/libxml2-2.9.2.tar.gz
tar -xf libxml2-2.9.2.tar.gz
rm libxml2-2.9.2.tar.gz
#git clone git://git.gnome.org/libxml2
cd libxml2-2.9.2
./configure --host=$TARGET --without-python --prefix=''
make
make  DESTDIR=/remote/arm/tools/build/sysroot install
make  DESTDIR=${TARGETFS} install

#need to install sqlite
cd $SRCDIR
wget http://www.sqlite.org/2015/sqlite-autoconf-3080801.tar.gz
tar -xf sqlite-autoconf-3080801.tar.gz
rm sqlite-autoconf-3080801.tar.gz
cd sqlite-autoconf-3080801
./configure --host=$TARGET --prefix=''
make
make DESTDIR=/remote/arm/tools/build/sysroot install
make  DESTDIR=${TARGETFS} install





cd $SRCDIR
wget http://uk1.php.net/get/php-5.6.5.tar.gz/from/this/mirror
mv mirror php-5.6.5.tar.gz
tar -xf php-5.6.5.tar.gz
rm php-5.6.5.tar.gz
cd php-5.6.5
#sed -i -e 's/my $installbuilddir = "/apache24/build";/' 
nano ${TARGETFS}/apache24/bin/apxs
./configure --host=$TARGET --prefix='' --with-libxml-dir=/remote/arm/tools/build/sysroot --with-sqlite3 --disable-all --with-apxs2=${TARGETFS}/apache24/bin/apxs 
LDFLAGS='-ldl' make
make INSTALL_ROOT=/remote/arm/targetfs2 install
#modify the incorrect location of the php module
#sed -i -e 's///remote//arm//targetfs2//apache24////'



#install the WWW data from the git repo to the filesystem.
cp -Rv $DIR/../www/* $TARGETFS/apache24/htdocs/
echo "WWW Files Copied"

echo "####################################################################"
echo "####################################################################"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                           COMPLETED SUCESSFULLY                  #"
echo "#                      INSTALLED TO ${TARGETFS}                    #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "####################################################################"
echo "####################################################################"








