#include <assert.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportObj.h"

#ifdef WIN32
#ifdef MESHIMPORTOBJ_EXPORTS
#define MESHIMPORTOBJ_API __declspec(dllexport)
#else
#define MESHIMPORTOBJ_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTOBJ_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTOBJ_API NVSHARE::MeshImporter * getInterface(PxI32 version_number);
};

namespace NVSHARE
{
class MyMeshImportObj : public MeshImporter, public Memalloc
{
public:
  MyMeshImportObj(void)
  {
  }

  virtual ~MyMeshImportObj(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual const char * getExtension(PxI32 index)  // report the default file name extension for this mesh type.
  {
    return ".obj";
  }

  virtual const char * getDescription(PxI32 index)
  {
    return "Wavefront Obj Files";
  }

  virtual bool importMesh(const char *meshName,const void *data,PxU32 dlen,NVSHARE::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    MeshImporter *imp = createMeshImportObj();
    if ( imp )
    {
      ret = imp->importMesh(meshName,data,dlen,callback,options,appResource);
      releaseMeshImportObj(imp);
    }

    return ret;
  }



}; // end of namespace


enum MeshImportObjAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace NVSHARE;


static MyMeshImportObj *gInterface=0;

extern "C"
{
MESHIMPORTOBJ_API MeshImporter * getInterface(PxI32 version_number)
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportObj);
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
  PxI32 ret = 0;

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
