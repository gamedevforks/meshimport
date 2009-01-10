#include <assert.h>
#include "./MeshImportFbx/MeshImportFbx.h"

#ifdef WIN32
#ifdef MESHIMPORTFBX_EXPORTS
#define MESHIMPORTFBX_API __declspec(dllexport)
#else
#define MESHIMPORTFBX_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTFBX_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTFBX_API MESHIMPORTFBX::MeshImportFbx * getInterface(int version_number);
};

namespace MESHIMPORTFBX
{
class MyMeshImportFbx : public MeshImportFbx
{
public:
  MyMeshImportFbx(void)
  {
  }

  ~MyMeshImportFbx(void)
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


enum MeshImportFbxAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORTFBX;


static MyMeshImportFbx *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImportFbx * getInterfaceMeshImportFbx(int version_number)
#else
MESHIMPORTFBX_API MeshImportFbx * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTFBX_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportFbx);
  }
  return static_cast<MeshImportFbx *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORTFBX;

bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    MEMALLOC_DELETE(MeshImportFbx,gInterface);
    gInterface = 0;
  }
  return ret;
}

using namespace MESHIMPORTFBX;

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
