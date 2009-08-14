#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImportBuilder.h"
#include "VtxWeld.h"
#include "UserMemAlloc.h"
#include "stringdict.h"
#include "sutil.h"
#include "FloatMath.h"
#include "KeyValueIni.h"

#pragma warning(disable:4100 4189)

namespace MESHIMPORT
{

typedef USER_STL::vector< MeshVertex >   MeshVertexVector;
typedef USER_STL::vector< NxU32 > MeshIndexVector;
typedef USER_STL::vector< SubMesh * >    SubMeshVector;
typedef USER_STL::vector< Mesh * >       MeshVector;
typedef USER_STL::vector< MeshAnimation * > MeshAnimationVector;
typedef USER_STL::vector< MeshSkeleton * > MeshSkeletonVector;

static NxI32 gSerializeFrame=1;

class MyMesh;

void validate(const MeshVertex &v)
{
	assert( _finite( v.mPos[0] ));
	assert( _finite( v.mPos[1] ));
	assert( _finite( v.mPos[2] ));
	assert( _finite( v.mNormal[0] ) );
	assert( _finite( v.mNormal[1] ) );
	assert( _finite( v.mNormal[2] ) );

	assert( _finite( v.mTexel1[0] ) );
	assert( _finite( v.mTexel1[1] ) );

	assert( _finite( v.mTexel2[0] ) );
	assert( _finite( v.mTexel2[1] ) );

	assert( _finite( v.mTexel3[0] ) );
	assert( _finite( v.mTexel3[1] ) );

	assert( _finite( v.mTexel4[0] ) );
	assert( _finite( v.mTexel4[1] ) );

	assert( _finite( v.mWeight[0] ) );
	assert( _finite( v.mWeight[1] ) );
	assert( _finite( v.mWeight[2] ) );
	assert( _finite( v.mWeight[3] ) );

	assert( v.mWeight[0] >= 0 && v.mWeight[0] <= 1 );
	assert( v.mWeight[1] >= 0 && v.mWeight[1] <= 1 );
	assert( v.mWeight[2] >= 0 && v.mWeight[2] <= 1 );
	assert( v.mWeight[3] >= 0 && v.mWeight[3] <= 1 );

	NxF32 sum = v.mWeight[0] + v.mWeight[1] + v.mWeight[2] + v.mWeight[3];
	assert( sum >= 0 && sum <= 1.001f );
	assert( v.mBone[0] >= 0 && v.mBone[0] < 1024 );
	assert( v.mBone[1] >= 0 && v.mBone[1] < 1024 );
	assert( v.mBone[2] >= 0 && v.mBone[2] < 1024 );
	assert( v.mBone[3] >= 0 && v.mBone[3] < 1024 );

	assert( _finite(v.mTangent[0]));
	assert( _finite(v.mTangent[1]));
	assert( _finite(v.mTangent[2]));

	assert( _finite(v.mBiNormal[0]));
	assert( _finite(v.mBiNormal[1]));
	assert( _finite(v.mBiNormal[2]));

}

class MySubMesh : public SubMesh
{
public:
  MySubMesh(const char *mat,NxU32 vertexFlags)
  {
    mMaterialName = mat;
    mVertexFlags  = (MeshVertexFlag)vertexFlags;
  }

  ~MySubMesh(void)
  {
  }

  bool isSame(const char *mat,NxU32 vertexFlags) const
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

    NxU32 i1 = vpool.GetVertex(v1);
    NxU32 i2 = vpool.GetVertex(v2);
    NxU32 i3 = vpool.GetVertex(v3);

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

