#!/bin/bash -xv

#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

FILE_EXT=tar.bz2
#FILE_EXT=zip
DEPENDS=${PWD}/dependencies.txt
INSTALL_ROOT=$PWD/../..
UPLOADS=$PWD/../../../uploads

mkdir -p ${UPLOADS}

#-------------------------------------------------------------------
# Execute
#-------------------------------------------------------------------

cd ${INSTALL_ROOT}

for PROJECT in `cat ${DEPENDS}`
do
	echo ${INSTALL_ROOT}/${PROJECT}

	# If the directory exists
	if [ -d ${INSTALL_ROOT}/${PROJECT} ]; then

		FILE="${UPLOADS}/${PROJECT}"
		
	    echo Creating ${FILE}...

		tar -c ${PROJECT} | bzip2 > "${FILE}.tar.bz2"
		
		zip -r "${FILE}.zip" ${PROJECT}

	fi	
done


