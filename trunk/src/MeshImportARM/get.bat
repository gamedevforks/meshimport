rd foundation /s /q
rd reviewed /s /q
rd general /s /q
rd NxParameterized /s /q

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

md string_parsing
cd string_parsing

md include
cd include
copy \p4\sw\physx\shared\general\string_parsing\2\include\FastXML.h
copy \p4\sw\physx\shared\general\string_parsing\2\include\PxAsciiConversion.h
copy \p4\sw\physx\shared\general\string_parsing\2\include\PxAsciiConversion.inl
cd ..

md src
cd src
copy \p4\sw\physx\shared\general\string_parsing\2\src\FastXML.cpp
cd ..

cd ..


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
copy \p4\sw\physx\APEXSDK\1.0\feature\UE3_APEX\NxParameterized\include\*.*
cd ..
md public
cd public
copy \p4\sw\physx\APEXSDK\1.0\feature\UE3_APEX\NxParameterized\public\*.*
cd ..
md src
cd src
copy \p4\sw\physx\APEXSDK\1.0\feature\UE3_APEX\NxParameterized\src\*.*
cd ..
cd ..

call get_params.bat

del *.bak /s

