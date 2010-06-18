md include
cd include

xcopy \p4\sw\physx\externals\FbxSDK\2010.2\include\*.* /s /Y

cd ..

md lib
cd lib

xcopy \p4\sw\physx\externals\FbxSDK\2010.2\lib\*.* /s /Y

cd ..
