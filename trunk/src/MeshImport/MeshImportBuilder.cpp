#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImportBuilder.h"
#include "VtxWeld.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/stringdict.h"
#include "common/snippets/sutil.h"

#pragma warning(disable:4100 4189)

namespace MESHIMPORT
{

typedef USER_STL::vector< MeshVertex >   MeshVertexVector;
typedef USER_STL::vector< unsigned int > MeshIndexVector;
typedef USER_STL::vector< SubMesh * >    SubMeshVector;
typedef USER_STL::vector< Mesh * >       MeshVector;
typedef USER_STL::vector< MeshAnimation * > MeshAnimationVector;
typedef USER_STL::vector< MeshSkeleton * > MeshSkeletonVector;

static int gSerializeFrame=1;

class MyMesh;

class MySubMesh : public SubMesh
{
public:
  MySubMesh(const char *mat,unsigned int vertexFlags)
  {
    mMaterialName = mat;
    mVertexFlags  = (MeshVertexFlag)vertexFlags;
  }

  ~MySubMesh(void)
  {
  }

  bool isSame(const char *mat,unsigned int vertexFlags) const
  {
    bool ret = false;
    assert(mat);
    assert(mMaterialName);
    if ( strcmp(mat,mMaterialName) == 0 && mVertexFlags == vertexFlags ) ret = true;
    return ret;
  }

  void gather(void)
  {
    mTriCount = mMyIndices.size()/3;
    mIndices  = &mMyIndices[0];
  }

  void add(const MeshVertex &v1,const MeshVertex &v2,const MeshVertex &v3,VertexPool< MeshVertex > &vpool)
  {
    mAABB.include(v1.mPos);
    mAABB.include(v2.mPos);
    mAABB.include(v3.mPos);

    unsigned int i1 = vpool.GetVertex(v1);
    unsigned int i2 = vpool.GetVertex(v2);
    unsigned int i3 = vpool.GetVertex(v3);

    mMyIndices.push_back(i1);
    mMyIndices.push_back(i2);
    mMyIndices.push_back(i3);
  }

  MeshIndexVector          mMyIndices;
  VertexPool< MeshVertex > mVertexPool;
};

class MyMesh : public Mesh
{
public:
  MyMesh(const char *meshName,const char *skeletonName)
  {
    mName = meshName;
    mSkeletonName = skeletonName;
    mCurrent = 0;
  }

  ~MyMesh(void)
  {
    release();
  }

  void release(void)
  {
    mSubMeshes = 0;
    mSubMeshCount = 0;
    SubMeshVector::iterator i;
    for (i=mMySubMeshes.begin(); i!=mMySubMeshes.end(); ++i)
    {
      MySubMesh *s = static_cast<MySubMesh *>((*i));
      delete s;
    }
    mMySubMeshes.clear();
  }

  bool isSame(const char *meshName) const
  {
    bool ret = false;
    assert(meshName);
    assert(mName);
    if ( strcmp(mName,meshName) == 0 ) ret = true;
    return ret;
  }

  void getCurrent(const char *materialName,unsigned int vertexFlags)
  {
    if ( materialName == 0 ) materialName = "default_material";
    if ( mCurrent == 0 || !mCurrent->isSame(materialName,vertexFlags) )
    {
      mCurrent =0;
      SubMeshVector::iterator i;
      for (i=mMySubMeshes.begin(); i!=mMySubMeshes.end(); ++i)
      {
        MySubMesh *s = static_cast< MySubMesh *>((*i));
        if ( s->isSame(materialName,vertexFlags) )
        {
          mCurrent = s;
          break;
        }
      }
      if ( mCurrent == 0 )
      {
        mCurrent = MEMALLOC_NEW(MySubMesh)(materialName,vertexFlags);
        mMySubMeshes.push_back(mCurrent);
      }
    }
  }

