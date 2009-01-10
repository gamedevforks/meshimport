@echo off
rem This batch file is used privately by the original author John W. Ratcliff to copy source between
rem a Perforce depot to the SourceForge SVN depot.
md bin
cd bin
md win32
cd win32
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\TestMeshImport.exe
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\TestMeshImport.exe.manifest
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\Assimp32.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImport.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImportAssimp.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImportEzm.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImportFbx.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImportLeveller.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImportObj.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImportPsk.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MeshImportOgre.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MSVCP80.dll
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\bin\win32\MSVCR80.dll


cd ..
cd ..

md ext
cd ext
md assimp
cd assimp
xcopy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\ext\assimp\*.* /s
cd ..
cd ..

md media
cd media
md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\media\MeshImport\*.*"
cd ..
cd ..

md include
cd include

md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImport\*.h
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImportAssimp\*.h
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImportEzm\*.h
cd ..


md MeshImportFbx
cd MeshImportFbx
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImportFbx\*.h
cd ..

md MeshImportLeveller
cd MeshImportLeveller
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImportLeveller\*.h
cd ..

md MeshImportObj
cd MeshImportObj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImportObj\*.h
cd ..

md MeshImportPsk
cd MeshImportPsk
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImportPsk\*.h
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\MeshImportOgre\*.h
cd ..

md common
cd common

md binding
cd binding
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\binding\binding.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\binding\binding.cpp
cd ..

md TinyXML
cd TinyXML
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\TinyXML\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\TinyXML\*.cpp
cd ..

md MemoryServices
cd MemoryServices
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\MemoryServices\MemoryContainer.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\MemoryServices\MemoryContainer.h
cd ..

md snippets
cd snippets
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\snippets\asc2bin.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\snippets\asc2bin.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\snippets\fmem.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\snippets\stable.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\snippets\StringDict.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\snippets\StringDict.h
cd ..

md FileInterface
cd FileInterface
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\FileInterface\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\include\common\FileInterface\*.h
cd ..


cd ..
cd ..

md compiler
cd compiler
md vc8
cd vc8
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\TestMeshImport.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\TestMeshImport.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImport.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImport.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportAssimp.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportEzm.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportEzm.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportFbx.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportFbx.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportLeveller.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportObj.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportObj.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportPsk.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportPsk.vcproj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportOgre.sln
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\vc8\MeshImportOgre.vcproj
cd ..
md xpj
cd xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\TestMeshImport.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImport.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImportAssimp.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImportEzm.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImportFbx.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImportLeveller.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImportObj.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImportPsk.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\MeshImportOgre.xpj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\xpj.exe
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\compiler\xpj\build1.bat
cd ..
cd ..

md app
cd app
md TestMeshImport
cd TestMeshImport
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\app\TestMeshImport\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\app\TestMeshImport\*.c
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\app\TestMeshImport\*.h
cd ..
cd ..

md docs
cd docs
md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\docs\MeshImport\MeshImport.txt
cd ..
cd ..

md installer
cd installer
md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\installer\MeshImport\license.txt
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\installer\MeshImport\MeshImport.nsi
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\installer\MeshImport\rocket.ico
cd ..
cd ..

md src
cd src


md MeshImport
cd MeshImport
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImport\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImport\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImport\*.c
cd ..

md MeshImportAssimp
cd MeshImportAssimp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportAssimp\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportAssimp\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportAssimp\*.c
cd ..

md MeshImportEzm
cd MeshImportEzm
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportEzm\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportEzm\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportEzm\*.c
cd ..


md MeshImportFbx
cd MeshImportFbx
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportFbx\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportFbx\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportFbx\*.c
cd ..

md MeshImportLeveller
cd MeshImportLeveller
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportLeveller\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportLeveller\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportLeveller\*.c
cd ..

md MeshImportObj
cd MeshImportObj
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportObj\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportObj\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportObj\*.c
cd ..

md MeshImportPsk
cd MeshImportPsk
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportPsk\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportPsk\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportPsk\*.c
cd ..

md MeshImportOgre
cd MeshImportOgre
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportOgre\*.h
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportOgre\*.cpp
copy \p4\depot\HeroEngine\JOHNR-MAIN\he_plugins\src\MeshImportOgre\*.c
cd ..

cd ..
