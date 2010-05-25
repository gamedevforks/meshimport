// traps memory allocation calls from nvstsl
#include "UserMemAlloc.h"

#ifdef WIN32
#include <windows.h>
#endif

#pragma warning(disable:4100)

namespace NVSHARE
{

class FileSystem;
class MeshImport;


FileSystem *gFileSystem=0;
SendTextMessage *gSendTextMessage=0;
MeshImport *gMeshImport=0;

}; // end of namespace
