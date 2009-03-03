#include <assert.h>
#include "common/snippets/UserMemAlloc.h"
#include "MeshImport/MeshImport.h"
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
MESHIMPORTOGRE_API MESHIMPORT::MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services);
};

namespace MESHIMPORT
{
class MyMeshImportOgre : public MeshImporter
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

  virtual int              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.

  virtual const char * getExtension(int index)  // report the default file name extension for this mesh type.
  {
    return ".xml";
//    if ( index == 0 )
//      return ".mesh.xml";
//    else
//      return ".skeleton.xml";
  }

  virtual const char * getDescription(int index)  // report the default file name extension for this mesh type.
  {
//    if ( index == 0 )
      return "Ogre3d XML Mesh Files";
//    else
//      return "Ogre3d XML Skeleton Files";
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    MESHIMPORT::MeshImporter *mi = MESHIMPORT::createMeshImportOgre();

    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options,appResource);
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


using namespace MESHIMPORT;


static MyMeshImportOgre *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportOgre(int version_number,SYSTEM_SERVICES::SystemServices *services)
#else
MESHIMPORTOGRE_API MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services)
#endif
{
  if ( services )
  {
    SYSTEM_SERVICES::gSystemServices = services;
  }
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportOgre);
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
    MEMALLOC_DELETE(MeshImportOgre,gInterface);
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
