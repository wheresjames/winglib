
!include "MUI2.nsh"

; InstallWinglibScriptEngine.nsi
;
;--------------------------------

!define APPNAME "SNTP Sync ${PROC}"

!define FILENAME "SntpSync"

!define APPKEY "${FILENAME}_${PROC}_${BUILD}"

; The name of the installer

; The file to write
!ifdef DVER
	!define APPVNAME "${APPNAME} ${DVER}"
!else
	!define APPVNAME "${APPNAME}"
!endif

Name "${APPVNAME}"

!ifdef FVER
	OutFile "${OUTROOT}\Install${FILENAME}${POSTFIX}_${FVER}_${PROC}_${BUILD}.exe"
!else
	OutFile "${OUTROOT}\Install${FILENAME}${POSTFIX}_${PROC}_${BUILD}.exe"
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

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_LICENSE "License.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

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
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "InstallLocation" '$INSTDIR'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)

;/******************************************************************************
;    WORKAROUND - lnkX64IconFix
;        This snippet was developed to address an issue with Windows 
;        x64 incorrectly redirecting the shortcuts icon from $PROGRAMFILES32 
;        to $PROGRAMFILES64.
; 
;    See Forum post: http://forums.winamp.com/newreply.php?do=postreply&t=327806
; 
;    Example:
;        CreateShortcut "$SMPROGRAMS\My App\My App.lnk" "$INSTDIR\My App.exe" "" "$INSTDIR\My App.exe"
;        ${lnkX64IconFix} "$SMPROGRAMS\My App\My App.lnk"
; 
;    Original Code by Anders [http://forums.winamp.com/member.php?u=70852]
; ******************************************************************************/
!ifndef ___lnkX64IconFix___
    !verbose push
    !verbose 0
 
    !include "LogicLib.nsh"
    !include "x64.nsh"
 
    !define ___lnkX64IconFix___
    !define lnkX64IconFix `!insertmacro _lnkX64IconFix`
    !macro _lnkX64IconFix _lnkPath
        !verbose push
        !verbose 0
        ${If} ${RunningX64}
            DetailPrint "WORKAROUND: 64bit OS Detected, Attempting to apply lnkX64IconFix"
            Push "${_lnkPath}"
            Call lnkX64IconFix
        ${EndIf}
        !verbose pop
    !macroend
 
    Function lnkX64IconFix ; _lnkPath
        Exch $5
        Push $0
        Push $1
        Push $2
        Push $3
        Push $4
        System::Call 'OLE32::CoCreateInstance(g "{00021401-0000-0000-c000-000000000046}",i 0,i 1,g "{000214ee-0000-0000-c000-000000000046}",*i.r1)i'
        ${If} $1 <> 0
            System::Call '$1->0(g "{0000010b-0000-0000-C000-000000000046}",*i.r2)'
            ${If} $2 <> 0
                System::Call '$2->5(w r5,i 2)i.r0'
                ${If} $0 = 0
                    System::Call '$1->0(g "{45e2b4ae-b1c3-11d0-b92f-00a0c90312e1}",*i.r3)i.r0'
                    ${If} $3 <> 0
                        System::Call '$3->5(i 0xA0000007)i.r0'
                        System::Call '$3->6(*i.r4)i.r0'
                        ${If} $0 = 0 
                            IntOp $4 $4 & 0xffffBFFF
                            System::Call '$3->7(ir4)i.r0'
                            ${If} $0 = 0 
                                System::Call '$2->6(i0,i0)'
                                DetailPrint "WORKAROUND: lnkX64IconFix Applied successfully"
                            ${EndIf}
                        ${EndIf}
                        System::Call $3->2()
                    ${EndIf}
                ${EndIf}
                System::Call $2->2()
            ${EndIf}
            System::Call $1->2()
        ${EndIf} 
        Pop $4
        Pop $3
        Pop $2
        Pop $1
        Pop $0
    FunctionEnd
    !verbose pop
!endif

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\${APPNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Configure.lnk" "$INSTDIR\${FILENAME}.cfg" \
				 "" "$INSTDIR\${FILENAME}${POSTFIX}.exe"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
  ; Fix 64 bit windows bug
  ${lnkX64IconFix} "$SMPROGRAMS\${APPNAME}\Configure.lnk"

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
  ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "InstallLocation"
  StrCmp $R0 "" done
    MessageBox MB_YESNOCANCEL|MB_ICONQUESTION \
			   "A previous version of ${APPNAME} was found.$\n$\nIt is recommended that you uninstall it first.$\n$\nDo you want to do that now?" \
			   /SD IDYES IDNO done IDYES uninst
      Abort
uninst:
    ExecWait '$R0 /S _?=$R1'
done: 
FunctionEnd

