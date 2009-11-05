#include <assert.h>

#include "UserMemAlloc.h"
#include "ImportFBX.h"
#include "MeshImportFbx.h"


static MeshImportFBX *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
MeshImportFBX * getInterfaceMeshImportFBX(int version_number)
#else
FBX_DLL_API MeshImportFBX * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTFBX_VERSION )
  {
    gInterface = MEMALLOC_NEW(MeshImportFBX);
  }
  return static_cast<MeshImportFBX *>(gInterface);
};

}

#ifdef WIN32

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE ,
                       DWORD  ul_reason_for_call,
                       LPVOID )
{
  int ret = 0;

  switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
      ret = 1;
			break;
		case DLL_THREAD_ATTACH:
      ret = 2;
			break;
		case DLL_THREAD_DETACH:
      ret = 3;
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#endif
