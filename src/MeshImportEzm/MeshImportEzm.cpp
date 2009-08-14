#include <assert.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportEzm.h"
#include "SystemServices.h"

#ifdef WIN32
#ifdef MESHIMPORTEZM_EXPORTS
#define MESHIMPORTEZM_API __declspec(dllexport)
#else
#define MESHIMPORTEZM_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTEZM_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTEZM_API MESHIMPORT::MeshImporter * getInterface(NxI32 version_number,SYSTEM_SERVICES::SystemServices *services);
};

namespace MESHIMPORT
{
class MyMeshImportEzm : public MeshImporter
{
public:
  MyMeshImportEzm(void)
  {
  }

  ~MyMeshImportEzm(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual const char * getExtension(NxI32 index)  // report the default file name extension for this mesh type.
  {
    return ".ezm";
  }

  virtual const char * getDescription(NxI32 index)
  {
    return "PhysX Rocket EZ-Mesh format";
  }


  virtual bool importMesh(const char *meshName,const void *data,NxU32 dlen,MESHIMPORT::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    MeshImporter *mi = MESHIMPORT::createMeshImportEZM();
    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options,appResource);
      MESHIMPORT::releaseMeshImportEZM(mi);
    }

    return ret;
  }



};

};



using namespace MESHIMPORT;


static MyMeshImportEzm *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportEzm(NxI32 version_number,SYSTEM_SERVICES::SystemServices *services)
#else
MESHIMPORTEZM_API MeshImporter * getInterface(NxI32 version_number,SYSTEM_SERVICES::SystemServices *services)
#endif
{
  if ( services )
  {
    SYSTEM_SERVICES::gSystemServices = services;
  }
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportEzm);
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
    MEMALLOC_DELETE(MeshimImportEzm,gInterface);
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
