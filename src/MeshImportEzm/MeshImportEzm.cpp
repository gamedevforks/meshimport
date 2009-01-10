#include <assert.h>
#include "MeshImportEzm/MeshImportEzm.h"
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
MESHIMPORTEZM_API MESHIMPORTEZM::MeshImportEzm * getInterface(int version_number);
};

namespace MESHIMPORTEZM
{
class MyMeshImportEzm : public MeshImportEzm
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

  virtual const char * getExtension(void)  // report the default file name extension for this mesh type.
  {
    return ".ezm";
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    MeshImporter *mi = MESHIMPORT::createMeshImportEZM();
    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options);
      MESHIMPORT::releaseMeshImportEZM(mi);
    }

    return ret;
  }



};

};



using namespace MESHIMPORTEZM;


static MyMeshImportEzm *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImportEzm * getInterfaceMeshImportEzm(int version_number)
#else
MESHIMPORTEZM_API MeshImportEzm * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTEZM_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportEzm);
  }
  return static_cast<MeshImportEzm *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORTEZM;

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

using namespace MESHIMPORTEZM;

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

#endif
