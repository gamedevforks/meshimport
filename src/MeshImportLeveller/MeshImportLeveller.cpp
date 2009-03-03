#include <assert.h>
#include "common/snippets/UserMemAlloc.h"
#include "MeshImport/MeshImport.h"

#ifdef WIN32
#ifdef MESHIMPORTLEVELLER_EXPORTS
#define MESHIMPORTLEVELLER_API __declspec(dllexport)
#else
#define MESHIMPORTLEVELLER_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTLEVELLER_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTLEVELLER_API MESHIMPORT::MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services);
};

namespace MESHIMPORT
{
class MyMeshImportLeveller : public MeshImporter
{
public:
  MyMeshImportLeveller(void)
  {
  }

  ~MyMeshImportLeveller(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual const char * getExtension(int index)  // report the default file name extension for this mesh type.
  {
    return ".ter";
  }

  virtual const char * getDescription(int index)
  {
    return "Leveller Files";
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    return ret;
  }



};


enum MeshImportLevellerAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORT;


static MyMeshImportLeveller *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportLeveller(int version_number,SYSTEM_SERVICES::SystemServices *services)
#else
MESHIMPORTLEVELLER_API MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services)
#endif
{
  if ( services )
  {
    SYSTEM_SERVICES::gSystemServices = services;
  }
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportLeveller);
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
    MEMALLOC_DELETE(MeshImportLeveller,gInterface);
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
