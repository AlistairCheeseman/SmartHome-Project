#/bin/bash
export DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/config.sh


#the source directory we will be storing downloaded files
export SRCDIR=${ROOTDIR}/sources
#set the build tool location
export BUILDTOOLSYSDIR=${ROOTDIR}/tools/build

#update the path to include the compiled binary programs
export PATH=${PATH}:/${BUILDTOOLSYSDIR}/bin
#set the build machine architechure
export BUILD=$(echo ${MACHTYPE} | sed "s/-[^-]*/-cross/")

#the prefix that will be before all the cross compiled programs
export CROSS_COMPILE=${TARGET}-

export CC=${CROSS_COMPILE}gcc
export CXX=${CROSS_COMPILE}g++
export AR=${CROSS_COMPILE}ar
export AS=${CROSS_COMPILE}as
export LD=${CROSS_COMPILE}ld
export RANLIB=${CROSS_COMPILE}ranlib
export READELF=${CROSS_COMPILE}readelf
export STRIP=${CROSS_COMPILE}strip

export TARGETFS=${ROOTDIR}/targetfs2
