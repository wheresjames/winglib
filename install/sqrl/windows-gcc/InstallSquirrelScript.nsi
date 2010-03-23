; InstallSquirrelScript.nsi
;

;--------------------------------

; The name of the installer
Name "Squirrel Script Engine"

; The file to write
OutFile "InstallSquirrelScript.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Squirrel Script Engine"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\SquirrelScript" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page license
LicenseData "License.txt"
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Squirrel Script Engine (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  SetOverwrite on
  File "InstallSquirrelScript.nsi"
  File "License.txt"
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\sqrl.exe"
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\modules
  
  ; Put file there
  SetOverwrite on
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_cell.dll"
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_curl.dll"
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_ffmpeg.dll"
;  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_fftw.dll"
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_gdchart.dll"
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_irrlicht.dll"
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_live555.dll"
;  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_mysql.dll"
  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_tinyxml.dll"
;  File "..\..\..\..\..\bin\windows-gcc-win32-i386-mingw32-static\_sqmod\sqmod_vmime.dll"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\SquirrelScript "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SquirrelScript" "DisplayName" "Squirrel Script Engine"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SquirrelScript" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SquirrelScript" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SquirrelScript" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Squirrel Script Engine"
  CreateShortCut "$SMPROGRAMS\Squirrel Script Engine\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SquirrelScript"
  DeleteRegKey HKLM SOFTWARE\SquirrelScript

  ; Remove files and uninstaller
  Delete $INSTDIR\InstallSquirrelScript.nsi
  Delete $INSTDIR\uninstall.exe
  
  Delete $INSTDIR\sqrl.exe  
  Delete $INSTDIR\modules\sqmod_cell.dll
  Delete $INSTDIR\modules\sqmod_curl.dll
  Delete $INSTDIR\modules\sqmod_ffmpeg.dll
  Delete $INSTDIR\modules\sqmod_fftw.dll
  Delete $INSTDIR\modules\sqmod_gdchart.dll
  Delete $INSTDIR\modules\sqmod_irrlicht.dll
  Delete $INSTDIR\modules\sqmod_live555.dll
  Delete $INSTDIR\modules\sqmod_mysql.dll
  Delete $INSTDIR\modules\sqmod_tinyxml.dll
  Delete $INSTDIR\modules\sqmod_vmime.dll

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Squirrel Script Engine\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Squirrel Script Engine"
  RMDir "$INSTDIR\modules"
  RMDir "$INSTDIR"

SectionEnd
