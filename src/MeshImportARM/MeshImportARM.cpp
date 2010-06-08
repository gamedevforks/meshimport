#include <assert.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportARM.h"

#ifdef WIN32
#ifdef MESHIMPORTARM_EXPORTS
#define MESHIMPORTARM_API __declspec(dllexport)
#else
#define MESHIMPORTARM_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTARM_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTARM_API NVSHARE::MeshImporter * getInterface(NxI32 version_number);
};

namespace NVSHARE
{
class MyMeshImportARM : public MeshImporter, public Memalloc
{
public:
  MyMeshImportARM(void)
  {
  }

  virtual ~MyMeshImportARM(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual NxI32              getExtensionCount(void) { return 2; }; // most importers support just one file name extension.

  virtual const char * getExtension(NxI32 index)  // report the default file name extension for this mesh type.
  {
  	return index == 0 ? ".apx" : ".apb";
  }

  virtual const char * getDescription(NxI32 index)  // report the default file name extension for this mesh type.
  {
    return index == 0 ?  "APEX Render Mesh XML" : "APEX Render Mesh Binary";
  }

  virtual bool importMesh(const char *meshName,const void *data,NxU32 dlen,NVSHARE::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    NVSHARE::MeshImporter *mi = NVSHARE::createMeshImportARM();

    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options,appResource);
      NVSHARE::releaseMeshImportARM(mi);
    }

    return ret;
  }



};


enum MeshImportARMAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace NVSHARE;


static MyMeshImportARM *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportARM(NxI32 version_number)
#else
MESHIMPORTARM_API MeshImporter * getInterface(NxI32 version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportARM);
  }
  return static_cast<MeshImporter *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace NVSHARE;

bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    delete gInterface;
    gInterface = 0;
  }
  return ret;
}

using namespace NVSHARE;

#ifdef WIN32

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE ,
                       DWORD  ul_reason_for_call,
                       LPVOID )
{
  NxI32 ret = 0;

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

#endif
