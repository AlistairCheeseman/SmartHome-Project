#!/bin/bash

set -e
STARTTIME=$(date +%s)
export DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ./buildscript.sh


export USE_MIRROR=TRUE #whether to use filestore internal server
export NETBOOT=TRUE #whether to use TFP & NFS.


rm -Rf $TARGETFS
rm -Rf $SRCDIR
mkdir -pv $TARGETFS
cd $TARGETFS

ln -svf usr/bin  $TARGETFS/bin
ln -svf usr/sbin $TARGETFS/sbin
ln -svf usr/lib  $TARGETFS/lib


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

if [ $USE_MIRROR == 'TRUE' ]; then


wget --no-check-certificate https://filestore.kmxsoftware.co.uk/u-boot-2014.07.tar.bz2
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/busybox-1.22.1.tar.bz2
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/iana-etc-2.30.tar.bz2
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/openssh-6.7p1.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/php-5.6.5.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/sqlite-autoconf-3080801.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/libxml2-2.9.2.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/httpd-2.4.18.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/pcre-8.38.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/apr-util-1.5.4.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/apr-1.5.2.tar.bz2
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/rsync-3.1.1.tar.gz
wget --no-check-certificate https://filestore.kmxsoftware.co.uk/mosquitto-1.3.5.tar.gz
else
wget ftp://ftp.denx.de/pub/u-boot/u-boot-2014.07.tar.bz2
wget --no-check-certificate http://busybox.net/downloads/busybox-1.22.1.tar.bz2
wget http://sethwklein.net/iana-etc-2.30.tar.bz2
wget http://www.mirrorservice.org/pub/OpenBSD/OpenSSH/portable/openssh-6.7p1.tar.gz
wget http://uk1.php.net/get/php-5.6.5.tar.gz/from/this/mirror
mv mirror php-5.6.5.tar.gz
wget http://www.sqlite.org/2015/sqlite-autoconf-3080801.tar.gz
wget ftp://xmlsoft.org/libxml2/libxml2-2.9.2.tar.gz
wget http://mirror.catn.com/pub/apache/httpd/httpd-2.4.18.tar.gz
wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-8.38.tar.gz
wget http://mirror.catn.com/pub/apache/apr/apr-util-1.5.4.tar.gz
wget http://mirror.catn.com/pub/apache/apr/apr-1.5.2.tar.bz2
wget http://rsync.samba.org/ftp/rsync/src/rsync-3.1.1.tar.gz
wget -4 http://mosquitto.org/files/source/mosquitto-1.3.5.tar.gz
fi

tar -xf u-boot-2014.07.tar.bz2
tar -xf busybox-1.22.1.tar.bz2
tar -xf iana-etc-2.30.tar.bz2
tar -xf openssh-6.7p1.tar.gz
tar -xf php-5.6.5.tar.gz
tar -xf sqlite-autoconf-3080801.tar.gz
tar -xf libxml2-2.9.2.tar.gz
tar -xf httpd-2.4.18.tar.gz
tar -xf pcre-8.38.tar.gz
tar -xf apr-util-1.5.4.tar.gz
tar -xf apr-1.5.2.tar.bz2
tar -xf rsync-3.1.1.tar.gz
tar -xf mosquitto-1.3.5.tar.gz

rm u-boot-2014.07.tar.bz2
rm busybox-1.22.1.tar.bz2
rm iana-etc-2.30.tar.bz2
rm openssh-6.7p1.tar.gz
rm php-5.6.5.tar.gz
rm sqlite-autoconf-3080801.tar.gz
rm libxml2-2.9.2.tar.gz
rm httpd-2.4.18.tar.gz
rm pcre-8.38.tar.gz
rm apr-util-1.5.4.tar.gz
rm apr-1.5.2.tar.bz2
rm rsync-3.1.1.tar.gz
rm mosquitto-1.3.5.tar.gz


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
echo "copying linux source files from toolchain build."
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

