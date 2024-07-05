!define APP_NAME "GlassOverFlow"
!define APP_VERSION "1.0"
!define INSTALLER_NAME "GlassOverFlowInstaller.exe"
!define VC_REDIST "vc_redist.x86.exe"
!define UNINSTALLER_NAME "uninstall.exe"

Name "${APP_NAME} ${APP_VERSION}"
OutFile "${INSTALLER_NAME}"
InstallDir "$PROGRAMFILES\${APP_NAME}"
RequestExecutionLevel admin

Function CheckForVCRedist
    ReadRegStr $0 HKLM "SOFTWARE\Microsoft\DevDiv\vc\Servicing\14.0\RuntimeMinimum" "Version"
    StrCmp $0 "" InstallVCRedist SkipVCRedist
    StrCmp $0 "14.0.24215" SkipVCRedist  ; Adjust version number as needed
    InstallVCRedist:
        ExecWait "$INSTDIR\${VC_REDIST} /quiet /norestart"
    SkipVCRedist:
FunctionEnd

Section "Install"
    SetOutPath "$INSTDIR"

    File "glassOverFlow.exe"
    
    SetOutPath "$INSTDIR\src"
    File /r "src\*.*"

    SetOutPath "$INSTDIR"
    File "${VC_REDIST}"
    
    Call CheckForVCRedist

    WriteUninstaller "$INSTDIR\${UNINSTALLER_NAME}"

    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME} ${APP_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" "$INSTDIR\${UNINSTALLER_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayVersion" "${APP_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "Publisher" "Your Company Name"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayIcon" "$INSTDIR\glassOverFlow.exe"

SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\glassOverFlow.exe"
    RMDir /r "$INSTDIR\src"
    Delete "$INSTDIR\${VC_REDIST}"
    Delete "$INSTDIR\${UNINSTALLER_NAME}"

    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
    RMDir "$INSTDIR"
SectionEnd