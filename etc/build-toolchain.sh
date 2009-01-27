#!/bin/bash -xv
#===================================================================
# Build Toolchain
#===================================================================
#
# License    : Public Domain
# Author     : Robert Umbehant <rumbehant@wheresjames.com>
# References :
#              http://frank.harvard.edu/~coldwell/toolchain/
#              http://sung2ne.tistory.com/entry/Building-a-GNULinux-ARM-Toolchain-from-scratch
#              http://wiki.davincidsp.com/index.php?title=Linux_Toolchain
#
#     DON'T expect this script to work perfectly for you.
#     DO Expect to tweak things a bit to get a build on your box.
#
# If you use this script correctly, it will install your patches
# and linux config.  Setup you're directory to match what you see
# below.  Set CFG_LINUX to the name of your config file.  For the
# example below, CFG_LINUX=<my_linux_config>  The actual file must
# begin with linux-cfg-  It will search for and apply all patches
# matching the version striong.  If VER_GCC=gcc-3.4.4, all patches
# matching gcc-3.4.4-<whatev>.patch will be applied.
#
# <root>
# |
# +---- build-toolchain.sh
# |
# +---- [patches]
#       |
#       +---- <lib>-my_patch.patch
#       |
#       +---- linux-cfg-<my_linux_config>
#

#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

#************************************************
# An old version 
#************************************************
#VER_BINUTILS=binutils-2.16
#VER_GCC=gcc-3.4.4
#VER_GLIBC=glibc-2.3.5
#VER_GLIBC_THREADS=glibc-linuxthreads-2.3.5
#VER_LINUX=linux-2.6.10
#DIR_LINUX=v2.6
#CFG_LINUX=custom_davinci_all_defconfig

#************************************************
# Recent version 
#************************************************
#VER_BINUTILS=binutils-2.19
#VER_GCC=gcc-4.3.2
#VER_GLIBC=glibc-2.7
#VER_GLIBC_THREADS=glibc-linuxthreads-2.5
#VER_LINUX=linux-2.6.28
#DIR_LINUX=v2.6
#CFG_LINUX=custom_davinci_all_defconfig

#************************************************
# MontaVista build
#************************************************
VER_BINUTILS=binutils-2.16
VER_GCC=gcc-3.4.4
VER_GLIBC=glibc-2.3.5
VER_GLIBC_THREADS=glibc-linuxthreads-2.3.5
VER_LINUX=linux-davinci-2.6
DIR_LINUX=v2.6
CFG_LINUX=custom_davinci_all_defconfig
GIT_REPOS="git://source.mvista.com/git/linux-davinci-2.6.git"

#-------------------------------------------------------------------

export ARCH=arm
TARGET=${ARCH}-unknown-linux-gnu
PATCHES=$PWD/patches
PREFIX=$PWD/../../tools/${ARCH}-${VER_LINUX}
DOWNLOADS=$PWD/../../downloads
SYSROOT=${PREFIX}/sysroot
BUILDROOT=${PREFIX}/src
export CROSS_COMPILE=${TARGET}-
export PATH=$PATH:${PREFIX}/bin
mkdir -p ${BUILDROOT}
mkdir -p ${BUILDROOT}/logs
mkdir -p ${DOWNLOADS}

#-------------------------------------------------------------------
# Get the sources
#-------------------------------------------------------------------

# Set to 1 to verify signatures, (you must have the public keys)
VERIFY_SIGS=0

cd ${BUILDROOT}
for URL in \
	"http://ftp.gnu.org/gnu/binutils/${VER_BINUTILS}.tar.bz2" \
	"http://ftp.gnu.org/gnu/gcc/${VER_GCC}/${VER_GCC}.tar.bz2" \
	"http://ftp.gnu.org/gnu/glibc/${VER_GLIBC}.tar.bz2" \
	"http://ftp.gnu.org/gnu/glibc/${VER_GLIBC_THREADS}.tar.bz2" \
	"http://ftp.kernel.org/pub/linux/kernel/${DIR_LINUX}/${VER_LINUX}.tar.bz2"