sleep 1
sed -i -e 's|/\* #include "am335x-bone-spi0-spidev.dtsi" \*/|#include \"am335x-bone-spi0-spidev.dtsi\"|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|#include "am335x-ttyO1.dtsi"|/\* #include "am335x-ttyO1.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|/\* #include "am335x-bone-ttyO1.dtsi" \*/|#include "am335x-bone-ttyO1.dtsi"|g' arch/arm/boot/dts/am335x-boneblack.dts
#sed -i -e 's|#include "am335x-boneblack-nxp-hdmi-audio.dtsi"|/\* #include "am335x-boneblack-nxp-hdmi-audio.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|#include "am335x-can0.dtsi"|/\* #include "am335x-can0.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|#include "am335x-can1.dtsi"|/\* #include "am335x-can1.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|#include "am335x-bone-i2c2-cape-eeprom.dtsi"|/\* #include "am335x-bone-i2c2-cape-eeprom.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|#include "am335x-ttyO2.dtsi"|/\* #include "am335x-ttyO2.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|#include "am335x-ttyO4.dtsi"|/\* #include "am335x-ttyO4.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
sed -i -e 's|#include "am335x-ttyO5.dtsi"|/\* #include "am335x-ttyO5.dtsi" \*/|g' arch/arm/boot/dts/am335x-boneblack.dts
make uImage dtbs LOADADDR=0x80008000
make modules
make INSTALL_MOD_PATH=$TARGETFS modules_install

cp arch/arm/boot/uImage $TARGETFS/boot/uImage
cp arch/arm/boot/zImage $TARGETFS/boot/zImage

cp arch/arm/boot/dts/am335x-boneblack.dtb $TARGETFS/boot/am335x-boneblack.dtb

# we want to make it so that the spitest exe is copied too
cd Documentation/spi
$CC spidev_test.c -o spitest
cp spitest ${TARGETFS}/bin/


cp ${DIR}/resources/uEnv.txt.nfs ${TARGETFS}/boot/
cp ${DIR}/resources/uEnv.txt.local ${TARGETFS}/boot/
if [ $NETBOOT == 'TRUE' ]; then
  cp ${TARGETFS}/boot/uEnv.txt.nfs ${TARGETFS}/boot/uEnv.txt
else
  cp ${TARGETFS}/boot/uEnv.txt.local ${TARGETFS}/boot/uEnv.txt
fi

cd $SRCDIR/busybox-1.22.1
make defconfig
sleep 1
sed -i -e 's|CONFIG_FEATURE_INETD_RPC=y|# CONFIG_FEATURE_INETD_RPC is not set|g' .config
#remove rpc support as cannot build with against new versions of glibc
#Networking Utilities --> (DISABLE) inetd/Support RPC services
#CONFIG_PREFIX sets the install directory
make CONFIG_PREFIX="${TARGETFS}"
make CONFIG_PREFIX="${TARGETFS}" install
chmod +x ${TARGETFS}/bin/busybox

cp examples/depmod.pl ${BUILDTOOLSYSDIR}/bin/depmod.pl
chmod 755 ${BUILDTOOLSYSDIR}/bin/depmod.pl

cd ${SRCDIR}/iana-etc-2.30
make get
make STRIP=yes
make DESTDIR=$TARGETFS install

cp ${DIR}/resources/fstab.nfs ${TARGETFS}/etc/fstab.nfs
cp ${DIR}/resources/fstab.local ${TARGETFS}/etc/fstab.local
if [ $NETBOOT == 'TRUE' ]; then
  cp ${TARGETFS}/etc/fstab.nfs ${TARGETFS}/etc/fstab
else
  cp ${TARGETFS}/etc/fstab.local ${TARGETFS}/etc/fstab
fi
cp ${DIR}/resources/mdev.conf ${TARGETFS}/etc/mdev.conf
cp ${DIR}/resources/profile ${TARGETFS}/etc/profile
cp ${DIR}/resources/inittab ${TARGETFS}/etc/inittab
cp ${DIR}/resources/hosts ${TARGETFS}/etc/hosts
cp ${DIR}/resources/shells ${TARGETFS}/etc/shells

cp ${DIR}/resources/rc.shutdown ${TARGETFS}/etc/rc.shutdown

cp ${DIR}/resources/rc.sysinit.nfs ${TARGETFS}/etc/
cp ${DIR}/resources/rc.sysinit.local ${TARGETFS}/etc/
if [ $NETBOOT == 'TRUE' ]; then
 cp ${TARGETFS}/etc/rc.sysinit.nfs ${TARGETFS}/etc/rc.sysinit
else
  cp ${TARGETFS}/etc/rc.sysinit.local ${TARGETFS}/etc/rc.sysinit
fi



chmod +x ${TARGETFS}/etc/rc.sysinit
chmod +x ${TARGETFS}/etc/rc.shutdown

