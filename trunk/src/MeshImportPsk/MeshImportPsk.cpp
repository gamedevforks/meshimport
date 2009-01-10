#include <assert.h>
#include "./MeshImportPsk/MeshImportPsk.h"

#ifdef WIN32
#ifdef MESHIMPORTPSK_EXPORTS
#define MESHIMPORTPSK_API __declspec(dllexport)
#else
#define MESHIMPORTPSK_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTPSK_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORTPSK_API MESHIMPORTPSK::MeshImportPsk * getInterface(int version_number);
};

namespace MESHIMPORTPSK
{
class MyMeshImportPsk : public MeshImportPsk
{
public:
  MyMeshImportPsk(void)
  {
  }

  ~MyMeshImportPsk(void)
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


enum MeshImportPskAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORTPSK;


static MyMeshImportPsk *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImportPsk * getInterfaceMeshImportPsk(int version_number)
#else
MESHIMPORTPSK_API MeshImportPsk * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORTPSK_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportPsk);
  }
  return static_cast<MeshImportPsk *>(gInterface);
};

};  // End of namespace PATHPLANNING

#ifndef PLUGINS_EMBEDDED

using namespace MESHIMPORTPSK;

bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    MEMALLOC_DELETE(MeshImoprtPsk,gInterface);
    gInterface = 0;
  }
  return ret;
}

using namespace MESHIMPORTPSK;

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
