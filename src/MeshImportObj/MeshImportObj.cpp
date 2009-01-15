#include <assert.h>
#include "./MeshImportObj/MeshImportObj.h"

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
MESHIMPORTOBJ_API MESHIMPORTOBJ::MeshImportObj * getInterface(int version_number);
};

namespace MESHIMPORTOBJ
{
class MyMeshImportObj : public MeshImportObj
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

  virtual const char * getExtension(int index)  // report the default file name extension for this mesh type.
  {
    return ".obj";
  }

  virtual const char * getDescription(int index)
  {
    return "Wavefront Obj Files";
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    return ret;
  }



};


enum MeshImportObjAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORTOBJ;


static MyMeshImportObj *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImportObj * getInterfaceMeshImportObj(int version_number)
#else
MESHIMPORTOBJ_API MeshImportObj * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTOBJ_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportObj);
  }
  return static_cast<MeshImportObj *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORTOBJ;

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

using namespace MESHIMPORTOBJ;

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
