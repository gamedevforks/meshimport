#include <assert.h>
#include "./MeshImportAssimp/MeshImportAssimp.h"

#ifdef WIN32
#ifdef MESHIMPORTASSIMP_EXPORTS
#define MESHIMPORTASSIMP_API __declspec(dllexport)
#else
#define MESHIMPORTASSIMP_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTASSIMP_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTASSIMP_API MESHIMPORTASSIMP::MeshImportAssimp * getInterface(int version_number);
};

namespace MESHIMPORTASSIMP
{
class MyMeshImportAssimp : public MeshImportAssimp
{
public:
  MyMeshImportAssimp(void)
  {
  }

  ~MyMeshImportAssimp(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual const char * getExtension(void)  // report the default file name extension for this mesh type.
  {
    return ".hms";
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    return ret;
  }



};


enum MeshImportAssimpAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORTASSIMP;


static MyMeshImportAssimp *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImportAssimp * getInterfaceMeshImportAssimp(int version_number)
#else
MESHIMPORTASSIMP_API MeshImportAssimp * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTASSIMP_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportAssimp);
  }
  return static_cast<MeshImportAssimp *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORTASSIMP;

bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    MEMALLOC_DELETE(MeshImportAssimp,gInterface);
    gInterface = 0;
  }
  return ret;
}

using namespace MESHIMPORTASSIMP;

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
