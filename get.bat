@echo off
rem This batch file is used privately by the original author John W. Ratcliff to copy source between
rem a Perforce depot to the SourceForge SVN depot.

md bin
cd bin
md win32
cd win32

copy \GoogleCode\CodeSuppository\bin\win32\MeshConvert.exe
copy \GoogleCode\CodeSuppository\bin\win32\MeshConvert.exe.manifest
copy \GoogleCode\CodeSuppository\bin\win32\MeshImport.dll
copy \GoogleCode\CodeSuppository\bin\win32\MeshImportEzm.dll
copy \GoogleCode\CodeSuppository\bin\win32\MeshImportObj.dll
copy \GoogleCode\CodeSuppository\bin\win32\MeshImportOgre.dll
copy \GoogleCode\CodeSuppository\bin\win32\MeshImportPSK.dll
copy \GoogleCode\CodeSuppository\bin\win32\MeshImportFBX.dll
copy \GoogleCode\CodeSuppository\bin\win32\MSVCP80.dll
copy \GoogleCode\CodeSuppository\bin\win32\MSVCR80.dll


cd ..
cd ..

md ext
cd ext
md fbx
cd fbx
xcopy c:\p4\experimental\CodeSuppository\ext\fbx\*.* /s
cd ..
cd ..

md media
cd media
md MeshImport
cd MeshImport
copy \GoogleCode\CodeSuppository\media\MeshImport\*.*"
cd ..
cd ..

md include
cd include

md MeshImport
cd MeshImport
copy \GoogleCode\CodeSuppository\include\MeshImport\*.h
copy \GoogleCode\CodeSuppository\include\MeshImport\*.cpp
cd ..

md MeshImportPSK
cd MeshImportPSK
copy \GoogleCode\CodeSuppository\include\MeshImportPSK\*.h
cd ..

md MeshImportFBX
cd MeshImportFBX
copy \GoogleCode\CodeSuppository\include\MeshImportFBX\*.h
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \GoogleCode\CodeSuppository\include\MeshImportEzm\*.h
cd ..


md MeshImportObj
cd MeshImportObj
copy \GoogleCode\CodeSuppository\include\MeshImportObj\*.h
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \GoogleCode\CodeSuppository\include\MeshImportOgre\*.h
cd ..

md common
cd common

md binding
cd binding
copy \GoogleCode\CodeSuppository\include\common\binding\binding.h
copy \GoogleCode\CodeSuppository\include\common\binding\binding.cpp
cd ..

md snippets
cd snippets
copy \GoogleCode\CodeSuppository\include\common\snippets\UserMemAlloc.h
copy \GoogleCode\CodeSuppository\include\common\snippets\KeyValue.h
copy \GoogleCode\CodeSuppository\include\common\snippets\KeyValue.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\KeyValueIni.h
copy \GoogleCode\CodeSuppository\include\common\snippets\KeyValueIni.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\winmsg.h
copy \GoogleCode\CodeSuppository\include\common\snippets\winmsg.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\telnet.h
copy \GoogleCode\CodeSuppository\include\common\snippets\telnet.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\FileInterface.h
copy \GoogleCode\CodeSuppository\include\common\snippets\FileInterface.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\FileSystem.h
copy \GoogleCode\CodeSuppository\include\common\snippets\SystemServices.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\SystemServices.h
copy \GoogleCode\CodeSuppository\include\common\snippets\inparser.h
copy \GoogleCode\CodeSuppository\include\common\snippets\inparser.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\asc2bin.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\asc2bin.h
copy \GoogleCode\CodeSuppository\include\common\snippets\fmem.h
copy \GoogleCode\CodeSuppository\include\common\snippets\stable.h
copy \GoogleCode\CodeSuppository\include\common\snippets\StringDict.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\StringDict.h
copy \GoogleCode\CodeSuppository\include\common\snippets\SendTextMessage.h
copy \GoogleCode\CodeSuppository\include\common\snippets\NxSimpleTypes.h
copy \GoogleCode\CodeSuppository\include\common\snippets\NxAssert.h
copy \GoogleCode\CodeSuppository\include\common\snippets\Nx.h
copy \GoogleCode\CodeSuppository\include\common\snippets\FastXml.h
copy \GoogleCode\CodeSuppository\include\common\snippets\FastXml.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\FloatMath.inl
copy \GoogleCode\CodeSuppository\include\common\snippets\FloatMath.cpp
copy \GoogleCode\CodeSuppository\include\common\snippets\FloatMath.h
copy \GoogleCode\CodeSuppository\include\common\snippets\sutil.h
copy \GoogleCode\CodeSuppository\include\common\snippets\sutil.cpp
cd ..