do
    FILE=${URL##*/}
    FILE=${DOWNLOADS}/${FILE%%\?*}
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

for URL in ${GIT_REPOS}
do
    FILE=${URL##*/}
    FILE=${FILE%.*}
    FILE=${FILE%%\?*}
    if [ ! -d ${BUILDROOT}/${FILE} ]; then        
		[ -d ${DOWNLOADS}/${FILE} ] || git-clone ${URL} ${DOWNLOADS}/${FILE}
		cp -r ${DOWNLOADS}/${FILE} ${BUILDROOT}/${FILE}		
	fi
done

#-------------------------------------------------------------------
# 1. GNU binutils
#-------------------------------------------------------------------
cd ${BUILDROOT}
[ -f ${VER_BINUTILS} ] || bunzip2 -c ${DOWNLOADS}/${VER_BINUTILS}.tar.bz2 | tar xvf -
find ${PATCHES} -name ${VER_BINUTILS}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_BINUTILS} -p1
mkdir -p BUILD/${VER_BINUTILS}
cd BUILD/${VER_BINUTILS}
../../${VER_BINUTILS}/configure --prefix=${PREFIX} --target=${TARGET} --with-sysroot=${SYSROOT} 2>&1 | tee -a ${BUILDROOT}/logs/10.${VER_BINUTILS}.configure.log
make 2>&1 | tee -a ${BUILDROOT}/logs/11.${VER_BINUTILS}.make.log
make install 2>&1 | tee -a ${BUILDROOT}/logs/12.${VER_BINUTILS}.make.install.log

#-------------------------------------------------------------------
# 2. Linux Kernel Headers
#-------------------------------------------------------------------
cd ${BUILDROOT}
[ -f ${VER_LINUX} ] || bunzip2 -c ${DOWNLOADS}/${VER_LINUX}.tar.bz2 | tar xvf -
ln -s ${VER_LINUX} linux
find ${PATCHES} -name ${VER_LINUX}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_LINUX} -p1
[ -f ${PATCHES}/linux-cfg-${CFG_LINUX} ] && cp -a ${PATCHES}/linux-cfg-${CFG_LINUX} linux/arch/${ARCH}/configs/${CFG_LINUX}
cd linux
make ${CFG_LINUX} 2>&1 | tee -a ${BUILDROOT}/logs/20.${VER_LINUX}.make.${CFG_LINUX}.log
make include/linux/version.h 2>&1 | tee -a ${BUILDROOT}/logs/21.${VER_LINUX}.make.version.h.log
mkdir -p ${SYSROOT}/usr/include

#cp -a ${PREFIX}/src/linux/include/linux ${SYSROOT}/usr/include/linux
#cp -a ${PREFIX}/src/linux/include/asm-arm ${SYSROOT}/usr/include/asm
#cp -a ${PREFIX}/src/linux/include/asm-generic ${SYSROOT}/usr/include/asm-generic

