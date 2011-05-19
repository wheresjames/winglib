; InstallWinglibScriptEngine.nsi
;
;--------------------------------

!define APPNAME "SNTP Sync ${PROC}"

!define FILENAME "SntpSync"

!define APPKEY "${FILENAME}_${PROC}"

; The name of the installer

; The file to write
!ifdef DVER
	!define APPVNAME "${APPNAME} ${DVER}"
!else
	!define APPVNAME "${APPNAME}"
!endif

Name "${APPVNAME}"

!ifdef FVER
	OutFile "${OUTROOT}\Install${FILENAME}${POSTFIX}_${FVER}_${PROC}.exe"
!else
	OutFile "${OUTROOT}\Install${FILENAME}${POSTFIX}_${PROC}.exe"
!endif

; The default installation director
!if "${PROC}" == "x64"
	InstallDir "$PROGRAMFILES64\${APPNAME}"
!else
	InstallDir "$PROGRAMFILES\${APPNAME}"
!endif

; Request application privileges for Windows Vista
RequestExecutionLevel admin

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "SOFTWARE\${APPKEY}" "Install_Dir"

;--------------------------------
; Pages

;!define MUI_FINISHPAGE_NOAUTOCLOSE
;!define MUI_FINISHPAGE_RUN
;!define MUI_FINISHPAGE_RUN_TEXT "Open Scripts Folder"
;!define MUI_FINISHPAGE_RUN_FUNCTION "OpenScriptsFolder"
;!insertmacro MUI_PAGE_FINISH
;Function OpenScriptsFolder
;	ExecShell "open" "$INSTDIR/scripts"
;FunctionEnd

Page license
LicenseData "License.txt"
Page components
Page directory
Page instfiles


UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "${APPVNAME} (required)"

  SectionIn RO

!if "${PROC}" == "x64"
	SetRegView 64
!endif
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  SetOverwrite on
  File "License.txt"
  File "${OUTROOT}\${FILENAME}${POSTFIX}.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\${APPKEY}" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "DisplayName" "${APPVNAME}"
!ifdef DVER
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "DisplayVersion" "${DVER}"
!endif
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\${APPNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Configure.lnk" "$INSTDIR\${FILENAME}.cfg"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Register And Start"

  ; Start service
  ExecWait '"$INSTDIR\${FILENAME}${POSTFIX}.exe" -install -start'
  
SectionEnd


;--------------------------------
; Uninstaller

Section "Uninstall"
  
!if "${PROC}" == "x64"
	SetRegView 64
!endif

  ; Stop wlink service
  ExecWait '"$INSTDIR\${FILENAME}${POSTFIX}.exe" -stop -uninstall'

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}"
  DeleteRegKey HKLM "SOFTWARE\${APPKEY}"

  ; Remove files and uninstaller
  Delete "$INSTDIR\uninstall.exe"
  
  Delete "$INSTDIR\${FILENAME}${POSTFIX}.exe"  

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APPNAME}\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${APPNAME}"
  RMDir "$INSTDIR"
  
SectionEnd

;--------------------------------
; Remove previous version

Function .onInit
!if "${PROC}" == "x64"
	SetRegView 64
!endif
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "UninstallString"
  StrCmp $R0 "" done
    MessageBox MB_YESNOCANCEL|MB_ICONQUESTION  "A previous version of ${APPNAME} was found.$\n$\nIt is recommended that you uninstall it first.$\n$\nDo you want to do that now?" IDNO done IDYES uninst
      Abort
uninst:
    ExecWait $R0
done: 
FunctionEnd

