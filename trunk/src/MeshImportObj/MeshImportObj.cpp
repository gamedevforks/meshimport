#include <assert.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportObj.h"
#include "SystemServices.h"

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
MESHIMPORTOBJ_API MESHIMPORT::MeshImporter * getInterface(NxI32 version_number,SYSTEM_SERVICES::SystemServices *services);
};

namespace MESHIMPORT
{
class MyMeshImportObj : public MeshImporter
{
public:
  MyMeshImportObj(void)
  {
  }

  ~MyMeshImportObj(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual const char * getExtension(NxI32 index)  // report the default file name extension for this mesh type.
  {
    return ".obj";
  }

  virtual const char * getDescription(NxI32 index)
  {
    return "Wavefront Obj Files";
  }

  virtual bool importMesh(const char *meshName,const void *data,NxU32 dlen,MESHIMPORT::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
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



};


enum MeshImportObjAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORT;


static MyMeshImportObj *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportObj(NxI32 version_number,SYSTEM_SERVICES::SystemServices *services)
#else
MESHIMPORTOBJ_API MeshImporter * getInterface(NxI32 version_number,SYSTEM_SERVICES::SystemServices *services)
#endif
{
  if ( services )
  {
    SYSTEM_SERVICES::gSystemServices = services;
  }
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportObj);
  }
  return static_cast<MeshImporter *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORT;

bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    MEMALLOC_DELETE(MeshImportObj,gInterface);
    gInterface = 0;
  }
  return ret;
}

using namespace MESHIMPORT;

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
