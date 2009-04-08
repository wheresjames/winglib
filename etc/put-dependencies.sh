#!/bin/bash -xv

#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

FILE_EXT=tar.bz2
#FILE_EXT=zip
INSTALL_ROOT=$PWD/../..
UPLOADS=$PWD/../../../uploads

mkdir -p ${UPLOADS}
mkdir -p ${INSTALL_ROOT}

#-------------------------------------------------------------------
# Execute
#-------------------------------------------------------------------

for PROJECT in `cat dependencies.txt`
do
	echo ${INSTALL_ROOT}/${PROJECT}

	# If the directory exists
	if [ -d ${INSTALL_ROOT}/${PROJECT} ]; then

		FILE="${UPLOADS}/${PROJECT}.${FILE_EXT}"
		
	    echo Creating ${FILE}...

		tar -c ${INSTALL_ROOT}/${PROJECT} | bzip2 > ${FILE}

	fi	
done