mkdir ${TARGETFS}/etc/udhcpc
cp ${DIR}/resources/default.script $TARGETFS/etc/udhcpc/
chmod +x ${TARGETFS}/etc/udhcpc/default.script

echo "beaglebone" > ${TARGETFS}/etc/HOSTNAME

cp ${DIR}/resources/resolv.conf ${TARGETFS}/etc/resolv.conf

mkdir $TARGETFS/etc/init.d

cp ${DIR}/resources/sshd ${TARGETFS}/etc/init.d/sshd
cp ${DIR}/resources/syslog ${TARGETFS}/etc/init.d/syslog
cp ${DIR}/resources/mosquitto ${TARGETFS}/etc/init.d/mosquitto
cp ${DIR}/resources/apache24 ${TARGETFS}/etc/init.d/apache24
cp ${DIR}/resources/rsmb ${TARGETFS}/etc/init.d/rsmb
cp ${DIR}/resources/wirelessBridge ${TARGETFS}/etc/init.d/wirelessBridge
cp ${DIR}/resources/logiclayer ${TARGETFS}/etc/init.d/logiclayer

chmod +x ${TARGETFS}/etc/init.d/sshd
chmod +x ${TARGETFS}/etc/init.d/syslog
chmod +x ${TARGETFS}/etc/init.d/mosquitto
chmod +x ${TARGETFS}/etc/init.d/apache24
chmod +x ${TARGETFS}/etc/init.d/rsmb
chmod +x ${TARGETFS}/etc/init.d/wirelessBridge
chmod +x ${TARGETFS}/etc/init.d/logiclayer




cp ${DIR}/resources/functions ${TARGETFS}/etc/init.d/functions

mkdir $TARGETFS/etc/rcS.d

#//install  sshd function and also syslog etc

#link scripts
cd ${TARGETFS}/etc/rcS.d
ln -sfv ../init.d/syslog S05syslog
ln -sfv ../init.d/sshd S30sshd
#mosquitto is not the primary mqtt broker anymore
#ln -sfv ../init.d/mosquitto S40mosquitto
ln -sfv ../init.d/rsmb S40rsmb
ln -sfv ../init.d/apache24 S50apache24
#temporarily disable for development
ln -sfv ../init.d/wirelessBridge S60wirelessBridge
ln -sfv ../init.d/logiclayer S55logiclayer

cd $SRCDIR/openssh-6.7p1
sed -i -e 's|#PermitEmptyPasswords no|PermitEmptyPasswords yes|g' sshd_config
./configure --host=$TARGET --with-libs --with-zlib=${BUILDTOOLSYSDIR}/sysroot --prefix=/ --with-ssl-dir=${BUILDTOOLSYSDIR}/sysroot LD=$TARGET-gcc AR=$TARGET-ar  --sysconfdir=/etc/ssh 
LD=$TARGET-gcc AR=$TARGET-ar make
make install-nokeys LD=$TARGET-gcc AR=$TARGET-ar DESTDIR=${TARGETFS} STRIP_OPT="-s --strip-program=arm-unknown-linux-gnueabihf-strip"

cd $SRCDIR/mosquitto-1.3.5
##maybe change prefix install dir as installs in /usr/local/ this is at the bottom of the config.mk
make
make install prefix='' DESTDIR=$TARGETFS
make install prefix='' DESTDIR=${BUILDTOOLSYSDIR}/sysroot
cp $TARGETFS/etc/mosquitto/mosquitto.conf.example $TARGETFS/etc/mosquitto/mosquitto.conf

sed -i -e 's|#pid_file|pid_file /var/run/mosquitto.pid|g' $TARGETFS/etc/mosquitto/mosquitto.conf 


cd ${SRCDIR}
git clone https://git.eclipse.org/r/mosquitto/org.eclipse.mosquitto.rsmb rsmb
cd rsmb/rsmb/src
GCC=$CC make
cp broker_mqtts ${TARGETFS}/bin/broker_mqtts
cp ${DIR}/resources/broker.cfg ${TARGETFS}/usr/bin/broker.cfg
cp Messages.1.3.0.2 ${TARGETFS}/usr/bin/Messages.1.3.0.2

cd $SRCDIR/rsync-3.1.1
./configure --host=$TARGET --prefix=/
make
make install prefix=/ DESTDIR=$TARGETFS

