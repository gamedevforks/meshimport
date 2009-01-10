#include <assert.h>
#include "./MeshImportOgre/MeshImportOgre.h"
#include "ImportOgre.h"

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
MESHIMPORTOGRE_API MESHIMPORTOGRE::MeshImportOgre * getInterface(int version_number);
};

namespace MESHIMPORTOGRE
{
class MyMeshImportOgre : public MeshImportOgre
{
public:
  MyMeshImportOgre(void)
  {
  }

  ~MyMeshImportOgre(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual const char * getExtension(void)  // report the default file name extension for this mesh type.
  {
    return ".xml";
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    MESHIMPORT::MeshImporter *mi = MESHIMPORT::createMeshImportOgre();

    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options);
      MESHIMPORT::releaseMeshImportOgre(mi);
    }

    return ret;
  }



};


enum MeshImportOgreAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORTOGRE;


static MyMeshImportOgre *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImportOgre * getInterfaceMeshImportOgre(int version_number)
#else
MESHIMPORTOGRE_API MeshImportOgre * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTOGRE_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportOgre);
  }
  return static_cast<MeshImportOgre *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORTOGRE;

bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    MEMALLOC_DELETE(MeshImportOgre,gInterface);
    gInterface = 0;
  }
  return ret;
}

using namespace MESHIMPORTOGRE;

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
