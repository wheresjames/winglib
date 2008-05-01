#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

set( project_name 	oexlib )

set( output_dir 	../../bin ) 
#add_definitions( -DUNICODE -D_UNICODE )

set( src_files
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
#					../../src/zip.cpp 
#					../../src/unzip.cpp 
					../../src/msg.cpp 
					../../src/msg_com.cpp 
					../../src/msg_info.cpp 
					../../src/msg_mgr.cpp 
					../../src/msg_object.cpp 
					
					../../src/vid/avi_file.cpp
   )
		   
set( header_files
					../../oexlib.h 
					
					../../inc/alloc.h
					../../inc/asso_list.h 
					../../inc/circ_buf.h 
					../../inc/data_packet.h 
					../../inc/fifo_sync.h 
					../../inc/file.h 
					../../inc/find_files.h 
					../../inc/md5.h 
					../../inc/parser.h 
					../../inc/sys_time.h 
					../../inc/zip.h 
					../../inc/unzip.h 
					../../inc/msg.h 
					../../inc/msg_com.h 
					../../inc/msg_info.h 
					../../inc/msg_mgr.h 

					../../inc/vid/avi_file.h
   )

if( WIN32 )
	set( src_os		../../src/os/win32 )		   
else( WIN32 )
	set( src_os		../../src/os/posix )		   
endif( WIN32 )
set( src_files_os
					${src_os}/base_file.cpp
					${src_os}/debug.cpp
					${src_os}/event.cpp
					${src_os}/fmap.cpp
					${src_os}/hq_timer.cpp
					${src_os}/ip_address.cpp
					${src_os}/ip_socket.cpp
					${src_os}/mem.cpp
					${src_os}/mutex.cpp
					${src_os}/sys.cpp
					${src_os}/thread.cpp
					${src_os}/tls.cpp
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

# OS source files
source_group( "${CMAKE_SYSTEM_NAME} OS Source Files" FILES ${src_files_os} )

if( WIN32 )
	add_library( ${project_name} STATIC ${src_files} ${src_files_os} ${header_files} )
else( WIN32 )
	add_library( ${project_name} STATIC ${src_files} ${src_files_os} ${header_files} )
endif( WIN32 )	

#-------------------------------------------------------------------
# build
#-------------------------------------------------------------------

target_link_libraries( 
						${project_name} 
					  )
						 

				 

