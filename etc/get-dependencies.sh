#!/bin/bash -xv
#===================================================================
# Build Toolchain
#===================================================================
#
# License    : Public Domain
# Author     : Robert Umbehant <rumbehant@wheresjames.com>
#
# +---- [downloads]  (Downloaded files will be copied here)
# |
# +---- <lib a, lib b, ...>	 (Libraries will be installed here)
# |
# +---- [etc]        (Put the script and patches here, the directory name is not important)
#       |
#       +---- get_dependencies.sh
#       |
#       +---- [patches]
#             |
#             +---- <lib>-my_patch.patch
#

#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

FILE_EXT=tar.bz2
#FILE_EXT=zip
PATCHES=$PWD/patches
INSTALL_ROOT=$PWD/../..
DOWNLOADS=$PWD/../../downloads
SF_SERVER=superb-east
#SF_SERVER=superb-west
#SF_SERVER=internap
#SF_SERVER=voxel

mkdir -p ${DOWNLOADS}
mkdir -p ${INSTALL_ROOT}

#-------------------------------------------------------------------
# Get the sources
#-------------------------------------------------------------------
#	"http://${SF_SERVER}.dl.sourceforge.net/sourceforge/boost/boost_1_38_0.tar.bz2" \
#	"http://${SF_SERVER}.dl.sourceforge.net/sourceforge/cximage/cximage599c_lite.zip" \
#	"http://${SF_SERVER}.dl.sourceforge.net/sourceforge/irrlicht/irrlicht-1.5.zip" \

for PROJECT in abel cell CxImage gdchart irrlicht j2k jpeg opencv png SqPlus tiff zlib
do
	echo ${INSTALL_ROOT}/${PROJECT}
	
	# Only install if it doesn't already exist
	if [ ! -d ${INSTALL_ROOT}/${PROJECT} ]; then
	
		URL="http://www.wetcoin.com/etc/${PROJECT}.${FILE_EXT}"
		FILE="${DOWNLOADS}/${PROJECT}.${FILE_EXT}"

	    echo Downloading ${FILE}...
	    [ -f ${FILE} ] || wget -O ${FILE} ${URL}
	    [ -s ${FILE} ] || rm ${FILE}
	
		cd ${INSTALL_ROOT}
	    echo Extracting ${PROJECT}...
	    if [ ${FILE_EXT} = "zip" ]; then
	    	unzip -n ${FILE}
	    else
			bunzip2 -c ${FILE} | tar xvf -
	    fi
	    echo Patching ${PROJECT}...
		find ${PATCHES} -name ${PROJECT}-*.patch | xargs -rtI {} cat {} | patch -d ${PROJECT} -p1	
	fi	
done