  virtual void        importTriangle(const char *materialName,
                                     unsigned int vertexFlags,
                                     const MeshVertex &v1,
                                     const MeshVertex &v2,
                                     const MeshVertex &v3)
  {
    mAABB.include( v1.mPos );
    mAABB.include( v2.mPos );
    mAABB.include( v3.mPos );
    getCurrent(materialName,vertexFlags);
    mVertexFlags|=vertexFlags;

    mCurrent->add(v1,v2,v3,mVertexPool);
#if 0
    if ( stristr(materialName,"cape") )
    {
      MeshVertex tv1 = v1;
      MeshVertex tv2 = v2;
      MeshVertex tv3 = v3;

      tv1.mNormal[0]*=-1;
      tv1.mNormal[1]*=-1;
      tv1.mNormal[2]*=-1;
      tv1.mTexel1[0] = 0;
      tv1.mTexel1[1] = 0;

      tv2.mNormal[0]*=-1;
      tv2.mNormal[1]*=-1;
      tv2.mNormal[2]*=-1;
      tv2.mTexel1[0] = 0;
      tv2.mTexel1[1] = 0;

      tv3.mNormal[0]*=-1;
      tv3.mNormal[1]*=-1;
      tv3.mNormal[2]*=-1;
      tv3.mTexel1[0] = 0;
      tv3.mTexel1[1] = 0;

      mCurrent->add(tv3,tv2,tv1,mVertexPool);
    }
#endif
  }

  virtual void        importIndexedTriangleList(const char *materialName,
                                                unsigned int vertexFlags,
                                                unsigned int vcount,
                                                const MeshVertex *vertices,
                                                unsigned int tcount,
                                                const unsigned int *indices)
  {
    for (unsigned int i=0; i<tcount; i++)
    {
      unsigned int i1 = indices[i*3+0];
      unsigned int i2 = indices[i*3+1];
      unsigned int i3 = indices[i*3+2];
      const MeshVertex &v1 = vertices[i1];
      const MeshVertex &v2 = vertices[i2];
      const MeshVertex &v3 = vertices[i3];
      importTriangle(materialName,vertexFlags,v1,v2,v3);
    }
  }

  void gather(int bone_count)
  {
    mSubMeshes = 0;
    mSubMeshCount = 0;
    if ( !mMySubMeshes.empty() )
    {
      mSubMeshCount = mMySubMeshes.size();
      mSubMeshes    = &mMySubMeshes[0];
      for (unsigned int i=0; i<mSubMeshCount; i++)
      {
        MySubMesh *m = static_cast<MySubMesh *>(mSubMeshes[i]);
        m->gather();
      }
    }
    mVertexCount = mVertexPool.GetSize();
    if ( mVertexCount > 0 )
    {
      mVertices = mVertexPool.GetBuffer();
      if ( bone_count > 0 )
      {
        for (unsigned int i=0; i<mVertexCount; i++)
        {
          MeshVertex &vtx = mVertices[i];
          if ( vtx.mBone[0] >= bone_count ) vtx.mBone[0] = 0;
          if ( vtx.mBone[1] >= bone_count ) vtx.mBone[1] = 0;
          if ( vtx.mBone[2] >= bone_count ) vtx.mBone[2] = 0;
          if ( vtx.mBone[3] >= bone_count ) vtx.mBone[3] = 0;
        }
      }
    }
  }

  VertexPool< MeshVertex > mVertexPool;
  MySubMesh        *mCurrent;
  SubMeshVector   mMySubMeshes;
};

typedef USER_STL::map< StringRef, StringRef > StringRefMap;
typedef USER_STL::vector< MeshMaterial >      MeshMaterialVector;
typedef USER_STL::vector< MeshInstance >      MeshInstanceVector;
typedef USER_STL::vector< MyMesh *>           MyMeshVector;
typedef USER_STL::vector< MeshCollision * >   MeshCollisionVector;

class MyMeshCollisionRepresentation : public MeshCollisionRepresentation
{
public:
  MyMeshCollisionRepresentation(const char *name,const char *info)
  {
    mName = name;
    mInfo = info;
  }
  ~MyMeshCollisionRepresentation(void)
  {
    MeshCollisionVector::iterator i;
    for (i=mGeometries.begin(); i!=mGeometries.end(); i++)
    {
      MeshCollision *mc = (*i);
      if ( mc->getType() == MCT_CONVEX )
      {
        MeshCollisionConvex *mcc = static_cast< MeshCollisionConvex *>(mc);
        MEMALLOC_DELETE_ARRAY(float,mcc->mVertices);
        MEMALLOC_DELETE_ARRAY(unsigned int,mcc->mIndices);
      }
    }
  }

  void gather(void)
  {
    mCollisionCount = mGeometries.size();
    mCollisionGeometry = &mGeometries[0];
  }

