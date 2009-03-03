#include <assert.h>
#include <vector>
#include "common/snippets/UserMemAlloc.h"
#include "MeshImport/MeshImport.h"
#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"

#ifdef WIN32
#ifdef MESHIMPORTASSIMP_EXPORTS
#define MESHIMPORTASSIMP_API __declspec(dllexport)
#else
#define MESHIMPORTASSIMP_API __declspec(dllimport)
#endif
#else
#define MESHIMPORTASSIMP_API
#endif

#pragma warning(disable:4100 4996)

bool doShutdown(void);

extern "C"
{
  MESHIMPORTASSIMP_API MESHIMPORT::MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services);
};

namespace MESHIMPORT
{
typedef USER_STL::vector< std::string > StringVector;

class MyMeshImportAssimp : public MeshImporter
{
public:
  MyMeshImportAssimp(void)
  {
    C_STRUCT aiString a;
    aiGetExtensionList(&a);
    const char *scan = a.data;
    while ( *scan )
    {
      if ( *scan == '*' ) 
      {
        scan++;
        char temp[512];
        char *dest = temp;
        while ( *scan && *scan != ';' )
        {
          *dest++ = *scan++;
        }
        *dest = 0;
        std::string str = temp;

        bool skip = false;

        if ( stricmp(temp,".obj") == 0 )
        {
          skip = true;
        }
        else if ( stricmp(temp,".xml") == 0 )
        {
          skip = true;
        }
        else if ( stricmp(temp,".ezm") == 0 )
        {
          skip = true;
        }
        else if ( stricmp(temp,".gr2") == 0 )
        {
          skip = true;
        }
        else if ( stricmp(temp,".spt") == 0 )
        {
          skip = true;
        }
        if ( !skip )
        {
          mExtensions.push_back(str);
          char scratch[512];
          char type[512];
          strcpy(type,temp);
          strlwr(type);
          if ( strcmp(type,".dae") == 0 )
          {
            strcpy(type,"Collada");
          }
          else if ( strcmp(type,".3ds") == 0 )
          {
            strcpy(type,"3D Studio Max 3DS");
          }
          else if ( strcmp(type,".ase") == 0 )
          {
            strcpy(type,"3D Studio Max ASE");
          }
          else if ( strcmp(type,".obj") == 0 )
          {
            strcpy(type,"Wavefront Object");
          }
          else if ( strcmp(type,".ply") == 0 )
          {
            strcpy(type,"Stanford Polygon Library");
          }
          else if ( strcmp(type,".dxf") == 0 )
          {
            strcpy(type,"AutoCAD DXF");
          }
          else if ( strcmp(type,".lwo") == 0 )
          {
            strcpy(type,"Lightwave");
          }
          else if ( strcmp(type,".stl") == 0 )
          {
            strcpy(type,"Sterolithography");
          }
          else if ( strcmp(type,".ac") == 0 )
          {
            strcpy(type,"AC3D");
          }
          else if ( strcmp(type,".smd") == 0 )
          {
            strcpy(type,"Valve Model");
          }
          else if ( strcmp(type,".vta") == 0 )
          {
            strcpy(type,"Valve Model");
          }
          else if ( strcmp(type,".mdl") == 0 )
          {
            strcpy(type,"Quake I");
          }
          else if ( strcmp(type,".mdl2") == 0 )
          {
            strcpy(type,"Quake II");
          }
          else if ( strcmp(type,".mdl3") == 0 )
          {
            strcpy(type,"Quake III");
          }
          else if ( strcmp(type,".mdc") == 0 )
          {
            strcpy(type,"Return to Castle Wolfenstein");
          }
          else if ( strcmp(type,".md5") == 0 )
          {
            strcpy(type,"Doom 3");
          }
          else if ( strcmp(type,".bvh") == 0 )
          {
            strcpy(type,"Biovision BVH");
          }
          else if ( strcmp(type,".x") == 0 )
          {
            strcpy(type,"DirectX");
          }
          else if ( strcmp(type,".q3d") == 0 )
          {
            strcpy(type,"Quick3D");
          }
          else if ( strcmp(type,".irrmesh") == 0 )
          {
            strcpy(type,"Irrlicht Mesh");
          }
          else if ( strcmp(type,".nff") == 0 )
          {
            strcpy(type,"Neutral File Format");
          }
          else if ( strcmp(type,".nff") == 0 )
          {
            strcpy(type,"Sense8 WorldToolkit");
          }
          else if ( strcmp(type,".off") == 0 )
          {
            strcpy(type,"Object File Format");
          }
          else if ( strcmp(type,".raw") == 0 )
          {
            strcpy(type,"PovRAY Raw");
          }
          else if ( strcmp(type,".ter") == 0 )
          {
            strcpy(type,"Terragen Terrain");
          }
          else if ( strcmp(type,".mdl") == 0 )
          {
            strcpy(type,"3D GameStudio");
          }
          else if ( strcmp(type,".hmp") == 0 )
          {
            strcpy(type,"3D GameStudio Terrain");
          }


          sprintf(scratch,"OpenAssetImport - %s", type );
          str = scratch;
          mDescriptions.push_back(str);
        }
      }
      scan++;
    }
  }

