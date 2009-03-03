@echo off
rem This batch file is used privately by the original author John W. Ratcliff to copy source between
rem a Perforce depot to the SourceForge SVN depot.
md bin
cd bin
md win32
cd win32

rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\TestMeshImport.exe
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\TestMeshImport.exe.manifest
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\Assimp32.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImport.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportAssimp.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportEzm.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportLeveller.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportObj.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MeshImportOgre.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MSVCP80.dll
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\bin\win32\MSVCR80.dll


cd ..
cd ..

md ext
cd ext
md assimp
cd assimp
xcopy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\ext\assimp\*.* /s
cd ..
cd ..

md media
cd media
md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\media\MeshImport\*.*"
cd ..
cd ..

md include
cd include

md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImport\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImport\*.cpp
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportAssimp\*.h
cd ..

md MeshImportEzm
cd MeshImportEzm
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportEzm\*.h
cd ..


md MeshImportLeveller
cd MeshImportLeveller
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportLeveller\*.h
cd ..

md MeshImportObj
cd MeshImportObj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportObj\*.h
cd ..

md MeshImportOgre
cd MeshImportOgre
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\MeshImportOgre\*.h
cd ..

md common
cd common

md binding
cd binding
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\binding\binding.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\binding\binding.cpp
cd ..

md MemoryServices
cd MemoryServices
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\MemoryServices\MemoryContainer.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\MemoryServices\MemoryContainer.h
cd ..

md snippets
cd snippets
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\FileSystem.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\SystemServices.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\SystemServices.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\inparser.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\inparser.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\asc2bin.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\asc2bin.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\fmem.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\stable.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\StringDict.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\StringDict.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\SendTextMessage.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\HeSimpleTypes.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\He.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\FastXml.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\FastXml.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\FloatMath.inl
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\FloatMath.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\FloatMath.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\sutil.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\snippets\sutil.cpp
cd ..

md FileInterface
cd FileInterface
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\FileInterface\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\include\common\FileInterface\*.h
cd ..


cd ..
cd ..

md compiler
cd compiler
md vc8
cd vc8
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\TestMeshImport.sln
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\TestMeshImport.vcproj
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImport.sln
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImport.vcproj
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.sln
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.vcproj
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportEzm.sln
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportEzm.vcproj
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportFbx.vcproj
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.sln
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.vcproj
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportObj.sln
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportObj.vcproj
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportOgre.sln
rem copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\vc8\MeshImportOgre.vcproj
cd ..
md xpj
cd xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\TestMeshImport.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImport.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportAssimp.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportEzm.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportLeveller.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportObj.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\MeshImportOgre.xpj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\xpj.exe
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\compiler\xpj\build1.bat
cd ..
cd ..

md app
cd app
md TestMeshImport
cd TestMeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestMeshImport\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestMeshImport\*.c
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\app\TestMeshImport\*.h
cd ..
cd ..

md docs
cd docs
md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\docs\MeshImport\MeshImport.txt
cd ..
cd ..

md installer
cd installer
md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\MeshImport\license.txt
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\MeshImport\MeshImport.nsi
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\installer\MeshImport\rocket.ico
cd ..
cd ..

md src
cd src


md MeshImport
cd MeshImport
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImport\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImport\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImport\*.c
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportAssimp\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportAssimp\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportAssimp\*.c
cd ..

md MeshImportEzm
cd MeshImportEzm
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportEzm\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportEzm\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportEzm\*.c
cd ..


md MeshImportLeveller
cd MeshImportLeveller
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportLeveller\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportLeveller\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportLeveller\*.c
cd ..

md MeshImportObj
cd MeshImportObj
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportObj\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportObj\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportObj\*.c
cd ..

md MeshImportOgre
cd MeshImportOgre
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportOgre\*.h
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportOgre\*.cpp
copy g:\p4\depot\HeroEngine\CLOTH-MAIN\he_plugins\src\MeshImportOgre\*.c
cd ..

cd ..