  MeshCollisionVector mGeometries;
};

typedef USER_STL::vector< MeshCollisionRepresentation * > MeshCollisionRepresentationVector;

class MyMeshBuilder : public MeshBuilder
{
public:
  MyMeshBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options,MeshImportApplicationResource *appResource)
  {
    gSerializeFrame++;
    mCurrentMesh = 0;
    mCurrentCollision = 0;
    mAppResource = appResource;
    importAssetName(meshName,0);
    mi->importMesh(meshName,data,dlen,this,options,appResource);
    gather();
  }

  MyMeshBuilder(MeshImportApplicationResource *appResource)
  {
    gSerializeFrame++;
    mCurrentMesh = 0;
    mCurrentCollision = 0;
    mAppResource = appResource;
  }


  ~MyMeshBuilder(void)
  {
    MEMALLOC_DELETE_ARRAY(Mesh *,mMeshes);
    MyMeshVector::iterator i;
    for (i=mMyMeshes.begin(); i!=mMyMeshes.end(); ++i)
    {
      MyMesh *src = (*i);
      delete src;
    }

    if ( !mMyAnimations.empty() )
    {
      MeshAnimationVector::iterator i;
      for (i=mMyAnimations.begin(); i!=mMyAnimations.end(); ++i)
      {
        MeshAnimation *a = (*i);
        for (int j=0; j<a->mTrackCount; j++)
        {
          MeshAnimTrack *ma = a->mTracks[j];
          MEMALLOC_DELETE_ARRAY(MeshAnimPose,ma->mPose);
          MEMALLOC_DELETE(MeshAnimTrack,ma);
        }
        MEMALLOC_DELETE_ARRAY(MeshAnimTrack *,a->mTracks);
        MEMALLOC_DELETE(MeshAnimation,a);
      }
    }

    if ( !mMySkeletons.empty() )
    {
      MeshSkeletonVector::iterator i;
      for (i=mMySkeletons.begin(); i!=mMySkeletons.end(); ++i)
      {
        MeshSkeleton *s = (*i);
        MEMALLOC_DELETE_ARRAY(MeshBone,s->mBones);
        MEMALLOC_DELETE(MeshSkeleton,s);
      }
    }
    if ( !mCollisionReps.empty() )
    {
      MeshCollisionRepresentationVector::iterator i;
      for (i=mCollisionReps.begin(); i!=mCollisionReps.end(); ++i)
      {
        MyMeshCollisionRepresentation *mcr = static_cast< MyMeshCollisionRepresentation *>(*i);
        delete mcr;
      }
    }
  }

  void gather(void)
  {

    gatherMaterials();
    // todo..
    mMaterialCount = mMyMaterials.size();

    if ( mMaterialCount )
      mMaterials     = &mMyMaterials[0];
    else
      mMaterials = 0;

    mMeshInstanceCount = mMyMeshInstances.size();

    if ( mMeshInstanceCount )
      mMeshInstances = &mMyMeshInstances[0];
    else
      mMeshInstances = 0;

    mAnimationCount = mMyAnimations.size();
    if ( mAnimationCount )
      mAnimations = &mMyAnimations[0];
    else
      mAnimations = 0;

    int bone_count = 0;
    mSkeletonCount = mMySkeletons.size();
    if ( mSkeletonCount )
    {
      mSkeletons = &mMySkeletons[0];
      bone_count = mSkeletons[0]->mBoneCount;
    }
    else
      mSkeletons = 0;

    mMeshCount = mMyMeshes.size();
    if ( mMeshCount )
    {
      MEMALLOC_DELETE_ARRAY(Mesh *,mMeshes);
      mMeshes    = MEMALLOC_NEW_ARRAY(Mesh *,mMeshCount)[mMeshCount];
      Mesh **dst = mMeshes;
      MyMeshVector::iterator i;
      for (i=mMyMeshes.begin(); i!=mMyMeshes.end(); ++i)
      {
        MyMesh *src = (*i);
        src->gather(bone_count);
        *dst++ = static_cast< Mesh *>(src);
      }
    }
    else
    {
      mMeshes = 0;
    }

    mMeshCollisionCount = mCollisionReps.size();

    if ( mMeshCollisionCount )
    {
      mMeshCollisionRepresentations = &mCollisionReps[0];
      for (unsigned int i=0; i<mMeshCollisionCount; i++)
      {
        MeshCollisionRepresentation *r = mMeshCollisionRepresentations[i];
        MyMeshCollisionRepresentation *mr = static_cast< MyMeshCollisionRepresentation *>(r);
        mr->gather();
      }
    }
    else
    {
      mMeshCollisionRepresentations = 0;
    }

  }

