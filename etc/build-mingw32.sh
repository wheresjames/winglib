#!/bin/bash -xv
#===================================================================
# Build mingw32
#===================================================================
#
# License     :	Public Domain
# Author      :	Robert Umbehant <rumbehant@wheresjames.com>
#
# +---- [downloads]  (Downloaded files will be copied here)
# |
# +---- [tools]      (Toolchain will be built here)
# |
# +---- [etc]        (Put the script and patches here, the directory name is not important)
#       |
#       +---- build-toolchain.sh
#       |
#       +---- [patches]
#             |
#             +---- <lib>-my_patch.patch
#             |
#             +---- linux-cfg-<my_linux_config>
#

#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

VER_BINUTILS=binutils-2.19.1
FILE_BINUTILS=binutils-2.19.1-src.tar.gz

VER_GCC=gcc-3.4.5-20060117-2
FILE_GCC=gcc-core-3.4.5-20060117-2-src.tar.gz
FILE_GPP=gcc-g++-3.4.5-20060117-2-src.tar.gz

VER_MINGW32=mingwrt-3.15.2-mingw32
FILE_MINGW32=mingwrt-3.15.2-mingw32-src.tar.gz

VER_MINGWAPI=w32api-3.13-mingw32
FILE_MINGWAPI=w32api-3.13-mingw32-src.tar.gz


#-------------------------------------------------------------------

TARGET=i386-mingw32
BUILD=i686-pc-linux-gnu
HOST=i686-pc-linux-gnu
SCRIPT_DIR=$PWD
PATCHES=$PWD/patches
PREFIX=$PWD/../../../tools/${TARGET}
DOWNLOADS=$PWD/../../../downloads
SYSROOT=${PREFIX}/sysroot
BUILDROOT=${PREFIX}/src
export CROSS_COMPILE=${PREFIX}/bin/${TARGET}-
export PATH=$PATH:${PREFIX}/bin
mkdir -p ${BUILDROOT}
mkdir -p ${BUILDROOT}/logs
mkdir -p ${DOWNLOADS}
mkdir -p ${PREFIX}/${TARGET}
mkdir -p ${SYSROOT}

# Set to 1 to verify signatures, (you must have the public keys)
VERIFY_SIGS=0
ARCHIVE_TYPE=tar.gz

#-------------------------------------------------------------------
# Get the sources
#-------------------------------------------------------------------
#	"http://hivelocity.dl.sourceforge.net/sourceforge/mingw/binutils-2.19.1-src.tar.gz" \
#	"http://ftp.gnu.org/gnu/binutils/${VER_BINUTILS}.tar.bz2" \
#	"http://ftp.gnu.org/gnu/gcc/${VER_GCC}/${VER_GCC}.tar.bz2" \

REPO_SERVER="http://hivelocity.dl.sourceforge.net/sourceforge/mingw/"

cd ${BUILDROOT}
for URL in \
	"${REPO_SERVER}${FILE_BINUTILS}" \
	"${REPO_SERVER}${FILE_GCC}" \
	"${REPO_SERVER}${FILE_GPP}" \
	"${REPO_SERVER}${FILE_MINGW32}" \
	"${REPO_SERVER}${FILE_MINGWAPI}"
do
    FILE=${URL##*/}
    FILE=${DOWNLOADS}/${FILE%%\?*}
    echo Downloading ${FILE}
    [ -f ${FILE} ] || wget -O ${FILE} ${URL}
    [ -s ${FILE} ] || rm ${FILE}
    
    if [ ${VERIFY_SIGS} -gt 0 ]; then
		[ -s ${FILE}.sig ] || wget -O ${FILE}.sig ${URL}.sig
		[ -s ${FILE}.sig ] || wget -O ${FILE}.sig ${URL}.sign
		gpg --verify ${FILE}.sig ${FILE}
		if [ $? -ne 0 ]; then
			exit 1
		fi    
	fi
done

# Extract windows headers
cd ${PREFIX}/${TARGET}
gunzip -c ${DOWNLOADS}/${FILE_MINGW32} | tar xvf -
cp -rf ${VER_MINGW32}/* ./
rm -Rf ${VER_MINGW32}

gunzip -c ${DOWNLOADS}/${FILE_MINGWAPI} | tar xvf -
cp -rf ${VER_MINGWAPI}/* ./
rm -Rf ${VER_MINGWAPI}

#-------------------------------------------------------------------
# 1. GNU binutils
#-------------------------------------------------------------------
cd ${BUILDROOT}
if [ ! -d ${VER_BINUTILS} ]; then 
	gunzip -c ${DOWNLOADS}/${FILE_BINUTILS} | tar xvf -
	find ${PATCHES} -name ${VER_BINUTILS}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_BINUTILS} -p1
	mkdir -p BUILD/${VER_BINUTILS}
	cd BUILD/${VER_BINUTILS}
	../../${VER_BINUTILS}/configure --prefix=${PREFIX} \
		                            --target=${TARGET} \
	                                2>&1 | tee -a ${BUILDROOT}/logs/10.${VER_BINUTILS}.configure.log
	make 2>&1 | tee -a ${BUILDROOT}/logs/11.${VER_BINUTILS}.make.log
	make install 2>&1 | tee -a ${BUILDROOT}/logs/12.${VER_BINUTILS}.make.install.log
	cp -v include/libiberty.h ${SYSROOT}/usr/include
	[ -z "${TOOL_ALIAS}" ] || sh ${SCRIPT_DIR}/link-tools.sh ${PREFIX}/bin/${TARGET} ${PREFIX}/bin/${TOOL_ALIAS} 2>&1 | tee -a ${BUILDROOT}/logs/13.${VER_BINUTILS}.link-gcc.sh.log
fi

#-------------------------------------------------------------------
# 2. gcc
#-------------------------------------------------------------------
cd ${BUILDROOT}

[ -d ${VER_GCC} ] || gunzip -c ${DOWNLOADS}/${FILE_GCC} | tar xvf -
[ -d ${VER_GPP} ] || gunzip -c ${DOWNLOADS}/${FILE_GPP} | tar xvf -
find ${PATCHES} -name ${VER_GCC}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_GCC} -p1
mkdir -p BUILD/${VER_GCC}
cd BUILD/${VER_GCC}

../../${VER_GCC}/configure --prefix=${PREFIX} \
                           --target=${TARGET} \
						   2>&1 | tee ${BUILDROOT}/logs/21.${VER_GCC}.configure.stage1.log
make 2>&1 | tee ${BUILDROOT}/logs/22.${VER_GCC}.make.stage1.log
make install 2>&1 | tee -a ${BUILDROOT}/logs/23.${VER_GCC}.make.stage1.install.log
[ -z "${TOOL_ALIAS}" ] || sh ${SCRIPT_DIR}/link-tools.sh ${PREFIX}/bin/${TARGET} ${PREFIX}/bin/${TOOL_ALIAS} 2>&1 | tee -a ${BUILDROOT}/logs/24.${VER_GCC}.link-gcc.sh.log