  void getCurrent(const char *materialName,NxU32 vertexFlags)
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
                                     NxU32 vertexFlags,
                                     const MeshVertex &_v1,
                                     const MeshVertex &_v2,
                                     const MeshVertex &_v3)
  {
	  MeshVertex v1 = _v1;
	  MeshVertex v2 = _v2;
	  MeshVertex v3 = _v3;
	  fm_normalize(v1.mNormal);
	  fm_normalize(v1.mBiNormal);
	  fm_normalize(v1.mTangent);

	  fm_normalize(v2.mNormal);
	  fm_normalize(v2.mBiNormal);
	  fm_normalize(v2.mTangent);

	  fm_normalize(v3.mNormal);
	  fm_normalize(v3.mBiNormal);
	  fm_normalize(v3.mTangent);


    mAABB.include( v1.mPos );
    mAABB.include( v2.mPos );
    mAABB.include( v3.mPos );
	validate(v1);
	validate(v2);
	validate(v3);
    getCurrent(materialName,vertexFlags);
    mVertexFlags|=vertexFlags;

    mCurrent->add(v1,v2,v3,mVertexPool);
  }

  virtual void        importIndexedTriangleList(const char *materialName,
                                                NxU32 vertexFlags,
                                                NxU32 vcount,
                                                const MeshVertex *vertices,
                                                NxU32 tcount,
                                                const NxU32 *indices)
  {
    for (NxU32 i=0; i<tcount; i++)
    {
      NxU32 i1 = indices[i*3+0];
      NxU32 i2 = indices[i*3+1];
      NxU32 i3 = indices[i*3+2];
      const MeshVertex &v1 = vertices[i1];
      const MeshVertex &v2 = vertices[i2];
      const MeshVertex &v3 = vertices[i3];
      importTriangle(materialName,vertexFlags,v1,v2,v3);
    }
  }