  void gatherMaterials(void)
  {
    mMyMaterials.clear();
    unsigned int mcount = mMaterialMap.size();
    mMyMaterials.reserve(mcount);
    StringRefMap::iterator i;
    for (i=mMaterialMap.begin(); i!=mMaterialMap.end(); ++i)
    {
      MeshMaterial m;
      m.mName = (*i).first.Get();
      m.mMetaData = (*i).second.Get();
      mMyMaterials.push_back(m);
    }
  }

  virtual void        importUserData(const char *userKey,const char *userValue)       // carry along raw user data as ASCII strings only..
  {
    assert(0); // not yet implemented
  }

  virtual void        importUserBinaryData(const char *name,unsigned int len,const unsigned char *data)
  {
    assert(0); // not yet implemented
  }

  virtual void        importTetraMesh(const char *tetraName,const char *meshName,unsigned int tcount,const float *tetraData)
  {
    assert(0); // not yet implemented
  }

  virtual void importMaterial(const char *matName,const char *metaData)
  {
    StringRef m = mStrings.Get(matName);
    StringRef d = mStrings.Get(metaData);
    mMaterialMap[m] = d;
  }

  virtual void        importAssetName(const char *assetName,const char *info)         // name of the overall asset.
  {
    mAssetName = mStrings.Get(assetName).Get();
    mAssetInfo = mStrings.Get(info).Get();
  }

  virtual void        importMesh(const char *meshName,const char *skeletonName)       // name of a mesh and the skeleton it refers to.
  {
    StringRef m1 = mStrings.Get(meshName);
    StringRef s1 = mStrings.Get(skeletonName);

    mCurrentMesh = 0;
    MyMeshVector::iterator found;
    for (found=mMyMeshes.begin(); found != mMyMeshes.end(); found++)
    {
      MyMesh *mm = (*found);
      if ( mm->mName == m1 )
      {
        mCurrentMesh = mm;
        mCurrentMesh->mSkeletonName = s1.Get();
        break;
      }
    }
    if ( mCurrentMesh == 0 )
    {
      MyMesh *m = MEMALLOC_NEW(MyMesh)(m1.Get(),s1.Get());
      mMyMeshes.push_back(m);
      mCurrentMesh = m;
    }
  }

  void getCurrentMesh(const char *meshName)
  {
    if ( meshName == 0 )
    {
      meshName = "default_mesh";
    }
    if ( mCurrentMesh == 0 || !mCurrentMesh->isSame(meshName) )
    {
      importMesh(meshName,0); // make it the new current mesh
    }
  }

  virtual void        importTriangle(const char *_meshName,
                                     const char *_materialName,
                                     unsigned int vertexFlags,
                                     const MeshVertex &v1,
                                     const MeshVertex &v2,
                                     const MeshVertex &v3)
  {
    const char *meshName = mStrings.Get(_meshName).Get();
    const char *materialName = mStrings.Get(_materialName).Get();
    getCurrentMesh(meshName);
    mAABB.include(v1.mPos);
    mAABB.include(v2.mPos);
    mAABB.include(v3.mPos);
    mCurrentMesh->importTriangle(materialName,vertexFlags,v1,v2,v3);
  }

  virtual void        importIndexedTriangleList(const char *_meshName,
                                                const char *_materialName,
                                                unsigned int vertexFlags,
                                                unsigned int vcount,
                                                const MeshVertex *vertices,
                                                unsigned int tcount,
                                                const unsigned int *indices)
  {
    const char *meshName = mStrings.Get(_meshName).Get();
    const char *materialName = mStrings.Get(_materialName).Get();
    getCurrentMesh(meshName);
    for (unsigned int i=0; i<vcount; i++)
    {
      mAABB.include( vertices[i].mPos );
    }
    mCurrentMesh->importIndexedTriangleList(materialName,vertexFlags,vcount,vertices,tcount,indices);
  }

