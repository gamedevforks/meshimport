#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "UserMemAlloc.h"
#include "FloatMath.h"
#include "MeshImport.h"
#include "UserMemAlloc.h"
#include "sutil.h"

#pragma warning(disable:4100 4189 4996)

#define HACK_SKELETON_POSITION 1
#define HACK_SKELETON_ROTATION 1
#define HACK_SKELETON_ROOT 1

#define HACK_ANIMATION_POSITION 1
#define HACK_ANIMATION_ROTATION 1
#define HACK_ANIMATION_ROOT 1

#define HACK_MESH_POSITION 1
#define HACK_MESH_WINDING 1

#define IMPORT_SCALE 1


namespace NVSHARE
{

struct AnimInfo : public Memalloc
{
	char	mName[64];
	char	mGroup[64];    // Animation's group name
	PxI32	mTotalBones;           // TotalBones * NumRawFrames is number of animation keys to digest.
	PxI32	mRootInclude;          // 0 none 1 included 	(unused)
	PxI32	mKeyCompressionStyle;  // Reserved: variants in tradeoffs for compression.
	PxI32	mKeyQuotum;            // Max key quotum for compression
	PxF32	mKeyReduction;       // desired
	PxF32	mTrackTime;          // explicit - can be overridden by the animation rate
	PxF32	mAnimRate;           // frames per second.
	PxI32	mStartBone;            // - Reserved: for partial animations (unused)
	PxI32	mFirstRawFrame;        //
	PxI32	mNumRawFrames;         // NumRawFrames and AnimRate dictate tracktime...
};

struct AnimKey : public Memalloc
{
	PxF32	mPosition[3];
	PxF32	mOrientation[4];
	PxF32	mTime;
};

struct Header : public Memalloc
{
    char  mChunkName[20];
    PxI32 mType;
    PxI32 mLen;
    PxI32 mCount;
};

struct Vector : public Memalloc
{
    PxF32 x;
    PxF32 y;
    PxF32 z;
};

struct Vertex : public Memalloc
{
    PxU16 mIndex;
    PxF32 mTexel[2];
    PxU8  mMaterialIndex;
    PxU8  mUnused;
};

struct Triangle : public Memalloc
{
    PxU16 mWedgeIndex[3];
    PxU8  mMaterialIndex;
    PxU8  mAuxMaterialIndex;
    PxU32 mSmoothingGroups;
};

struct Material : public Memalloc
{
    char mMaterialName[64];
    PxI32 mTextureIndex;
    PxU32 mPolyFlags;
    PxU32 mAuxMaterial;
	PxU32 mAuxFlags;
    PxI32 mLodBias;
    PxI32 mLodStyle;
};

struct Bone : public Memalloc
{
    char  mName[64];
    PxU32 mFlags;
    PxI32 mNumChildren;
    PxI32 mParentIndex;
    PxF32 mOrientation[4];
    PxF32 mPosition[3];
    PxF32 mLength;
    PxF32 mXSize;
    PxF32 mYSize;
    PxF32 mZSize;
};

struct BoneInfluence : public Memalloc
{
    PxF32 mWeight;
    PxI32 mVertexIndex;
    PxI32 mBoneIndex;
};

class DeformVector : public Memalloc
{
public:
    DeformVector(void)
    {
        mCount = 0;
        mWeight[0] = mWeight[1] = mWeight[2] = mWeight[3] = 0;
        mBone[0] = mBone[1] = mBone[2] = mBone[3] = 0;
    }

    PxU32  mCount;
    PxF32  mWeight[4];
    PxU16  mBone[4];

};


// [Dummy Header]
// [Vector Header]
// Array of Fvectors.
// [Vertex]
// Array of Vertices

class MeshImporterPSK : public MeshImporter, public Memalloc
{
public:
  MeshImporterPSK(void)
  {
  }

  virtual ~MeshImporterPSK(void)
  {
  }

  virtual PxI32              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.
  virtual const char *     getExtension(PxI32 index) { return ".psk"; }; // report the default file name extension for this mesh type.
  virtual const char *     getDescription(PxI32 index)
  {
	  return "PSK Skeletal Meshes";
  }

