#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

set( project_name 	AviScan )

set( output_dir 	../../bin ) 

#add_definitions( -DUNICODE=1 -D_UNICODE=1 )

set( src_files
					../../stdafx.cpp 
					../../main_app.cpp 
					../../frame_wnd.cpp 
					
					../../vid/riff_file.cpp
   )
		   
set( header_files
					../../stdafx.h
					../../main_app.h
					../../frame_wnd.h
					
					../../vid/riff_file.h
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

#set( wxWidgets_USE_UNICODE 1 )
set( wxWidgets_USE_LIBS base core gl net )
#set( wxWidgets_CONFIGURATION mswu )
find_package( wxWidgets )
if( wxWidgets_FOUND )
	include( ${wxWidgets_USE_FILE} )
	include_directories( ${wxWidgets_INCLUDE_DIRS} )
endif( wxWidgets_FOUND )

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

if( _WINDOWS )
	add_executable( ${project_name} WIN32 ${src_files} ${header_files} )
else( _WINDOWS )
	add_executable( ${project_name} WIN32 ${src_files} ${header_files} )
endif( _WINDOWS )	

#-------------------------------------------------------------------
# build
#-------------------------------------------------------------------

#set( exe_targets, ${project_name} )
#set( lib_targets, )

#if( wxWidgets_FOUND )
#	set( targets, ${lib_targets} ${wxWidgets_LIBRARIES} )
#endif( wxWidgets_FOUND )

#target_link_libraries( ${targets} ${lib_targets} )

target_link_libraries( 
						${project_name} 
						${wxWidgets_LIBRARIES}
					  )
						 

				 