#APR
cd $SRCDIR/apr-1.5.2
#may have to be installed in src dir.
#have not tested apr_cv_tcp_nodelay_with_cork
./configure --host=$TARGET --prefix=$SRCDIR/apache/apr-build ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_tcp_nodelay_with_cork=no ac_cv_sizeof_struct_iovec=1
#copy the output from the test program as we can't run it on the build machine
cp ${DIR}/resources/apr_escape_test_char.h include/private/
#comment out as we have included the file from the test machine

sed -i -e 's|include/private/apr_escape_test_char.h: tools/gen_test_char|#include/private/apr_escape_test_char.h: tools/gen_test_char|g' Makefile
sed -i -e 's|$(APR_MKDIR) include/private|#$(APR_MKDIR) include/private|g' Makefile
sed -i -e 's|tools/gen_test_char > $@|#tools/gen_test_char > $@|g' Makefile
sed -i -e 's|LINK_PROG = $(LIBTOOL) $(LTFLAGS) --mode=link $(COMPILE) $(LT_LDFLAGS)|#LINK_PROG = $(LIBTOOL) $(LTFLAGS) --mode=link $(COMPILE) $(LT_LDFLAGS)|g' Makefile
sed -i -e 's|-no-install $(ALL_LDFLAGS) -o $@|#-no-install $(ALL_LDFLAGS) -o $@|g' Makefile

