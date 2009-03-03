SetCompressor /SOLID /FINAL lzma
SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
XPStyle on

!define ShortName      "MeshImport"
!define LongName       "MeshImport - MeshImport plugin system by John W. Ratcliff"
!define ShortVersion   "1.1"
!define LongVersion    "v1.1"
Icon                   "Rocket.ico"
UninstallIcon          "Rocket.ico"
!define RootDir        "..\.."
!define TEMP           "c:\temp"

Name        "${LongName}"
OutFile     "${ShortName}_${ShortVersion}.exe"

InstallDir  "c:\Program Files\MeshImport"

InstallDirRegKey HKLM "Software\MeshImport" "Install_Dir"

CRCCheck force
BrandingText "MeshImport by John W. Ratcliff"

#-------------------------------------------
# License Page Info
#-------------------------------------------
PageEx license
LicenseText "License Agreement"
LicenseData license.txt
PageExEnd

#-------------------------------------------
# Other Page Declarations (including uninstall pages)
#-------------------------------------------
Page components "" .onSelChange ""
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles



#-------------------------------------------
# Executables, DLLs and Data section
#-------------------------------------------
Section "MeshImport Executable and Data" sec1

  # ----------Executables and DLLs---------
  SetOutPath "$INSTDIR\bin\win32"

## Install the two executables.
  File "${RootDir}\bin\win32\TestMeshImport.exe"
  File "${RootDir}\bin\win32\TestMeshImport.exe.Manifest"
  File "${RootDir}\bin\win32\MeshImport.dll"
  File "${RootDir}\bin\win32\Assimp32.dll"
  File "${RootDir}\bin\win32\MeshImportAssimp.dll"
  File "${RootDir}\bin\win32\MeshImportEzm.dll"
  File "${RootDir}\bin\win32\MeshImportLeveller.dll"
  File "${RootDir}\bin\win32\MeshImportObj.dll"
  File "${RootDir}\bin\win32\MeshImportOgre.dll"

## Install the various DLL components

  File "${RootDir}\bin\win32\MSVCP80.dll"
  File "${RootDir}\bin\win32\MSVCR80.dll"

  SetOutPath "$INSTDIR\ext\assimp"
  File "${RootDir}\ext\assimp\*.h"
  File "${RootDir}\ext\assimp\*.lib"

  SetOutPath "$INSTDIR\ext\assimp\Compiler"
  File "${RootDir}\ext\assimp\Compiler\*.h"

  SetOutPath "$INSTDIR\ext\assimp\Compiler\MSVC"
  File "${RootDir}\ext\assimp\Compiler\MSVC\*.h"

## Install the source code
  SetOutPath "$INSTDIR\src\MeshImport"
  File "${RootDir}\src\MeshImport\*.cpp"
  File "${RootDir}\src\MeshImport\*.h"

  SetOutPath "$INSTDIR\src\MeshImportAssimp"
  File "${RootDir}\src\MeshImportAssimp\*.cpp"

  SetOutPath "$INSTDIR\src\MeshImportEzm"
  File "${RootDir}\src\MeshImportEzm\*.cpp"

  SetOutPath "$INSTDIR\src\MeshImportLeveller"
  File "${RootDir}\src\MeshImportLeveller\*.cpp"

  SetOutPath "$INSTDIR\src\MeshImportObj"
  File "${RootDir}\src\MeshImportObj\*.cpp"

  SetOutPath "$INSTDIR\src\MeshImportOgre"
  File "${RootDir}\src\MeshImportOgre\*.cpp"


## Install the header files.

  SetOutPath "$INSTDIR\include\MeshImport"
  File "${RootDir}\include\MeshImport\*.h"

  SetOutPath "$INSTDIR\include\MeshImportAssimp"
  File "${RootDir}\include\MeshImportAssimp\*.h"

  SetOutPath "$INSTDIR\include\MeshImportEzm"
  File "${RootDir}\include\MeshImportEzm\*.h"

  SetOutPath "$INSTDIR\include\MeshImportLeveller"
  File "${RootDir}\include\MeshImportLeveller\*.h"

  SetOutPath "$INSTDIR\include\MeshImportObj"
  File "${RootDir}\include\MeshImportObj\*.h"

  SetOutPath "$INSTDIR\include\MeshImportOgre"
  File "${RootDir}\include\MeshImportOgre\*.h"

  SetOutPath "$INSTDIR\include\common\binding"
  File "${RootDir}\include\common\binding\*.h"
  File "${RootDir}\include\common\binding\*.cpp"

  SetOutPath "$INSTDIR\include\common\snippets"
  File "${RootDir}\include\common\snippets\*.h"
  File "${RootDir}\include\common\snippets\*.cpp"

  SetOutPath "$INSTDIR\include\common\MemoryServices"
  File "${RootDir}\include\common\MemoryServices\*.h"
  File "${RootDir}\include\common\MemoryServices\*.cpp"

