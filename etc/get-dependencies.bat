@echo off

REM #-------------------------------------------------------------------
REM # Setup
REM #-------------------------------------------------------------------

set PWD=%CD%
set FILE_EXT=tar.bz2
set PATCHES=%CD%/patches
set INSTALL_ROOT=%CD%\..\..
set DOWNLOADS=%CD%\..\..\..\downloads
set REPO=http://www.wetcoin.com/src

md %INSTALL_ROOT%
md %DOWNLOADS%

set /p PROJECTS=<dependencies.txt

for %%p in (%PROJECTS%) do get-file.bat "%REPO%/%%p.tar.bz2" "%DOWNLOADS%\%%p.tar.bz2" "%INSTALL_ROOT%"

