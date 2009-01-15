#pragma warning(disable:4702) // disabling a warning that only shows up when building VC7
#include <assert.h>
#include <vector>

#include "./MeshImport/MeshImport.h"
#include "MeshImportBuilder.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/FileInterface/FileInterface.h"

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
      MeshBuilder *b = createMeshBuilder(meshName,data,dlen,mi,options);
      if ( b )
      {
        ret = static_cast< MeshSystem * >(b);
      }
    }

    return ret;
  }

  virtual void             releaseMeshSystem(MeshSystem *mesh)
  {
    MeshBuilder *b = static_cast< MeshBuilder *>(mesh);
    releaseMeshBuilder(b);
  }


  virtual int              getImporterCount(void)
  {
    return mImporters.size();
  }

  virtual MeshImporter    *getImporter(int index)
  {
    MeshImporter *ret = 0;
    assert( index >=0 && index < (int)mImporters.size() );
    if ( index >= 0 && index < (int)mImporters.size() )
    {
      ret = mImporters[index];
    }
    return ret;
  }

  const char *getStr(const char *str)
  {
    if ( str == 0 ) str = "";
    return str;
  }

  void printAABB(FILE_INTERFACE *fph,const MeshAABB &a)
  {
    fi_fprintf(fph,"       <MeshAABB min=\"%0.9f,%0.9f,%0.9f\" max=\"%0.9f,%0.9f,%0.9f\"/>\r\n", a.mMin[0], a.mMin[1], a.mMin[2], a.mMax[0], a.mMax[1], a.mMax[2] );
  }

  void print(FILE_INTERFACE *fph,MeshRawTexture *t)
  {
    assert(0); // not yet implemented
  }

  void print(FILE_INTERFACE *fph,MeshTetra *t)
  {
    assert(0); // not yet implemented
  }

  void print(FILE_INTERFACE *fph,MeshSkeleton *s)
  {
    assert(0);
  }

  void print(FILE_INTERFACE *fph,MeshAnimation *a)
  {
    assert(0);
  }

  void print(FILE_INTERFACE *fph,const MeshMaterial &m)
  {
    fi_fprintf(fph,"      <Material name=%s meta_data=%s/>\r\n", m.mName, m.mMetaData );
  }

  virtual void            *serializeMeshSystem(MeshSystem *mesh,unsigned int &len,MeshSerializeFormat format)
  {
    void * ret = 0;
    len = 0;

    FILE_INTERFACE *fph = fi_fopen("foo", "wmem", 0, 0);

    if ( fph )
    {
      fi_fprintf(fph,"<?xml version=\"1.0f\"?>\r\n");
      fi_fprintf(fph,"  <MeshSystem asset_name=\"%s\" asset_info=\"%s\" mesh_system_version=%d mesh_system_asset_version=%d>\r\n", getStr(mesh->mAssetName), getStr(mesh->mAssetInfo), mesh->mMeshSystemVersion, mesh->mAssetVersion );
      printAABB(fph,mesh->mAABB);

      fi_fprintf(fph,"    <Textures count=%d>\r\n", mesh->mTextureCount );
      for (unsigned int i=0; i<mesh->mTextureCount; i++)
      {
        print(fph,mesh->mTextures[i]);
      }
      fi_fprintf(fph,"    </Textures>\r\n");

      fi_fprintf(fph,"   <TetraMeshes count=%d>\r\n", mesh->mTetraMeshCount );
      for (unsigned int i=0; i<mesh->mTetraMeshCount; i++)
      {
        print(fph,mesh->mTetraMeshes[i]);
      }
      fi_fprintf(fph,"   </TetraMeshes>\r\n");

      fi_fprintf(fph,"    <Skeletons count=%d>\r\n");

      for (unsigned int i=0; i<mesh->mSkeletonCount; i++)
      {
        print(fph,mesh->mSkeletons[i]);
      }

      fi_fprintf(fph,"    </Skeletons>\r\n");

      fi_fprintf(fph,"    <Animations count=%d>\r\n", mesh->mAnimationCount );
      for (unsigned int i=0; i<mesh->mAnimationCount; i++)
      {
        print(fph,mesh->mAnimations[i]);
      }
      fi_fprintf(fph,"    </Animations>\r\n");

      fi_fprintf(fph,"    <Materials count=%d>\r\n", mesh->mMaterialCount );
      for (unsigned int i=0; i<mesh->mMaterialCount; i++)
      {
        print(fph,mesh->mMaterials[i]);
      }



      fi_fprintf(fph,"  </MeshSystem>\r\n");

      size_t olen;
      void *temp = fi_getMemBuffer(fph,&olen);
      if ( temp )
      {
        ret = MEMALLOC_MALLOC(olen);
        memcpy(ret,temp,olen);
        len = olen;
      }
      fi_fclose(fph);
    }

    return ret;
  }

  virtual  void             releaseSerializeMemory(void *data)
  {
    MEMALLOC_FREE(data);
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
