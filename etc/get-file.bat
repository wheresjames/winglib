@echo off

echo %1 %2 %3

IF EXIST %2 GOTO EXTRACT
wget %1 -O %2 %1

:EXTRACT

REM # IF EXIST %3 GOTO EOF

set OLDWD=%CD%
cd %3
%OLDWD%\bunzip2 -c %2 | %OLDWD%\tar xkf -
cd %OLDWD%

:EOF
