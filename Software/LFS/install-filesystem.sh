#/bin/bash

set -e
export DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ./buildscript.sh

mkdir -pv $TARGETFS
cd $TARGETFS

ln -svf usr/bin  $TARGETFS/bin
ln -svf usr/sbin $TARGETFS/sbin
ln -svr usr/lib  $TARGETFS/lib


mkdir -pv  $TARGETFS/{bin,boot,dev,etc,home,lib/{firmware,modules}}
mkdir -pv  $TARGETFS/{mnt,opt,proc,sbin,srv,sys}
mkdir -pv  $TARGETFS/var/{cache,lib,local,lock,log,opt,run,spool}
install -dv -m 0750  $TARGETFS/root
install -dv -m 1777  $TARGETFS/tmp
mkdir -pv  $TARGETFS/usr/{,local/}{bin,include,lib,sbin,share,src}


ln -svf ../proc/mounts ${TARGETFS}/etc/mtab





cat > ${TARGETFS}/etc/passwd << "EOF"
root::0:0:root:/root:/bin/ash
mosquitto::100:100:mosquitto daemon::
sshd::101:101:SSH Daemon::
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
#Device Drivers > LED Support > LED Trigger Support > (DISABLE) LED Default ON Trigger
make menuconfig
make uImage dtbs LOADADDR=0x80008000
 
make modules
make INSTALL_MOD_PATH=$TARGETFS modules_install

cp arch/arm/boot/uImage $TARGETFS/boot/uImage
cp arch/arm/boot/zImage $TARGETFS/boot/zImage
cp arch/arm/boot/dts/am335x-boneblack.dtb $TARGETFS/boot/am335x-boneblack.dtb




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










cp -vP /${BUILDTOOLSYSDIR}/sysroot/lib/*.so* ${TARGETFS}/lib/
cp -vP /${BUILDTOOLSYSDIR}/arm-unknown-linux-gnueabihf/lib/*.so* ${TARGETFS}/lib/

#TODO:
#sort out sql install. at the moment it is a lot of hassle and can only be ran manually.
#the Device Tree Builder from Rober Nelson must be applied to kernel to get SPI working on the 3.14 kernel.
#
#
#wget https://downloads.mariadb.org/interstitial/mariadb-5.5.41/source/mariadb-5.5.41.tar.gz
#wget http://dev.mysql.com/get/Downloads/MySQL-5.6/mysql-5.6.20.tar.gz
#tar -xf mysql-5.6.20.tar.gz
#cd mysql-5.6.20
#install sql
#cd sql dir
#mkdir bld
#
#
#
#
#https://github.com/RobertCNelson/dtb-rebuilder/tree/3.14-ti