make
make install
cp -v $SRCDIR/apache/apr-build/lib/*.so* $TARGETFS/lib/

cd ${SRCDIR}/apr-util-1.5.4
./configure --host=${TARGET} --prefix=${SRCDIR}/apache/apr-util-build --with-apr=${SRCDIR}/apache/apr-build
make
make install
cp -v ${SRCDIR}/apache/apr-util-build/lib/*.so* $TARGETFS/lib/

cd ${SRCDIR}/pcre-8.38
./configure --host=${TARGET} --prefix=${SRCDIR}/apache/pcre-build
make
make install
cp -v ${SRCDIR}/apache/pcre-build/lib/*.so* $TARGETFS/lib/

cd ${SRCDIR}/httpd-2.4.18
#set the default user
sed -i -e 's/User daemon/User httpd/g' ${SRCDIR}/httpd-2.4.18/docs/conf/httpd.conf.in
sed -i -e 's/Group daemon/Group httpd/g' docs/conf/httpd.conf.in
./configure --prefix=/apache24 --host=${TARGET} --with-apr=${SRCDIR}/apache/apr-build --with-apr-util=${SRCDIR}/apache/apr-util-build  --with-pcre=${SRCDIR}/apache/pcre-build ap_cv_void_ptr_lt_long=no --with-mpm=prefork  --sysconfdir=/apache24/etc
#this will fail, but it must be ran to allow the below replacement to take place. ||true ensures that the broken make will not halt the buildscript
make || true
sleep 1
make || true
echo "copying premade test_char.h file"
cp -v ${DIR}/resources/test_char.h server/test_char.h
sleep 1
make
make DESTDIR=${TARGETFS} install

#need to add libraries from apr, apr-util, pcre
cp -vP /${BUILDTOOLSYSDIR}/sysroot/lib/*.so* ${TARGETFS}/lib/
cp -vP /${BUILDTOOLSYSDIR}/arm-unknown-linux-gnueabihf/lib/*.so* ${TARGETFS}/lib/

cd $SRCDIR/libxml2-2.9.2
./configure --host=$TARGET --without-python --prefix='' --without-lzma --with-zlib=${BUILDTOOLSYSDIR}/sysroot
make
make  DESTDIR=${BUILDTOOLSYSDIR}/sysroot install
make  DESTDIR=${TARGETFS} install

#need to install sqlite
cd $SRCDIR/sqlite-autoconf-3080801
./configure --host=$TARGET --prefix=''
make
make DESTDIR=${BUILDTOOLSYSDIR}/sysroot install
make  DESTDIR=${TARGETFS} install

cd $SRCDIR/php-5.6.5
#need to test
sed -i -e 's|my $installbuilddir = "/apache24/build";|my $installbuilddir = "'"${TARGETFS}"'/apache24/build";|g' ${TARGETFS}/apache24/bin/apxs
sed -i -e 's|includedir = ${prefix}/include|includedir = '"${TARGETFS}"'/apache24/include|g' ${TARGETFS}/apache24/build/config_vars.mk
#nano ${TARGETFS}/apache24/bin/apxs
./configure --host=$TARGET --prefix='' --with-libxml-dir=${BUILDTOOLSYSDIR}/sysroot --with-sqlite3 --enable-pdo --enable-json --with-pdo-sqlite --disable-all --with-apxs2=${TARGETFS}/apache24/bin/apxs --enable-session
LDFLAGS='-ldl' make
make INSTALL_ROOT=${TARGETFS} install
#modify the incorrect location of the php module
#${TARGETFS}/apache24/modules/libphp5.so
#to
#modules/libphp5.so
#using | as a delimeter so as not to confuse the escaping sequence
sed -i -e 's|'"${TARGETFS}"'/apache24/||g' $TARGETFS/apache24/etc/httpd.conf
sed -i -e 's|DirectoryIndex index.html|DirectoryIndex index.html index.xhtml index.php|g' $TARGETFS/apache24/etc/httpd.conf
#todo: this sed command needs to only replace the first occurence.
sed -i -e '1,/AllowOverride None/s/AllowOverride None/AllowOverride All/' $TARGETFS/apache24/etc/httpd.conf
sed -i -e 's|#LoadModule rewrite_module modules/mod_rewrite.so|LoadModule rewrite_module modules/mod_rewrite.so|g' $TARGETFS/apache24/etc/httpd.conf
echo "<FilesMatch \.php$>
    SetHandler application/x-httpd-php
</FilesMatch>" >> $TARGETFS/apache24/etc/httpd.conf
echo "PidFile /var/run/apache24.pid" >> $TARGETFS/apache24/etc/httpd.conf

#http://www.tldp.org/LDP/Linux-Filesystem-Hierarchy/html/var.html
#make db directory and allow access to all
mkdir $TARGETFS/var/db
chmod -R 777 $TARGETFS/var/db
#// need to make sure /usr/share has zone info.
#http://www.linuxfromscratch.org/lfs/view/stable/chapter06/glibc.html
mkdir $SRCDIR/tz
cd $SRCDIR/tz
wget http://www.iana.org/time-zones/repository/releases/tzdata2015b.tar.gz
tar -xf tzdata2015b.tar.gz
rm tzdata2015b.tar.gz
export ZONEINFO=${TARGETFS}/usr/share/zoneinfo
mkdir -pv $ZONEINFO/{posix,right}
for tz in etcetera southamerica northamerica europe africa antarctica  \
          asia australasia backward pacificnew systemv; do
    zic -L /dev/null   -d $ZONEINFO       -y "sh yearistype.sh" ${tz}
    zic -L /dev/null   -d $ZONEINFO/posix -y "sh yearistype.sh" ${tz}
    zic -L leapseconds -d $ZONEINFO/right -y "sh yearistype.sh" ${tz}
done
cp -v zone.tab zone1970.tab iso3166.tab $ZONEINFO
zic -d $ZONEINFO -p America/New_York
unset ZONEINFO


cp -v $TARGETFS/usr/share/zoneinfo/Europe/London $TARGETFS/etc/localtime

#install the WWW data from the git repo to the filesystem and remove the old apachefile
rm -Rv ${TARGETFS}/apache24/htdocs
cp -Rv $DIR/../www $TARGETFS/apache24/htdocs
echo "WWW Files Copied"

#build the apps and copy them over
cd ${DIR}/../Apps/LogicLayer
make CXX=${CXX}
make CXX=${CXX} DESTDIR=${TARGETFS} install

cd ${DIR}/../Apps/Wireless-NRF
make CXX=${CXX}
make CXX=${CXX} DESTDIR=${TARGETFS} install

cd ${DIR}/../Apps/SHScheduler
make CXX=${CXX}
make CXX=${CXX} DESTDIR=${TARGETFS} install



#create a crontab to call the scheduler
install -dv ${TARGETFS}/var/spool/cron/crontabs
echo "*/5 * * * * /usr/bin/shscheduler" > ${TARGETFS}/var/spool/cron/crontabs/root


cd ${TARGETFS}
echo "packaging generated Filesystem"
tar jcf ../fs.tar.bz2 *
cd ${TARGETFS}/..
rm -R ${SRCDIR}
rm -R ${TARGETFS}
ENDTIME=$(date +%s)
echo "####################################################################"
echo "####################################################################"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                           COMPLETED SUCESSFULLY                  #"
echo "#                      INSTALLED TO ${TARGETFS}            #"
echo "#                                took $((($ENDTIME - $STARTTIME)/60)) mins             #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "#                                                                  #"
echo "####################################################################"
echo "####################################################################"