  void getVertex(MeshVertex &dest,const Vector &p,const Vertex &v,const DeformVector &dv,const Vector &normal)
  {

    dest.mPos[0] = p.x*IMPORT_SCALE;
    dest.mPos[1] = p.y*IMPORT_SCALE;
    dest.mPos[2] = p.z*IMPORT_SCALE;

    dest.mNormal[0] = normal.x;
    dest.mNormal[1] = normal.y;
    dest.mNormal[2] = normal.z;

    dest.mTexel1[0] = v.mTexel[0];
    dest.mTexel2[1] = v.mTexel[1];

    dest.mWeight[0] = dv.mWeight[0];
    dest.mWeight[1] = dv.mWeight[1];
    dest.mWeight[2] = dv.mWeight[2];
    dest.mWeight[3] = dv.mWeight[3];

    dest.mBone[0]   = dv.mBone[0];
    dest.mBone[1]   = dv.mBone[1];
    dest.mBone[2]   = dv.mBone[2];
    dest.mBone[3]   = dv.mBone[3];

  }

  void fixName(char *_name)
  {
    char *lastSpace = 0;
	char *name = _name;

    while ( *name )
    {
        if ( *name == 32 )
		{
			if ( !lastSpace )
              lastSpace = name;
		}
		else
			lastSpace = 0;
        name++;
    }
    if ( lastSpace )
    {
        *lastSpace = 0;
    }

	char *scan = _name;
	while ( *scan )
	{
		if ( *scan == 32 ) *scan = '-';
		scan++;
	}
  }

