
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

#add_subdirectory( ../../src ../../bin/cmake )

#CMAKE_USE_RELATIVE_PATHS
#set( OUTPUT_BINDIR "../../bin" )

set( CMAKE_DEBUG_POSTFIX  "_d" )
if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
	set( LIB_POSTFIX "64" )
endif( CMAKE_SIZEOF_VOID_P MATCHES "8" )

#-------------------------------------------------------------------
# oexlib
#-------------------------------------------------------------------

project (oexlib)

FILE( MAKE_DIRECTORY ../../bin )

#set( OUTPUT_LIBDIR ../../bin )
set( LIBRARY_OUTPUT_PATH ../../bin )
#set( OUTPUT_BINDIR ../../bin )
set( EXECUTABLE_OUTPUT_PATH ../../bin )
#set( CMAKE_PREFIX_PATH ../../bin )

#include_directories( ../../inc )

#link_directories( ../../lib )


set( oexlib_src
				../../src/alloc.cpp 
				../../src/asso_list.cpp 
				../../src/circ_buf.cpp 
				../../src/data_packet.cpp 
				../../src/fifo_sync.cpp 
				../../src/file.cpp 
				../../src/find_files.cpp 
				../../src/md5.cpp 
				../../src/oex.cpp 
				../../src/parser.cpp 
				../../src/sys_time.cpp 
				../../src/unzip.cpp 
				../../src/zip.cpp 				
   )
		   
set( oexlib_os ../../src/os/win32 )		   
set( oexlib_os_src
				${oexlib_os}/base_file.cpp
				${oexlib_os}/debug.cpp
				${oexlib_os}/event.cpp
				${oexlib_os}/fmap.cpp
				${oexlib_os}/hq_timer.cpp
				${oexlib_os}/ip_address.cpp
				${oexlib_os}/ip_socket.cpp
				${oexlib_os}/mem.cpp
				${oexlib_os}/mutex.cpp
				${oexlib_os}/sys.cpp
				${oexlib_os}/thread.cpp
				${oexlib_os}/tls.cpp
   )

set( oexlib_headers
				../../inc/alloc.h
				../../inc/arb_delegates.h
   )

set( oexlib_os_headers
				../../inc/base_file.h
				../../inc/debug.h
   )

# Add header files to group
source_group( "Header Files" FILES ${oexlib_headers} )
source_group( "${CMAKE_SYSTEM_NAME} OS Header Files" FILES ${oexlib_os_headers} )

# Add source files to group
source_group( "Source Files" FILES ${oexlib_src} )
source_group( "${CMAKE_SYSTEM_NAME} OS Source Files" FILES ${oexlib_os_src} )

add_library( oexlib STATIC ${oexlib_src} ${oexlib_os_src} ${oexlib_headers} )


#-------------------------------------------------------------------
# build
#-------------------------------------------------------------------
target_link_libraries( oexlib )


#set( EXECUTABLE_OUTPUT_PATH ../../bin )
#FILE( MAKE_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}" )
							 