## Install the application source code.
  SetOutPath "$INSTDIR\app\TestMeshImport"
  File "${RootDir}\app\TestMeshImport\*.cpp"

# Install the project builder files
  SetOutPath "$INSTDIR\compiler\xpj"
  File "${RootDir}\compiler\xpj\*.xpj"
  File "${RootDir}\compiler\xpj\xpj.exe"
  File "${RootDir}\compiler\xpj\build1.bat"

## Install the VC8 solution and project files.
  SetOutPath "$INSTDIR\compiler\VC8"

  File "${RootDir}\compiler\VC8\*.sln"
  File "${RootDir}\compiler\VC8\*.vcproj"

  CreateDirectory "$SMPROGRAMS\MeshImport"

  # add shortcut to MeshImport executable
  CreateShortCut  "$SMPROGRAMS\MeshImport\TestMeshImport.exe.lnk" "$INSTDIR\bin\win32\TestMeshImport.exe" "" "" "" "SW_SHOWNORMAL" ""

 CreateDirectory "$SMPROGRAMS\MeshImport\Projects"
  CreateShortCut "$SMPROGRAMS\MeshImport\Projects\TestMeshImport.sln.lnk" "$INSTDIR\compiler\VC8\TestMeshImport.sln" "" "" "" "SW_SHOWNORMAL" ""


SectionEnd

#-------------------------------------------
# Registry keys, shortcuts, etc. section
#-------------------------------------------
Section "-RegistryInfo" sec6 ; the - sign indicates it is hidden

  # Write the install dir into the registry
  WriteRegStr HKLM "SOFTWARE\John W. Ratcliff\MeshImport" "Install_Dir" "$INSTDIR"

  # Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MeshImport" "DisplayName" "MeshImport"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MeshImport" "UninstallString" '"$INSTDIR\MeshImport_uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MeshImport" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MeshImport" "NoRepair" 1
  WriteUninstaller "MeshImport_uninstall.exe"

  # MeshImport projects
  CreateDirectory "$SMPROGRAMS\MeshImport"

  CreateShortCut "$SMPROGRAMS\MeshImport\MeshImport_uninstall.lnk" "$INSTDIR\MeshImport_uninstall.exe" "" "$INSTDIR\MeshImport_uninstall.exe" 0

SectionEnd


#-------------------------------------------
# Uninstaller Section
#-------------------------------------------
Section "Uninstall"

  # Remove registry keys
  DeleteRegKey HKLM "SOFTWARE\John W. Ratcliff\MeshImport"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MeshImport"

  # Remove uninstaller and its link
  Delete $INSTDIR\MeshImport_uninstall.exe

  # Remove all files

  RMDir /r $INSTDIR

SectionEnd


#-------------------------------------------
# Function .onSelChange
#-------------------------------------------
Function .onSelChange
  # Here we detect if all of the sections are unchecked
  # If they are, then we disable the Next button

  ; get all of the flags
  SectionGetFlags ${sec1} $1
  SectionGetFlags ${sec2} $2
  SectionGetFlags ${sec5} $4
  SectionGetFlags ${sec7} $5
  SectionGetFlags ${sec8} $6
  SectionGetFlags ${sec9} $7
  SectionGetFlags ${sec10} $8
  IntOp $9 $1 + $2
  IntOp $9 $9 + $3
  IntCmp $9 0 DisableSix EnableSix EnableSix
EnableSix:
  SectionSetFlags ${sec6} 1
  Goto ContinueOn
DisableSix:
  SectionSetFlags ${sec6} 0
ContinueOn:

  IntOp $0 $1 + $2
  IntOp $0 $0 + $3
  IntOp $0 $0 + $4
  IntOp $0 $0 + $5
  IntOp $0 $0 + $6
  IntOp $0 $0 + $7
  IntOp $0 $0 + $8

  IntCmp $0 0 DisWin EnWin EnWin

DisWin:
  GetDlgItem $0 $HWNDPARENT 1
  EnableWindow $0 0
  Goto FnEnd

EnWin:
  GetDlgItem $0 $HWNDPARENT 1
  EnableWindow $0 1

FnEnd:

FunctionEnd

