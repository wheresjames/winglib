#!/bin/bash -xv

#-------------------------------------------------------------------
# Get the sources
#-------------------------------------------------------------------
#	"http://${SF_SERVER}.dl.sourceforge.net/sourceforge/boost/boost_1_38_0.tar.bz2" \
#	"http://${SF_SERVER}.dl.sourceforge.net/sourceforge/cximage/cximage599c_lite.zip" \
#	"http://${SF_SERVER}.dl.sourceforge.net/sourceforge/irrlicht/irrlicht-1.5.zip" \

#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

FILE_EXT=tar.bz2
#FILE_EXT=zip
PATCHES=$PWD/patches
INSTALL_ROOT=$PWD/../..
DOWNLOADS=$PWD/../../../downloads
REPO=http://www.wetcoin.com/etc
SF_SERVER=superb-east
#SF_SERVER=superb-west
#SF_SERVER=internap
#SF_SERVER=voxel

mkdir -p ${DOWNLOADS}
mkdir -p ${INSTALL_ROOT}

#-------------------------------------------------------------------
# Execute
#-------------------------------------------------------------------

for PROJECT in `cat dependencies.txt`
do
	echo ${INSTALL_ROOT}/${PROJECT}
	
	# Only install if it doesn't already exist
	if [ ! -d ${INSTALL_ROOT}/${PROJECT} ]; then
	
		URL="${REPO}/${PROJECT}.${FILE_EXT}"
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


