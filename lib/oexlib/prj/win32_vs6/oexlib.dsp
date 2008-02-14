# Microsoft Developer Studio Project File - Name="oexlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CMAKE DSP Header file
# This file is read by the CMAKE, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit in that case.

# variables to REPLACE
# 
#  == include path
#   == compiler defines
# ..\..\bin\ == override in output directory
# oexlib  == name of output library

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
!MESSAGE "oexlib - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "oexlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "oexlib - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
!MESSAGE "oexlib - Win32 Debug" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "oexlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_LIB"  /FD /c
# ADD CPP    
# ADD CPP  /DWIN32 /D_WINDOWS /W3 /Zm1000 /GX /GR /D "_MBCS"
# ADD CPP /MD /O2 /Ob2 /D NDEBUG -DCMAKE_INTDIR=\"Release\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\bin\Release/oexlib.lib" 



!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_LIB"  /FD /GZ /c
# ADD CPP    
# ADD CPP  /DWIN32 /D_WINDOWS /W3 /Zm1000 /GX /GR /D "_MBCS"
# ADD CPP /D_DEBUG /MDd /Zi  /Ob0 /Od /GZ -DCMAKE_INTDIR=\"Debug\" 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\bin\Debug/oexlib_d.lib" 



!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP    
# ADD CPP  /DWIN32 /D_WINDOWS /W3 /Zm1000 /GX /GR /D "_MBCS"
# ADD CPP /MD /O1 /Ob1 /D NDEBUG -DCMAKE_INTDIR=\"MinSizeRel\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\bin\MinSizeRel/oexlib.lib" 



!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_LIB"  /FD /c
# ADD CPP    
# ADD CPP  /DWIN32 /D_WINDOWS /W3 /Zm1000 /GX /GR /D "_MBCS"
# ADD CPP /MD /Zi /O2 /Ob1 /D NDEBUG -DCMAKE_INTDIR=\"RelWithDebInfo\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\bin\RelWithDebInfo/oexlib.lib" 



!ENDIF 

# Begin Target

# Name "oexlib - Win32 Release"
# Name "oexlib - Win32 Debug"
# Name "oexlib - Win32 MinSizeRel"
# Name "oexlib - Win32 RelWithDebInfo"

# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt

!IF  "$(CFG)" == "oexlib - Win32 Release"
USERDEP__HACK=\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeVS6FindMake.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystem.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeSystem.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystemSpecificInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeGenericSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\gcc.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\WindowsPaths.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeRCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.c.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCPlatform.cmake\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXPlatform.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeWindowsSystemConfig.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Building Custom Rule M:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6/CMakeLists.txt

oexlib.dsp.cmake :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\CMake 2.4\bin\cmake.exe" -HM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6 -BM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6

# End Custom Build

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"
USERDEP__HACK=\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeVS6FindMake.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystem.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeSystem.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystemSpecificInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeGenericSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\gcc.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\WindowsPaths.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeRCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.c.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCPlatform.cmake\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXPlatform.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeWindowsSystemConfig.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Building Custom Rule M:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6/CMakeLists.txt

oexlib.dsp.cmake :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\CMake 2.4\bin\cmake.exe" -HM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6 -BM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6

# End Custom Build

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"
USERDEP__HACK=\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeVS6FindMake.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystem.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeSystem.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystemSpecificInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeGenericSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\gcc.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\WindowsPaths.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeRCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.c.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCPlatform.cmake\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXPlatform.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeWindowsSystemConfig.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Building Custom Rule M:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6/CMakeLists.txt

oexlib.dsp.cmake :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\CMake 2.4\bin\cmake.exe" -HM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6 -BM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6

# End Custom Build

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"
USERDEP__HACK=\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeVS6FindMake.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystem.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeSystem.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeSystemSpecificInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeGenericSystem.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\gcc.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\WindowsPaths.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeDetermineRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCCompiler.cmake.in"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeRCCompiler.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeRCInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestRCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CheckTypeSize.c.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCCompiler.cmake.in"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCXXInformation.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\Platform\Windows-cl.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCPlatform.cmake\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeFiles\CMakeCXXPlatform.cmake\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeVisualStudio6Configurations.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeCommonLanguageInclude.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Modules\CMakeTestCXXCompiler.cmake"\
	"C:\Program Files\CMake 2.4\share\cmake-2.4\Templates\CMakeWindowsSystemConfig.cmake"\
	M:\Code\_temp\winglib\lib\oexlib\prj\win32_vs6\CMakeLists.txt
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Building Custom Rule M:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6/CMakeLists.txt

oexlib.dsp.cmake :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\CMake 2.4\bin\cmake.exe" -HM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6 -BM:/Code/_temp/winglib/lib/oexlib/prj/win32_vs6

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\alloc.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\asso_list.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\circ_buf.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\data_packet.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\fifo_sync.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\file.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\find_files.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\md5.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\oex.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\parser.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\sys_time.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\unzip.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\zip.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\inc\alloc.h

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\inc\arb_delegates.h

# End Source File
# End Group
# Begin Group "Windows OS Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\base_file.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\debug.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\event.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\fmap.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\hq_timer.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\ip_address.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\ip_socket.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\mem.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\mutex.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\sys.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\thread.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=M:\Code\_temp\winglib\lib\oexlib\src\os\win32\tls.cpp

!IF  "$(CFG)" == "oexlib - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "oexlib - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
