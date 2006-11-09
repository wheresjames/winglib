# Microsoft Developer Studio Project File - Name="oexlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=oexlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "oexlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "oexlib.mak" CFG="oexlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "oexlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "oexlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "oexlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release"
# PROP Intermediate_Dir "../Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Debug"
# PROP Intermediate_Dir "../Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "oexlib - Win32 Release"
# Name "oexlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\circ_buf.cpp
# End Source File
# Begin Source File

SOURCE=.\data_packet.cpp
# End Source File
# Begin Source File

SOURCE=.\os\win32\fmap.cpp
# End Source File
# Begin Source File

SOURCE=.\opc\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\os\win32\mmap.cpp
# End Source File
# Begin Source File

SOURCE=.\os\win32\mutex.cpp
# End Source File
# Begin Source File

SOURCE=.\os\win32\os_check.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\circ_buf.h
# End Source File
# Begin Source File

SOURCE=.\data_packet.h
# End Source File
# Begin Source File

SOURCE=.\file_mapping.h
# End Source File
# Begin Source File

SOURCE=.\os\fmap.h
# End Source File
# Begin Source File

SOURCE=.\opc\md5.h
# End Source File
# Begin Source File

SOURCE=.\memory.h
# End Source File
# Begin Source File

SOURCE=.\os\mmap.h
# End Source File
# Begin Source File

SOURCE=.\os\mutex.h
# End Source File
# Begin Source File

SOURCE=.\oexlib.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_config.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_debugging.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_exceptions.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_macros.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_namespace.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_os_win32.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_typedefs.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_types.h
# End Source File
# Begin Source File

SOURCE=.\oexlib_warnings.h
# End Source File
# Begin Source File

SOURCE=.\RFun.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\os\win32\stdwin.h
# End Source File
# Begin Source File

SOURCE=.\thread_safe.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
