rd foundation /s /q
rd reviewed /s /q
rd general /s /q

md NxParameterized
cd NxParameterized
rd include /s /q
rd public /s /q
rd src /s /q
cd ..

md foundation
cd foundation
copy \p4\sw\physx\shared\foundation\2\feature\apex\*.*
md windows
cd windows
copy \p4\sw\physx\shared\foundation\2\feature\apex\windows\*.*
cd ..
cd ..

md reviewed
cd reviewed

md include
cd include
copy \p4\sw\physx\shared\reviewed\2\feature\apex\include\*.*

md windows
cd windows
copy \p4\sw\physx\shared\reviewed\2\feature\apex\include\windows\*.*
cd ..
cd ..

md src
cd src
copy \p4\sw\physx\shared\reviewed\2\feature\apex\src\*.*
md windows
cd windows
copy \p4\sw\physx\shared\reviewed\2\feature\apex\src\windows\*.*
cd ..

cd ..
cd ..

md general
cd general

md PxIOStream
cd PxIOStream

md include
cd include
copy \p4\sw\physx\shared\general\PxIOStream\2\include\*.*
cd ..

md public
cd public
copy \p4\sw\physx\shared\general\PxIOStream\2\public\*.*
cd ..


cd ..

md shared
cd shared
md include
cd include
copy \p4\sw\physx\shared\general\shared\2\include\*.*
cd ..
cd ..

cd ..

md NxParameterized
cd NxParameterized
md include
cd include
copy \p4\sw\physx\APEXSDK\1.0\trunk\NxParameterized\include\*.*
cd ..
md public
cd public
copy \p4\sw\physx\APEXSDK\1.0\trunk\NxParameterized\public\*.*
cd ..
md src
cd src
copy \p4\sw\physx\APEXSDK\1.0\trunk\NxParameterized\src\*.*
cd ..
cd ..

call get_params.bat

del *.bak /s