cp -af ${PREFIX}/src/linux/arch/arm/include/asm/* ${SYSROOT}/usr/include/asm
cp -af ${PREFIX}/src/linux/arch/arm/mach-davinci/include/mach/* ${SYSROOT}/usr/include/asm

#-------------------------------------------------------------------
# 3. Glibc headers
#-------------------------------------------------------------------
cd ${BUILDROOT}
[ -f ${VER_GLIBC} ] || bunzip2 -c ${DOWNLOADS}/${VER_GLIBC}.tar.bz2 | tar xvf -
find ${PATCHES} -name ${VER_GLIBC}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_GLIBC} -p1
cd ${VER_GLIBC}
[ -f ${VER_GLIBC_THREADS} ] || bunzip2 -c ${DOWNLOADS}/${VER_GLIBC_THREADS}.tar.bz2 | tar xvf -
find ${PATCHES} -name ${VER_GLIBC_THREADS}-*.patch | xargs -rtI {} cat {} | patch -p0
cd ..
mkdir BUILD/${VER_GLIBC}-headers
cd BUILD/${VER_GLIBC}-headers
../../${VER_GLIBC}/configure --prefix=/usr --host=${TARGET} --enable-add-ons=linuxthreads --with-headers=${SYSROOT}/usr/include 2>&1 | tee ${BUILDROOT}/logs/30.${VER_GLIBC}.configure.headers.log
make cross-compiling=yes install_root=${SYSROOT} install-headers 2>&1 | tee ${BUILDROOT}/logs/31.${VER_GLIBC}.make.headers.log
touch ${SYSROOT}/usr/include/gnu/stubs.h
touch ${SYSROOT}/usr/include/bits/stdio_lim.h

#-------------------------------------------------------------------
# 4. Stage 1 gcc
#-------------------------------------------------------------------
cd ${BUILDROOT}
[ -f ${VER_GCC} ] || bunzip2 -c ${DOWNLOADS}/${VER_GCC}.tar.bz2 | tar xvf -
find ${PATCHES} -name ${VER_GCC}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_GCC} -p1
mkdir -p BUILD/${VER_GCC}-stage1
cd BUILD/${VER_GCC}-stage1
../../${VER_GCC}/configure --prefix=${PREFIX} --target=${TARGET} --enable-languages=c --with-sysroot=${SYSROOT} 2>&1 | tee ${BUILDROOT}/logs/40.${VER_GCC}.configure.stage1.log
make 2>&1 | tee ${BUILDROOT}/logs/41.${VER_GCC}.make.stage1.log
make install 2>&1 | tee -a ${BUILDROOT}/logs/42.${VER_GCC}.make.stage1.install.log

#-------------------------------------------------------------------
# 5. Build glibc
#-------------------------------------------------------------------
cd ${BUILDROOT}
mkdir BUILD/${VER_GLIBC}
cd BUILD/${VER_GLIBC}
BUILD_CC=gcc CC=${CROSS_COMPILE}gcc AR=${CROSS_COMPILE}ar RANLIB=${CROSS_COMPILE}ranlib AS=${CROSS_COMPILE}as LD=${CROSS_COMPILE}ld ../../${VER_GLIBC}/configure --prefix=/usr --build=i386-redhat-linux --host=${TARGET} --target=${TARGET} --without-__thread --enable-add-ons=linuxthreads --with-headers=${SYSROOT}/usr/include 2>&1 | tee ${BUILDROOT}/logs/50.${VER_GLIBC}.configure.log
make 2>&1 | tee ${BUILDROOT}/logs/51.${VER_GLIBC}.make.log
make install_root=${SYSROOT} install | tee -a ${BUILDROOT}/logs/52.${VER_GLIBC}.make.install.log

#-------------------------------------------------------------------
# 6. Stage 2 gcc
#-------------------------------------------------------------------
cd ${BUILDROOT}
mkdir BUILD/${VER_GCC}
cd BUILD/${VER_GCC}
../../${VER_GCC}/configure --prefix=${PREFIX} --target=${TARGET} --enable-languages=c,c++ --with-sysroot=${SYSROOT} 2>&1 | tee ${BUILDROOT}/logs/60.${VER_GCC}.configure.stage2.log
make 2>&1 | tee ${BUILDROOT}/logs/61.${VER_GCC}.make.stage2.log
make install 2>&1 | tee -a ${BUILDROOT}/logs/62.${VER_GCC}.make.stage2.install.log

#-------------------------------------------------------------------
# 7. Build Linux kernel
#-------------------------------------------------------------------
cd ${BUILDROOT}/linux
make zImage 2>&1 | tee -a ${BUILDROOT}/logs/70.${VER_LINUX}.make.uImage.log
make modules 2>&1 | tee -a ${BUILDROOT}/logs/71.${VER_LINUX}.make.modules.log
make INSTALL_MOD_PATH=${SYSROOT} modules_install 2>&1 | tee -a ${BUILDROOT}/logs/72.${VER_LINUX}.make.modules.install.log