  ~MyMeshImportAssimp(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  virtual int   getExtensionCount(void) 
  {
    return mExtensions.size();
  }

  virtual const char * getExtension(int index)  // report the default file name extension for this mesh type.
  {
    const char *ret = 0;
    if ( index >= 0 && index < (int)mExtensions.size() )
    {
      ret = mExtensions[index].c_str();
    }
    return ret;
  }

  virtual const char * getDescription(int index)  // report the default file name extension for this mesh type.
  {
    const char *ret = 0;
    if ( index >= 0 && index < (int)mDescriptions.size() )
    {
      ret = mDescriptions[index].c_str();
    }
    return ret;
  }

  void importMesh(const aiMesh *m,MESHIMPORT::MeshImportInterface *callback)
  {
    if ( m->HasFaces() && m->HasPositions() )
    {
      unsigned int vcount = m->mNumVertices;
      MeshVertex *vertices = MEMALLOC_NEW_ARRAY(MeshVertex,vcount)[vcount];

      MeshVertex *dest = vertices;
      const aiVector3D *vsource = m->mVertices;

      for (unsigned int i=0; i<vcount; i++)
      {
        dest->mPos[0] = vsource->x;
        dest->mPos[1] = vsource->y;
        dest->mPos[2] = vsource->z;

        vsource++;
        dest++;
      }

      const aiFace * faces = m->mFaces;
      for (unsigned int i=0; i<m->mNumFaces; i++)
      {
        assert( faces->mNumIndices == 3 );
        if ( faces->mNumIndices == 3 )
        {
          unsigned int i1 = faces->mIndices[0];
          unsigned int i2 = faces->mIndices[1];
          unsigned int i3 = faces->mIndices[2];
          const MeshVertex &v1 = vertices[i1];
          const MeshVertex &v2 = vertices[i2];
          const MeshVertex &v3 = vertices[i3];
          callback->importTriangle("foo","foo",MIVF_ALL,v1,v2,v3);
        }

        faces++;
      }


      MEMALLOC_DELETE_ARRAY(MeshVertex,vertices);


    }
  }

  virtual bool importMesh(const char *meshName,const void *data,unsigned int dlen,MESHIMPORT::MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    const C_STRUCT aiScene *scene = aiImportFile(meshName,(unsigned int)(aiProcess_ConvertToLeftHanded | aiProcess_RemoveRedundantMaterials | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals));
    if ( scene )
    {
      for (unsigned int i=0; i<scene->mNumMeshes; i++)
      {
        const aiMesh *m = scene->mMeshes[i];
        importMesh(m,callback);
      }
    }

    return ret;
  }


private:
  StringVector  mExtensions;
  StringVector  mDescriptions;
};


enum MeshImportAssimpAPI
{
  PPA_TEST,
};

};  // End of Namespace


using namespace MESHIMPORT;


static MyMeshImportAssimp *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImporter * getInterfaceMeshImportAssimp(int version_number,SYSTEM_SERVICES::SystemServices *services)
#else
MESHIMPORTASSIMP_API MeshImporter * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services)
#endif
{
  if ( services )
  {
    SYSTEM_SERVICES::gSystemServices = services;
  }
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == MESHIMPORT_VERSION )
  {
    gInterface = MEMALLOC_NEW(MyMeshImportAssimp);
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
    MEMALLOC_DELETE(MeshImportAssimp,gInterface);
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
