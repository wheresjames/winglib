#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

set( project_name 	test )

# if debug build
#set( CMAKE_BUILD_TYPE debug )
#add_definitions( -D_DEBUG -DDEBUG )

set( output_dir 	../../bin )
#add_definitions( -DUNICODE -D_UNICODE )

set( src_files
					../../stdafx.cpp
					../../test.cpp
   )

set( header_files
					../../stdafx.h
   )

#-------------------------------------------------------------------
# Init
#-------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED( VERSION 2.0 )

set( CMAKE_DEBUG_POSTFIX  "_d" )
if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
	set( LIB_POSTFIX "64" )
endif( CMAKE_SIZEOF_VOID_P MATCHES "8" )

project( ${project_name} )

#-------------------------------------------------------------------
# Packages
#-------------------------------------------------------------------

#find_package( Threads )
#find_package( OpenGL )
#find_package( JPEG )

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------

FILE( MAKE_DIRECTORY ${output_dir} )

set( LIBRARY_OUTPUT_PATH ${output_dir} )
set( EXECUTABLE_OUTPUT_PATH ${output_dir} )

# Add header files to group
source_group( "Header Files" FILES ${header_files} )

# Add source files to group
source_group( "Source Files" FILES ${src_files} )

if( WIN32 )
	add_executable( ${project_name} ${src_files} ${header_files} )
else( WIN32 )
	add_executable( ${project_name} ${src_files} ${header_files} )
endif( WIN32 )

#-------------------------------------------------------------------
# build
#-------------------------------------------------------------------

#add_library( imp_rt STATIC IMPORTED )
#set_property( TARGET imp_rt PROPERTY
#              IMPORTED_LOCATION /usr/lib/librt.a )

target_link_libraries(
						${project_name}
						${CMAKE_CURRENT_BINARY_DIR}/../../../../lib/oexlib/bin/liboexlib_d.a
						/usr/lib/librt.a
					  )
