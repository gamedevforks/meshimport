#include <assert.h>
#include "./MeshImportLeveller/MeshImportLeveller.h"

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
MESHIMPORTLEVELLER_API MESHIMPORTLEVELLER::MeshImportLeveller * getInterface(int version_number);
};

namespace MESHIMPORTLEVELLER
{
class MyMeshImportLeveller : public MeshImportLeveller
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

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options)
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


using namespace MESHIMPORTLEVELLER;


static MyMeshImportLeveller *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImportLeveller * getInterfaceMeshImportLeveller(int version_number)
#else
MESHIMPORTLEVELLER_API MeshImportLeveller * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTLEVELLER_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportLeveller);
  }
  return static_cast<MeshImportLeveller *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORTLEVELLER;

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

using namespace MESHIMPORTLEVELLER;

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