cd ..
cd ..

md compiler
cd compiler
md vc8
cd vc8
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshConvert.sln
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshConvert.vcproj
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImport.sln
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImport.vcproj
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportEzm.sln
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportEzm.vcproj
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportFbx.vcproj
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportPSK.sln
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportPSK.vcproj
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportObj.sln
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportObj.vcproj
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportOgre.sln
REM copy \GoogleCode\CodeSuppository\compiler\vc8\MeshImportOgre.vcproj
cd ..
md xpj
cd xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\MeshConvert.xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\MeshImport.xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\MeshImportFBX.xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\MeshImportEzm.xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\MeshImportPSK.xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\MeshImportObj.xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\MeshImportOgre.xpj
copy \GoogleCode\CodeSuppository\compiler\xpj\xpj.exe
copy \GoogleCode\CodeSuppository\compiler\xpj\build1.bat
cd ..
cd ..

md app
cd app
md MeshConvert
cd MeshConvert
copy \GoogleCode\CodeSuppository\app\MeshConvert\*.cpp
copy \GoogleCode\CodeSuppository\app\MeshConvert\*.c
copy \GoogleCode\CodeSuppository\app\MeshConvert\*.h
cd ..
cd ..

md docs
cd docs
md MeshImport
cd MeshImport
copy \GoogleCode\CodeSuppository\docs\MeshImport\MeshImport.txt
cd ..
cd ..

md src
cd src


md MeshImport
cd MeshImport
copy \GoogleCode\CodeSuppository\src\MeshImport\*.h
copy \GoogleCode\CodeSuppository\src\MeshImport\*.cpp
copy \GoogleCode\CodeSuppository\src\MeshImport\*.c
cd ..

md MeshImportFBX
cd MeshImportFBX
copy \GoogleCode\CodeSuppository\src\MeshImportFBX\*.h
copy \GoogleCode\CodeSuppository\src\MeshImportFBX\*.cpp
copy \GoogleCode\CodeSuppository\src\MeshImportFBX\*.c
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \GoogleCode\CodeSuppository\src\MeshImportEzm\*.h
copy \GoogleCode\CodeSuppository\src\MeshImportEzm\*.cpp
copy \GoogleCode\CodeSuppository\src\MeshImportEzm\*.c
cd ..


md MeshImportPSK
cd MeshImportPSk
copy \GoogleCode\CodeSuppository\src\MeshImportPSK\*.h
copy \GoogleCode\CodeSuppository\src\MeshImportPSK\*.cpp
copy \GoogleCode\CodeSuppository\src\MeshImportPSK\*.c
cd ..

md MeshImportObj
cd MeshImportObj
copy \GoogleCode\CodeSuppository\src\MeshImportObj\*.h
copy \GoogleCode\CodeSuppository\src\MeshImportObj\*.cpp
copy \GoogleCode\CodeSuppository\src\MeshImportObj\*.c
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \GoogleCode\CodeSuppository\src\MeshImportOgre\*.h
copy \GoogleCode\CodeSuppository\src\MeshImportOgre\*.cpp
copy \GoogleCode\CodeSuppository\src\MeshImportOgre\*.c
cd ..

cd ..