  // warning, this code will not work on big endian processors.
  virtual bool             importMesh(const char *_meshName,const void *_data,PxU32 dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
	  bool ret = false;

	  const char *meshName = _meshName;
	  const char *slash = lastSlash(meshName);
	  if ( slash )
	  {
		  slash++;
		  meshName = slash;
	  }

	  char scratch[2048];
	  strncpy(scratch,_meshName,2048);
	  char *psk = stristr(scratch,".psk");
	  if ( psk )
	  {
		  *psk = 0;
		  strcat(scratch,".psa");
	  }

	  callback->importAssetName(meshName,0);
	  callback->importMesh(meshName,meshName);

      //  have to make a local copy of the data because we are going to modify some of it.
      // Primarily just truncating some whitespace from bone names.
      void *data = MEMALLOC_MALLOC(dlen);
      memcpy(data,_data,dlen);

      if ( appResource )
      {
        PxU32 len;
        void *mem = appResource->getApplicationResource(meshName,scratch,len);
        if ( mem )
        {

		   char *scan = ( char *)mem;
		   Header *baseHeader = ( Header *)scan;
		   scan+=sizeof(Header);

		   Header *bonesHeader = ( Header *)scan;
		   scan+=sizeof(Header);
		   Bone *bones = ( Bone *)scan;
		   assert(sizeof(Bone)==bonesHeader->mLen);
		   scan+=(bonesHeader->mLen*bonesHeader->mCount);

		   Header *animInfo = ( Header *)scan;
		   scan+=sizeof(Header);
		   assert( animInfo->mLen == sizeof(AnimInfo));
		   AnimInfo *ainfo = ( AnimInfo *)scan;
		   scan+=(animInfo->mLen*animInfo->mCount);

		   Header *animKeysHeader = ( Header *)scan;
		   scan+=sizeof(Header);
		   assert( animKeysHeader->mLen == sizeof(AnimKey) );
		   AnimKey *keys = ( AnimKey *)scan;
		   scan+=(animKeysHeader->mLen*animKeysHeader->mCount);

		   Header *scaleKeysHeader = ( Header *)scan;
		   scan+=sizeof(Header);

           MeshAnimation ma;
           ma.mName = ainfo->mName;
           ma.mTrackCount = ainfo->mTotalBones;
           ma.mFrameCount = ainfo->mNumRawFrames;
		   ma.mDtime = 1.0f / (PxF32)(ainfo->mAnimRate);
           ma.mDuration = ma.mDtime*ainfo->mNumRawFrames;
           ma.mTracks = (MeshAnimTrack **)MEMALLOC_MALLOC(sizeof(MeshAnimTrack *)*ma.mTrackCount);

           for (PxI32 i=0; i<ma.mTrackCount; i++)
           {
             Bone &b = bones[i];
             fixName(b.mName);
			 MeshAnimTrack *track = MEMALLOC_NEW(MeshAnimTrack);
             ma.mTracks[i] = track;
             track->mName      = b.mName;
             track->mFrameCount = ma.mFrameCount;
             track->mDuration   = ma.mDuration;
			 track->mDtime      = ma.mDtime;
             track->mPose       = MEMALLOC_NEW(MeshAnimPose)[track->mFrameCount];

             for (PxI32 j=0; j<ma.mFrameCount; j++)
             {

			   PxU32 index = (j*ma.mTrackCount)+i;

               AnimKey &key = keys[index];
               MeshAnimPose &p = track->mPose[j];

               p.mPos[0] = key.mPosition[0]*IMPORT_SCALE;
#if HACK_ANIMATION_POSITION
               p.mPos[1] = -key.mPosition[1]*IMPORT_SCALE;
#else
               p.mPos[1] = key.mPosition[1]*IMPORT_SCALE;
#endif
               p.mPos[2] = key.mPosition[2]*IMPORT_SCALE;

               p.mQuat[0] = key.mOrientation[0];
#if HACK_ANIMATION_ROTATION
               p.mQuat[1] = -key.mOrientation[1];
#else
               p.mQuat[1] = key.mOrientation[1];
#endif
               p.mQuat[2] = key.mOrientation[2];
               p.mQuat[3] = key.mOrientation[3];

#if HACK_ANIMATION_ROOT
			   if ( i == 0)
			   {
				   p.mQuat[3] = -p.mQuat[3];
			   }
#endif
               index++;
            }
          }

          callback->importAnimation(ma);

          for (PxI32 i=0; i<ma.mTrackCount; i++)
          {
            MeshAnimTrack *track = ma.mTracks[i];
            delete []track->mPose;
            delete track;
          }
          delete []ma.mTracks;
          appResource->releaseApplicationResource(mem);
        }
      }



       char *scan = ( char *)data;
       char *end  = scan+dlen;

	   Header *baseHeader = ( Header *)scan;

       scan+=sizeof(Header); // skip first dummy header

       Header *h = 0;

       Header *positionsHeader = h = (Header *)scan;
	   assert( positionsHeader->mLen == sizeof(Vector));
       scan+=sizeof(Header);
       Vector *positions = ( Vector *)scan;
       scan+=h->mLen*h->mCount;
#if HACK_MESH_POSITION
	   for (PxI32 i=0; i<positionsHeader->mCount; i++)
	   {
		   Vector &v = positions[i];
		   v.y*=-1; // flip the Y-coordinate
	   }
#endif
       Header *verticesHeader = h = ( Header *)scan;
	   assert( verticesHeader->mLen == sizeof(Vertex));
       scan+=sizeof(Header);
       Vertex *vertices = ( Vertex *)scan;
       scan+=h->mLen*h->mCount;

       Header *trianglesHeader = h = ( Header *)scan;
 	   assert( trianglesHeader->mLen == sizeof(Triangle));
       scan+=sizeof(Header);
       Triangle *triangles = ( Triangle *)scan;
       scan+=h->mLen*h->mCount;

       Header *materialsHeader = h = ( Header *)scan;
	   assert( materialsHeader->mLen == sizeof(Material));
       scan+=sizeof(Header);
       Material *materials= ( Material *)scan;
       scan+=h->mLen*h->mCount;

       Header *bonesHeader = h = ( Header *)scan;
	   assert( bonesHeader->mLen == sizeof(Bone));
       scan+=sizeof(Header);
       Bone *bones= ( Bone *)scan;
       scan+=h->mLen*h->mCount;

       Header *boneInfluencesHeader =  h = ( Header *)scan;
	   assert( boneInfluencesHeader->mLen == sizeof(BoneInfluence));
       scan+=sizeof(Header);
       BoneInfluence *boneInfluences = ( BoneInfluence *)scan;
       scan+=h->mLen*h->mCount;

      if ( bonesHeader->mCount > 0 )
      {
        MeshSkeleton *ms = MEMALLOC_NEW(MeshSkeleton);
        ms->mName = meshName;
        ms->mBoneCount = bonesHeader->mCount;
        ms->mBones = MEMALLOC_NEW(MeshBone)[ms->mBoneCount];
        for (PxI32 i=0; i<ms->mBoneCount; i++)
        {
            MeshBone &dest = ms->mBones[i];
            Bone &src = bones[i];
            fixName(src.mName);
            dest.mName = src.mName;
            dest.mParentIndex = (i==0) ? -1 : src.mParentIndex;

            dest.mPosition[0] = src.mPosition[0]*IMPORT_SCALE;
#if HACK_SKELETON_POSITION
            dest.mPosition[1] = -src.mPosition[1]*IMPORT_SCALE;
#else
            dest.mPosition[1] = src.mPosition[1]*IMPORT_SCALE;
#endif
            dest.mPosition[2] = src.mPosition[2]*IMPORT_SCALE;

            dest.mOrientation[0] = src.mOrientation[0];
#if HACK_SKELETON_ROTATION
            dest.mOrientation[1] = -src.mOrientation[1];
#else
            dest.mOrientation[1] = src.mOrientation[1];
#endif
            dest.mOrientation[2] = src.mOrientation[2];
            dest.mOrientation[3] = src.mOrientation[3];

#if HACK_SKELETON_ROOT
			if ( i == 0 )
			{
				dest.mOrientation[3]*=-1;
			}
#endif
            dest.mScale[0] = 1; //src.mXSize;
            dest.mScale[1] = 1; //src.mYSize;
            dest.mScale[2] = 1; //src.mZSize;

        }
        callback->importSkeleton(*ms);
        delete []ms->mBones;
        delete ms;
      }

      assert( scan == end );

      DeformVector *dvertices = MEMALLOC_NEW(DeformVector)[positionsHeader->mCount];

      for (PxI32 i=0; i<boneInfluencesHeader->mCount; i++)
      {
         BoneInfluence &b = boneInfluences[i];
        DeformVector &d = dvertices[b.mVertexIndex];
        if ( d.mCount < 4 )
        {
            d.mWeight[d.mCount] = b.mWeight;
            d.mBone[d.mCount] = (PxU16)b.mBoneIndex;
            d.mCount++;
        }
      }

      Vector *normals = (Vector *)MEMALLOC_MALLOC( sizeof(Vector)*positionsHeader->mCount);
      memset(normals,0,sizeof(Vector)*positionsHeader->mCount);
      for (PxI32 i=0; i<trianglesHeader->mCount; i++)
      {
         Triangle &t = triangles[i];
        MeshVertex mv1,mv2,mv3;

         Vertex &v1 = vertices[t.mWedgeIndex[0]];
         Vertex &v2 = vertices[t.mWedgeIndex[1]];
         Vertex &v3 = vertices[t.mWedgeIndex[2]];

         Vector &p1 = positions[v1.mIndex];
         Vector &p2 = positions[v2.mIndex];
         Vector &p3 = positions[v3.mIndex];

        Vector &n1 = normals[v1.mIndex];
        Vector &n2 = normals[v2.mIndex];
        Vector &n3 = normals[v3.mIndex];

        Vector normal;
        fm_computePlane( &p1.x, &p3.x, &p2.x, &normal.x );

        n1.x+=normal.x;
        n1.y+=normal.y;
        n1.z+=normal.z;

        n2.x+=normal.x;
        n2.y+=normal.y;
        n2.z+=normal.z;

        n3.x+=normal.x;
        n3.y+=normal.y;
        n3.z+=normal.z;
      }

      for (PxI32 i=0; i<positionsHeader->mCount; i++)
      {
        Vector &n = normals[i];
        fm_normalize(&n.x);
      }



      for (PxI32 i=0; i<trianglesHeader->mCount; i++)
      {
         Triangle &t = triangles[i];
        MeshVertex mv1,mv2,mv3;

         Vertex &v1 = vertices[t.mWedgeIndex[0]];
         Vertex &v2 = vertices[t.mWedgeIndex[1]];
         Vertex &v3 = vertices[t.mWedgeIndex[2]];

         Vector &p1 = positions[v1.mIndex];
         Vector &p2 = positions[v2.mIndex];
         Vector &p3 = positions[v3.mIndex];

         Vector &n1 = normals[v1.mIndex];
         Vector &n2 = normals[v2.mIndex];
         Vector &n3 = normals[v3.mIndex];


         DeformVector &dv1 = dvertices[v1.mIndex];
         DeformVector &dv2 = dvertices[v2.mIndex];
         DeformVector &dv3 = dvertices[v3.mIndex];

        getVertex(mv1,p1,v1,dv1,n1);
        getVertex(mv2,p2,v2,dv2,n2);
        getVertex(mv3,p3,v3,dv3,n3);

        const char *material = "default";
        if ( t.mMaterialIndex >= 0 && t.mMaterialIndex < materialsHeader->mCount )
        {
            material = materials[ t.mMaterialIndex ].mMaterialName;
        }
#if HACK_MESH_WINDING
        callback->importTriangle(meshName,material, MIVF_ALL, mv1, mv3, mv2 );
#else
		callback->importTriangle(meshName,material, MIVF_ALL, mv1, mv2, mv3 );
#endif

      }


      delete []dvertices;
      MEMALLOC_FREE(data);

	  return ret;
  }


};

MeshImporter * createMeshImportPSK(void)
{
    MeshImporterPSK *m = MEMALLOC_NEW(MeshImporterPSK);
    return static_cast< MeshImporter *>(m);
}

void           releaseMeshImportPSK(MeshImporter *iface)
{
    MeshImporterPSK *p = static_cast< MeshImporterPSK *>(iface);
    delete p;
}


};  // end of namespace
