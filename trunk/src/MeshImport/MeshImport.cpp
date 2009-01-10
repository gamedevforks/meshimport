#pragma warning(disable:4702) // disabling a warning that only shows up when building VC7
#include <assert.h>
#include <vector>

#include "./MeshImport/MeshImport.h"
#include "MeshImportBuilder.h"
#include "common/snippets/UserMemAlloc.h"

#pragma warning(disable:4996)


#ifdef WIN32
#ifdef MESHIMPORT_EXPORTS
#define MESHIMPORT_API __declspec(dllexport)
#else
#define MESHIMPORT_API __declspec(dllimport)
#endif
#else
#define MESHIMPORT_API
#endif

#pragma warning(disable:4100)

bool doShutdown(void);

extern "C"
{
MESHIMPORT_API MESHIMPORT::MeshImport * getInterface(int version_number);
};

namespace MESHIMPORT
{


typedef USER_STL::vector< MeshImporter * > MeshImporterVector;

class MyMeshImport : public MeshImport
{
public:
  MyMeshImport(void)
  {
  }

  ~MyMeshImport(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual MeshImporter *   locateMeshImporter(const char *fname) // based on this file name, find a matching mesh importer.
  {
    MeshImporter *ret = 0;

    char scratch[512];
    strncpy(scratch,fname,512);
    MeshImporterVector::iterator i;
    for (i=mImporters.begin(); i!=mImporters.end(); ++i)
    {
      MeshImporter *mi = (*i);
      const char *e = mi->getExtension();
      if ( strstr(scratch,e) )
      {
        ret = mi;
        break;
      }
    }
    return ret;
  }

  virtual void addImporter(MeshImporter *importer)  // add an additional importer
  {
    mImporters.push_back(importer);
  }

  bool importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    MeshImporter *mi = locateMeshImporter(meshName);
    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options);
    }

    return ret;
  }

  virtual MeshSystem *     createMeshSystem(const char *meshName,const void *data,unsigned int dlen,const char *options) // imports and converts to a single MeshSystem data structure
  {
    MeshSystem *ret = 0;

    MeshImporter *mi = locateMeshImporter(meshName);
    if ( mi )
    {
      MeshImportBuilder *b = createMeshImportBuilder(meshName,data,dlen,mi,options);
      if ( b )
      {
        ret = static_cast< MeshSystem * >(b);
      }
    }

    return ret;
  }

  virtual void             releaseMeshSystem(MeshSystem *mesh)
  {
    MeshImportBuilder *b = static_cast< MeshImportBuilder *>(mesh);
    releaseMeshImportBuilder(b);
  }


private:
  MeshImporterVector  mImporters;

};

};  // End of Namespace


using namespace MESHIMPORT;


static MyMeshImport *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImport * getInterfaceMeshImport(int version_number)
#else
MESHIMPORT_API MeshImport * getInterface(int version_number)
#endif
{
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImport);
  }
  return static_cast<MeshImport *>(gInterface);
};

};  // End of namespace PATHPLANNING

using namespace MESHIMPORT;

#ifndef PLUGINS_EMBEDDED
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