  // do a deep copy...
  virtual void        importAnimation(const MeshAnimation &animation)
  {
    MeshAnimation *a = MEMALLOC_NEW(MeshAnimation);
    a->mName = mStrings.Get(animation.mName).Get();
    a->mTrackCount = animation.mTrackCount;
    a->mFrameCount = animation.mFrameCount;
    a->mDuration = animation.mDuration;
    a->mDtime = animation.mDtime;
    a->mTracks = MEMALLOC_NEW_ARRAY(MeshAnimTrack *,a->mTrackCount)[a->mTrackCount];
    for (int i=0; i<a->mTrackCount; i++)
    {
      const MeshAnimTrack &src =*animation.mTracks[i];
      MeshAnimTrack *t = MEMALLOC_NEW(MeshAnimTrack);
      t->mName = mStrings.Get(src.mName).Get();
      t->mFrameCount = src.mFrameCount;
      t->mDuration = src.mDuration;
      t->mDtime = src.mDtime;
      t->mPose = MEMALLOC_NEW_ARRAY(MeshAnimPose,t->mFrameCount)[t->mFrameCount];
      memcpy(t->mPose,src.mPose,sizeof(MeshAnimPose)*t->mFrameCount);
      a->mTracks[i] = t;
    }
    mMyAnimations.push_back(a);
  }

  virtual void        importSkeleton(const MeshSkeleton &skeleton)
  {
    MeshSkeleton *sk = MEMALLOC_NEW(MeshSkeleton);
    sk->mName = mStrings.Get( skeleton.mName ).Get();
    sk->mBoneCount = skeleton.mBoneCount;
    sk->mBones = 0;
    if ( sk->mBoneCount > 0 )
    {
      sk->mBones = MEMALLOC_NEW_ARRAY(MeshBone,sk->mBoneCount)[sk->mBoneCount];
      MeshBone *dest = sk->mBones;
      const MeshBone *src = skeleton.mBones;
      for (unsigned int i=0; i<(unsigned int)sk->mBoneCount; i++)
      {
        *dest = *src;
        dest->mName = mStrings.Get(src->mName).Get();
        src++;
        dest++;
      }
    }
    mMySkeletons.push_back(sk);
  }

  virtual void        importRawTexture(const char *textureName,const unsigned char *pixels,unsigned int wid,unsigned int hit)
  {
//    assert(0); // not yet implemented
  }

  virtual void        importMeshInstance(const char *meshName,const float pos[3],const float rotation[4],const float scale[3])
  {
    StringRef ref = mStrings.Get(meshName);
    MeshInstance m;
    m.mMeshName = ref.Get();
    m.mPosition[0] = pos[0];
    m.mPosition[1] = pos[1];
    m.mPosition[2] = pos[2];
    m.mRotation[0] = rotation[0];
    m.mRotation[1] = rotation[1];
    m.mRotation[2] = rotation[2];
    m.mRotation[3] = rotation[3];
    m.mScale[0] = scale[0];
    m.mScale[1] = scale[1];
    m.mScale[2] = scale[2];
    mMyMeshInstances.push_back(m);
  }

  virtual void importCollisionRepresentation(const char *name,const char *info) // the name of a new collision representation.
  {

    StringRef ref1 = mStrings.Get(name);
    StringRef ref2 = mStrings.Get(info);

    mCurrentCollision = 0;

    MeshCollisionRepresentationVector::iterator i;
    for (i=mCollisionReps.begin(); i!=mCollisionReps.end(); ++i)
    {
      MyMeshCollisionRepresentation *mcr = static_cast< MyMeshCollisionRepresentation *>(*i);
      if ( strcmp(mcr->mName,name) == 0 )
      {
        mCurrentCollision = mcr;
        break;
      }
    }
    if ( mCurrentCollision )
    {
      mCurrentCollision->mInfo = ref2.Get();
    }
    else
    {
      MyMeshCollisionRepresentation *mcr = MEMALLOC_NEW(MyMeshCollisionRepresentation)(ref1.Get(),ref2.Get());
      MeshCollisionRepresentation *mr = static_cast< MeshCollisionRepresentation *>(mcr);
      mCollisionReps.push_back(mr);
      mCurrentCollision = mcr;
    }
  }

  void getCurrentRep(const char *name)
  {
    if ( mCurrentCollision == 0 || strcmp(mCurrentCollision->mName,name) != 0 )
    {
      mCurrentCollision = 0;
      MeshCollisionRepresentationVector::iterator i;
      for (i=mCollisionReps.begin(); i!=mCollisionReps.end(); ++i)
      {
        MyMeshCollisionRepresentation *mcr = static_cast< MyMeshCollisionRepresentation *>(*i);
        if ( strcmp(mcr->mName,name) == 0 )
        {
          mCurrentCollision = mcr;
          break;
        }
      }
      if ( mCurrentCollision == 0 )
      {
        importCollisionRepresentation(name,0);
      }
    }
  }

