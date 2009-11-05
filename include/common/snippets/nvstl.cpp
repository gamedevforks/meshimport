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
class CommLayer;

SystemServices *gSystemServices=0;

#if USE_MEMORY_TRACKER
MemoryTracker *gMemoryTracker=0;
#endif

FileSystem *gFileSystem=0;
SendTextMessage *gSendTextMessage=0;
MeshImport *gMeshImport=0;
CommLayer *gCommLayer=0;

#if USE_MEMORY_TRACKER
NVSHARE::MemoryTracker * createMemoryTracker(void) // loads the DLL to track memory allocations.
{
    NVSHARE::MemoryTracker *ret = 0;
#ifdef WIN32
    if ( gMemoryTracker == 0 )
    {
      UINT errorMode = SEM_FAILCRITICALERRORS;
      UINT oldErrorMode = SetErrorMode(errorMode);
      HMODULE module = LoadLibraryA("MemoryTracker.dll");
      SetErrorMode(oldErrorMode);
      void *proc = GetProcAddress(module,"getInterface");
      if ( proc )
      {
        typedef void * (__cdecl * NX_GetToolkit)(NxI32 version,void *systemServices);
      	ret = gMemoryTracker = (NVSHARE::MemoryTracker *)((NX_GetToolkit)proc)(MEMORY_TRACKER_VERSION,0);
       }
    }
#endif
    return ret;
}
#endif

}; // end of namespace
