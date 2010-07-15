#include <assert.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportEzm.h"

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
MESHIMPORTEZM_API NVSHARE::MeshImporter * getInterface(NxI32 version_number);
};

namespace NVSHARE
{
class MyMeshImportEzm : public MeshImporter, public Memalloc
{
public:
  MyMeshImportEzm(void)
  {
  }

  virtual ~MyMeshImportEzm(void)
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


  virtual bool importMesh(const char *meshName,const void *data,NxU32 dlen,NVSHARE::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    MeshImporter *mi = NVSHARE::createMeshImportEZM();
    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options,appResource);
      NVSHARE::releaseMeshImportEZM(mi);
    }

    return ret;
  }

  virtual const void * saveMeshSystem(MeshSystem *ms,NxU32 &dlen,bool binary) 
  {
	  return NULL;
  }

  virtual void releaseSavedMeshSystem(const void *mem) 
  {

  }


};

};



using namespace NVSHARE;


static MyMeshImportEzm *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportEzm(NxI32 version_number)
#else
MESHIMPORTEZM_API MeshImporter * getInterface(NxI32 version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportEzm);
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