  void gather(NxI32 bone_count)
  {
    mSubMeshes = 0;
    mSubMeshCount = 0;
    if ( !mMySubMeshes.empty() )
    {
      mSubMeshCount = mMySubMeshes.size();
      mSubMeshes    = &mMySubMeshes[0];
      for (NxU32 i=0; i<mSubMeshCount; i++)
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
        for (NxU32 i=0; i<mVertexCount; i++)
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
        MEMALLOC_DELETE_ARRAY(NxF32,mcc->mVertices);
        MEMALLOC_DELETE_ARRAY(NxU32,mcc->mIndices);
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
  MyMeshBuilder(KeyValueIni *ini,const char *meshName,const void *data,NxU32 dlen,MeshImporter *mi,const char *options,MeshImportApplicationResource *appResource)
  {
    mINI = ini;
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
    mINI = 0;
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
        for (NxI32 j=0; j<a->mTrackCount; j++)
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

    NxI32 bone_count = 0;
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
      for (NxU32 i=0; i<mMeshCollisionCount; i++)
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
    NxU32 mcount = mMaterialMap.size();
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

  virtual void        importUserBinaryData(const char *name,NxU32 len,const NxU8 *data)
  {
    assert(0); // not yet implemented
  }

  virtual void        importTetraMesh(const char *tetraName,const char *meshName,NxU32 tcount,const NxF32 *tetraData)
  {
    assert(0); // not yet implemented
  }

  virtual void importMaterial(const char *matName,const char *metaData)
  {
    matName = getMaterialName(matName);
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

  virtual void importPlane(const NxF32 *p)
  {
	  mPlane[0] = p[0];
	  mPlane[1] = p[1];
	  mPlane[2] = p[2];
	  mPlane[3] = p[3];
  }



  virtual void        importTriangle(const char *_meshName,
                                     const char *_materialName,
                                     NxU32 vertexFlags,
                                     const MeshVertex &v1,
                                     const MeshVertex &v2,
                                     const MeshVertex &v3)
  {
    _materialName = getMaterialName(_materialName);
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
                                                NxU32 vertexFlags,
                                                NxU32 vcount,
                                                const MeshVertex *vertices,
                                                NxU32 tcount,
                                                const NxU32 *indices)
  {
    _materialName = getMaterialName(_materialName);
    const char *meshName = mStrings.Get(_meshName).Get();
    const char *materialName = mStrings.Get(_materialName).Get();
    getCurrentMesh(meshName);
    for (NxU32 i=0; i<vcount; i++)
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
    for (NxI32 i=0; i<a->mTrackCount; i++)
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
      for (NxU32 i=0; i<(NxU32)sk->mBoneCount; i++)
      {
        *dest = *src;
        dest->mName = mStrings.Get(src->mName).Get();
        src++;
        dest++;
      }
    }
    mMySkeletons.push_back(sk);
  }

  virtual void        importRawTexture(const char *textureName,const NxU8 *pixels,NxU32 wid,NxU32 hit)
  {
//    assert(0); // not yet implemented
  }

  virtual void        importMeshInstance(const char *meshName,const NxF32 pos[3],const NxF32 rotation[4],const NxF32 scale[3])
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


  virtual void importSphere(const char *collision_rep,    // the collision representation it is associated with
                            const char *boneName,         // the name of the bone it is associated with in a skeleton.
                            const NxF32 *transform,
                            NxF32 radius)
  {
    getCurrentRep(collision_rep);
    MeshCollisionSphere *c = MEMALLOC_NEW(MeshCollisionSphere);
    c->mName = mStrings.Get(boneName).Get();
    if ( transform )
    {
      memcpy(c->mTransform,transform,sizeof(NxF32)*16);
    }
    c->mRadius = radius;
    MeshCollision *mc = static_cast< MeshCollision *>(c);
    mCurrentCollision->mGeometries.push_back(mc);
  }

  virtual void importCapsule(const char *collision_rep,    // the collision representation it is associated with
                                const char *boneName,         // the name of the bone it is associated with in a skeleton.
                                const NxF32 *transform,       // the full 4x4 transform for this hull, null if in world space.
                                NxF32 radius,
                                NxF32 height)
  {
    getCurrentRep(collision_rep);
    MeshCollisionCapsule *c = MEMALLOC_NEW(MeshCollisionCapsule);
    c->mName = mStrings.Get(boneName).Get();
    if ( transform )
    {
      memcpy(c->mTransform,transform,sizeof(NxF32)*16);
    }
    c->mRadius = radius;
    c->mHeight = height;
    MeshCollision *mc = static_cast< MeshCollision *>(c);
    mCurrentCollision->mGeometries.push_back(mc);
  }

  virtual void importOBB(const char *collision_rep,    // the collision representation it is associated with
                         const char *boneName,         // the name of the bone it is associated with in a skeleton.
                         const NxF32 *transform,       // the full 4x4 transform for this hull, null if in world space.
                         const NxF32 *sides)
  {
    getCurrentRep(collision_rep);
    MeshCollisionBox *c = MEMALLOC_NEW(MeshCollisionBox);
    c->mName = mStrings.Get(boneName).Get();
    if ( transform )
    {
      memcpy(c->mTransform,transform,sizeof(NxF32)*16);
    }
    c->mSides[0] = sides[0];
    c->mSides[1] = sides[1];
    c->mSides[2] = sides[2];
    MeshCollision *mc = static_cast< MeshCollision *>(c);
    mCurrentCollision->mGeometries.push_back(mc);
  }



  virtual void importConvexHull(const char *collision_rep,    // the collision representation it is associated with
    const char *boneName,         // the name of the bone it is associated with in a skeleton.
    const NxF32 *transform,       // the full 4x4 transform for this hull, null if in world space.
    NxU32 vertex_count,
    const NxF32 *vertices,
    NxU32 tri_count,
    const NxU32 *indices)
  {
    getCurrentRep(collision_rep);
    MeshCollisionConvex *c = MEMALLOC_NEW(MeshCollisionConvex);
    c->mName = mStrings.Get(boneName).Get();
    if ( transform )
    {
      memcpy(c->mTransform,transform,sizeof(NxF32)*16);
    }
    c->mVertexCount = vertex_count;
    if ( c->mVertexCount )
    {
      c->mVertices = MEMALLOC_NEW_ARRAY(NxF32,vertex_count*3)[vertex_count*3];
      memcpy(c->mVertices,vertices,sizeof(NxF32)*vertex_count*3);
    }
    c->mTriCount = tri_count;
    if ( c->mTriCount )
    {
      c->mIndices = MEMALLOC_NEW_ARRAY(NxU32,tri_count*3)[tri_count*3];
      memcpy(c->mIndices,indices,sizeof(NxU32)*tri_count*3);
    }
    MeshCollision *mc = static_cast< MeshCollision *>(c);
    mCurrentCollision->mGeometries.push_back(mc);
  }

  virtual void rotate(NxF32 rotX,NxF32 rotY,NxF32 rotZ)
  {
    NxF32 quat[4];
    fm_eulerToQuat(rotX*FM_DEG_TO_RAD,rotY*FM_DEG_TO_RAD,rotZ*FM_DEG_TO_RAD,quat);

    {
      MeshSkeletonVector::iterator i;
      for (i=mMySkeletons.begin(); i!=mMySkeletons.end(); ++i)
      {
        MeshSkeleton *ms = (*i);
        for (NxI32 j=0; j<ms->mBoneCount; j++)
        {
          MeshBone &b = ms->mBones[j];
          if ( b.mParentIndex == -1 )
          {
            fm_quatRotate(quat,b.mPosition,b.mPosition);
            fm_multiplyQuat(quat,b.mOrientation,b.mOrientation);
          }
        }
      }
    }

    {
      MyMeshVector::iterator i;
      for (i=mMyMeshes.begin(); i!=mMyMeshes.end(); ++i)
      {
        MyMesh *m = (*i);
        NxU32 vcount = m->mVertexPool.GetSize();
        if ( vcount > 0 )
        {
          MeshVertex *vb = m->mVertexPool.GetBuffer();
          for (NxU32 j=0; j<vcount; j++)
          {
            fm_quatRotate(quat,vb->mPos,vb->mPos);
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
        for (NxI32 j=0; j<ma->mTrackCount && j <1; j++)
        {
          MeshAnimTrack *t = ma->mTracks[j];
          for (NxI32 k=0; k<t->mFrameCount; k++)
          {
            MeshAnimPose &p = t->mPose[k];
            fm_quatRotate(quat,p.mPos,p.mPos);
            fm_multiplyQuat(quat,p.mQuat,p.mQuat);
          }
        }
      }
    }
  }

  virtual void scale(NxF32 s)
  {
    {
      MeshSkeletonVector::iterator i;
      for (i=mMySkeletons.begin(); i!=mMySkeletons.end(); ++i)
      {
        MeshSkeleton *ms = (*i);
        for (NxI32 j=0; j<ms->mBoneCount; j++)
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
        NxU32 vcount = m->mVertexPool.GetSize();
        if ( vcount > 0 )
        {
          MeshVertex *vb = m->mVertexPool.GetBuffer();
          for (NxU32 j=0; j<vcount; j++)
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
        for (NxI32 j=0; j<ma->mTrackCount; j++)
        {
          MeshAnimTrack *t = ma->mTracks[j];
          for (NxI32 k=0; k<t->mFrameCount; k++)
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

  virtual NxI32 getSerializeFrame(void)
  {
    return gSerializeFrame;
  }

  const char *getMaterialName(const char *matName)
  {
    const char *ret = matName;
    if ( mINI )
    {
        NxU32 keycount;
        NxU32 lineno;
        const KeyValueSection *section = locateSection(mINI,"REMAP_MATERIALS",keycount,lineno);
        if ( section )
        {
            for (NxU32 i=0; i<keycount; i++)
            {
                const char *key = getKey(section,i,lineno);
                const char *value = getValue(section,i,lineno);
                if ( key && value )
                {
                    if ( stricmp(key,matName) == 0 )
                    {
                        ret = value;
                        break;
                    }
                }
            }
        }
    }
    return ret;
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
  KeyValueIni                        *mINI;
};

MeshBuilder * createMeshBuilder(KeyValueIni *ini,const char *meshName,const void *data,NxU32 dlen,MeshImporter *mi,const char *options,MeshImportApplicationResource *appResource)
{
  MyMeshBuilder *b = MEMALLOC_NEW(MyMeshBuilder)(ini,meshName,data,dlen,mi,options,appResource);
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