  virtual void importConvexHull(const char *collision_rep,    // the collision representation it is associated with
    const char *boneName,         // the name of the bone it is associated with in a skeleton.
    const float *transform,       // the full 4x4 transform for this hull, null if in world space.
    unsigned int vertex_count,
    const float *vertices,
    unsigned int tri_count,
    const unsigned int *indices)
  {
    getCurrentRep(collision_rep);

    MeshCollisionConvex *c = MEMALLOC_NEW(MeshCollisionConvex);
    c->mName = mStrings.Get(boneName).Get();
    if ( transform )
    {
      memcpy(c->mTransform,transform,sizeof(float)*16);
    }
    c->mVertexCount = vertex_count;
    if ( c->mVertexCount )
    {
      c->mVertices = MEMALLOC_NEW_ARRAY(float,vertex_count*3)[vertex_count*3];
      memcpy(c->mVertices,vertices,sizeof(float)*vertex_count*3);
    }
    c->mTriCount = tri_count;
    if ( c->mTriCount )
    {
      c->mIndices = MEMALLOC_NEW_ARRAY(unsigned int,tri_count*3)[tri_count*3];
      memcpy(c->mIndices,indices,sizeof(unsigned int)*tri_count*3);
    }
    MeshCollision *mc = static_cast< MeshCollision *>(c);
    mCurrentCollision->mGeometries.push_back(mc);
  }


  virtual void scale(float s)
  {
    {
      MeshSkeletonVector::iterator i;
      for (i=mMySkeletons.begin(); i!=mMySkeletons.end(); ++i)
      {
        MeshSkeleton *ms = (*i);
        for (int j=0; j<ms->mBoneCount; j++)
        {
          MeshBone &b = ms->mBones[j];
          b.mPosition[0]*=s;
          b.mPosition[1]*=s;
          b.mPosition[2]*=s;
        }
      }
    }

    {
      MyMeshVector::iterator i;
      for (i=mMyMeshes.begin(); i!=mMyMeshes.end(); ++i)
      {
        MyMesh *m = (*i);
        unsigned int vcount = m->mVertexPool.GetSize();
        if ( vcount > 0 )
        {
          MeshVertex *vb = m->mVertexPool.GetBuffer();
          for (unsigned int j=0; j<vcount; j++)
          {
            vb->mPos[0]*=s;
            vb->mPos[1]*=s;
            vb->mPos[2]*=s;
            vb++;
          }
        }
      }
    }

    {
      MeshAnimationVector::iterator i;
      for (i=mMyAnimations.begin(); i!=mMyAnimations.end(); ++i)
      {
        MeshAnimation *ma = (*i);
        for (int j=0; j<ma->mTrackCount; j++)
        {
          MeshAnimTrack *t = ma->mTracks[j];
          for (int k=0; k<t->mFrameCount; k++)
          {
            MeshAnimPose &p = t->mPose[k];
            p.mPos[0]*=s;
            p.mPos[1]*=s;
            p.mPos[2]*=s;
          }
        }
      }
    }
  }

  virtual int getSerializeFrame(void) 
  {
    return gSerializeFrame;
  }


private:
  StringDict                          mStrings;
  StringRefMap                        mMaterialMap;
  MeshMaterialVector                  mMyMaterials;
  MeshInstanceVector                  mMyMeshInstances;
  MyMesh                             *mCurrentMesh;
  MyMeshVector                        mMyMeshes;
  MeshAnimationVector                 mMyAnimations;
  MeshSkeletonVector                  mMySkeletons;
  MyMeshCollisionRepresentation      *mCurrentCollision;
  MeshCollisionRepresentationVector   mCollisionReps;
  MeshImportApplicationResource      *mAppResource;
};

MeshBuilder * createMeshBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options,MeshImportApplicationResource *appResource)
{
  MyMeshBuilder *b = MEMALLOC_NEW(MyMeshBuilder)(meshName,data,dlen,mi,options,appResource);
  return static_cast< MeshBuilder *>(b);
}

MeshBuilder * createMeshBuilder(MeshImportApplicationResource *appResource)
{
  MyMeshBuilder *b = MEMALLOC_NEW(MyMeshBuilder)(appResource);
  return static_cast< MeshBuilder *>(b);
}

void          releaseMeshBuilder(MeshBuilder *m)
{
  MyMeshBuilder *b = static_cast< MyMeshBuilder *>(m);
  MEMALLOC_DELETE(MyMsehImportBuilder,b);
}

}; // end of namesapace
