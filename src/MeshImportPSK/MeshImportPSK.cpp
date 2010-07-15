#include <assert.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportPSK.h"

#ifdef WIN32
#ifdef MESHIMPORTOGRE_EXPORTS
#define MESHIMPORTOGRE_API __declspec(dllexport)
#else
#define MESHIMPORTOGRE_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTOGRE_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTOGRE_API NVSHARE::MeshImporter * getInterface(NxI32 version_number);
};

namespace NVSHARE
{
class MyMeshImportPSK : public MeshImporter, public Memalloc
{
public:
  MyMeshImportPSK(void)
  {
  }

  virtual ~MyMeshImportPSK(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual NxI32              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.

  virtual const char * getExtension(NxI32 index)  // report the default file name extension for this mesh type.
  {
    return ".psk";
  }

  virtual const char * getDescription(NxI32 index)  // report the default file name extension for this mesh type.
  {
    return "PSK 3d Skeletal Mesh Files";
  }

  virtual bool importMesh(const char *meshName,const void *data,NxU32 dlen,NVSHARE::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    NVSHARE::MeshImporter *mi = NVSHARE::createMeshImportPSK();

    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options,appResource);
      NVSHARE::releaseMeshImportPSK(mi);
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


enum MeshImportPSKAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace NVSHARE;


static MyMeshImportPSK *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportPSK(NxI32 version_number)
#else
MESHIMPORTOGRE_API MeshImporter * getInterface(NxI32 version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportPSK);
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
