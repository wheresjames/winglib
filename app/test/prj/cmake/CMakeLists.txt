
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

set( CMAKE_DEBUG_POSTFIX  "_d" )
if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
	set( LIB_POSTFIX "64" )
endif( CMAKE_SIZEOF_VOID_P MATCHES "8" )

#-------------------------------------------------------------------
# Project settings
#-------------------------------------------------------------------

 set( PROJECT_NAME "test" )
 set( OUTPUT_FOLDER "../../bin" ) 
 
#-------------------------------------------------------------------
# Source Files
#-------------------------------------------------------------------

set( src_files
				../../test.cpp 
				../../stdafx.cpp 
   )
	   
#-------------------------------------------------------------------
# Header Files
#-------------------------------------------------------------------

set( hdr_files
				../../stdafx.h
   )

#-------------------------------------------------------------------
# build
#-------------------------------------------------------------------

project ( ${PROJECT_NAME} )

FILE( MAKE_DIRECTORY ${OUTPUT_FOLDER} )

set( LIBRARY_OUTPUT_PATH ${OUTPUT_FOLDER} )
set( EXECUTABLE_OUTPUT_PATH ${OUTPUT_FOLDER} )

# Add header files to group
source_group( "Header Files" FILES ${hdr_files} )

# Add source files to group
source_group( "Source Files" FILES ${src_files} )

# Set project
add_executable( ${PROJECT_NAME} ${src_files} ${hdr_files} )

# What to build
target_link_libraries( ${PROJECT_NAME} ../../../../lib/oexlib/bin/oexlib_d )

							 

