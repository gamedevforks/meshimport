#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImportBuilder.h"
#include "MeshImport/MeshSystem.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/stringdict.h"

#pragma warning(disable:4100)

namespace MESHIMPORT
{
class MySubMesh
{
public:
  unsigned int mVertexFlags;

  USER_STL::vector< MeshImportVertex > mVertices;
  USER_STL::vector< unsigned int >     mIndices;
};

class MyMesh
{
public:
  const char *mName;
  USER_STL::vector< MySubMesh > mSubMeshes;
};

typedef USER_STL::vector< MeshImportMaterial > MeshImportMaterialVector;
typedef USER_STL::vector< MyMesh >             MeshImportMeshVector;

class MyMeshImportBuilder : public MeshImportBuilder, public MESHIMPORT::MeshImportInterface
{
public:
  MyMeshImportBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options)
  {
    mCurmesh     = -1;
    mCurMaterial = -1;

    mi->importMesh(meshName,data,dlen,this,options);
    
    // todo..
    mMaterialCount = mMyMaterials.size();
    mMaterials     = &mMyMaterials[0];

    mMeshCount = mMyMeshes.size();
    mMeshes = MEMALLOC_NEW_ARRAY(Mesh, mMeshCount)[mMeshCount];

    for (int i = 0; i < (int) mMeshCount; i++)
    {
      Mesh *outmesh  = &mMeshes[i];
      MyMesh *inmesh = &mMyMeshes[i];
      
      //outmesh->mSkeleton TODO
      
      outmesh->mSubMeshCount = inmesh->mSubMeshes.size();
      for (int c = 0; c < (int) outmesh->mSubMeshCount; c++)
      {
        SubMesh *outsub  = &outmesh->mSubMeshes[i];
        MySubMesh *insub = &inmesh->mSubMeshes[i];

        outsub->mTriCount = insub->mIndices.size() / 3;
        outsub->mIndices = &insub->mIndices[0];
        outsub->mVertexCount = insub->mVertices.size();
        outsub->mVertices = &insub->mVertices[0];
        
        outsub->mMaterial = &mMyMaterials[c];
        // TODO: collapse unused materials
        
        //TODO AABB
        //outsub->mAABB
      }
    }
  }

  ~MyMeshImportBuilder(void)
  {
  }

  virtual MaterialRef createMaterialRef(const char *matName)
  {
    // Search for duplicate materials by name, not ideal.
    StringRef m = mStrings.Get(matName);

    unsigned int i;
    for (i = 0; i < mMyMaterials.size(); i++)
    {
      //[rgd] ask john best method for compare
      if ( stricmp(matName, mMyMaterials[i].mName) == 0 )
        return i;
    }
    
    MeshImportMaterial mat;
    mat.mName = m.Get();
    mat.mMetaData = mStrings.Get("");
    mMyMaterials.push_back(mat);

    return mCurMaterial = (MaterialRef) i;  
  }

  virtual MeshRef createMeshRef(const char *meshName)
  {
    // no dupe check for now
    mCurmesh++;
    mMyMeshes.resize(mMyMeshes.size() + 1);
    
    return mCurmesh;
  }

  virtual void importAssetName(const char *assetName,const char *info)
  {
  }

  virtual MaterialRef importMaterial(const char *matName,const char *metaData)
  {
    StringRef m = mStrings.Get(matName);
    StringRef d = mStrings.Get(metaData);

    // Search and set duplicate materials by name
    unsigned int i;
    for (i = 0; i < mMyMaterials.size(); i++)
    {
      //[rgd] ask john best method for compare
      if ( stricmp(matName, mMyMaterials[i].mName) == 0 )
      {
        if ( strlen( mMyMaterials[i].mMetaData ) > 0  
          && stricmp(metaData, mMyMaterials[i].mMetaData) != 0 )
        {
          // ACK non-equal collision, do something
        }
        mMyMaterials[i].mMetaData = d.Get();
        return mCurMaterial = (MaterialRef) i;
      }
    }

    // Otherwise create a new one.
    MeshImportMaterial mat;
    mat.mName = m.Get();
    mat.mMetaData = d.Get();
    mMyMaterials.push_back(mat);
    
    return mCurMaterial = (MaterialRef) i;
  }

  virtual void importTriangle(unsigned int vertexFlags,const MeshImportVertex verts[3])
  {
    assert( mCurmesh >= 0);
    assert( mCurMaterial >= 0);

    importTriangle(mCurmesh, mCurMaterial, vertexFlags, verts);
  }

  virtual void importTriangle(MeshRef meshref, MaterialRef matref, unsigned int vertexFlags,const MeshImportVertex verts[3])
  {
    MyMesh    * curMesh = NULL;
    MySubMesh * subMesh = NULL;
    
    assert(meshref < (int) mMyMeshes.size());
    
    curMesh = &mMyMeshes[meshref];
    if (matref >= (int) curMesh->mSubMeshes.size())
    {
      curMesh->mSubMeshes.resize(matref + 1);
      curMesh->mSubMeshes[matref].mVertexFlags = vertexFlags; // first vertex added sets flags
    }

    subMesh = &curMesh->mSubMeshes[matref];

    int indices[3] = {-1, -1, -1};

    // weld duplicate vertices
    unsigned int i;
    for (i = 0; i < subMesh->mVertices.size(); i++)
    {
      if (memcmp(&subMesh->mVertices[i], &verts[0], sizeof(MeshImportVertex)) == 0)
        indices[0] = i;
      if (memcmp(&subMesh->mVertices[i], &verts[1], sizeof(MeshImportVertex)) == 0)
        indices[1] = i;
      if (memcmp(&subMesh->mVertices[i], &verts[2], sizeof(MeshImportVertex)) == 0)
        indices[2] = i;
    }

    for (i = 0; i < 3; i++)
    {
      if (indices[i] == -1)
      {
        indices[i] = subMesh->mVertices.size();
        subMesh->mVertices.push_back(verts[i]);
      }

      subMesh->mIndices.push_back(indices[i]);
    }
  }
  
  virtual void importAnimation(const MeshImportAnimation &animation)
  {
  }

  virtual void importSkeleton(const MeshImportSkeleton &skeleton)
  {
  }

  virtual void importRawTexture(const char *textureName,const unsigned char *pixels,unsigned int wid,unsigned int hit)
  {
  }

  virtual void importMeshInstance(const char *meshName,const float pos[3],const float rotation[4],const float scale[3])
  {

  }


private:
  StringDict  mStrings;
  MeshImportMaterialVector mMyMaterials;
  MeshImportMeshVector     mMyMeshes;

  MeshRef     mCurmesh;
  MaterialRef mCurMaterial;
};

MeshImportBuilder * createMeshImportBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options)
{
  MyMeshImportBuilder *b = MEMALLOC_NEW(MyMeshImportBuilder)(meshName,data,dlen,mi,options);
  return static_cast< MeshImportBuilder *>(b);
}

void          releaseMeshImportBuilder(MeshImportBuilder *m)
{
  MyMeshImportBuilder *b = static_cast< MyMeshImportBuilder *>(m);
  MEMALLOC_DELETE(MyMsehImportBuilder,b);
}

}; // end of namesapace
