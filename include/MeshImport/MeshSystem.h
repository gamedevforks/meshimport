#ifndef MESH_SYSTEM_H

#define MESH_SYSTEM_H

//** Defines a set of data structures for transporting mesh data between application modules.

#pragma warning(push)
#pragma warning(disable:4996)

#define MAXSTRLEN 256

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <malloc.h>
#include <math.h>

#include "../common/snippets/UserMemAlloc.h"

namespace MESHIMPORT
{

enum MeshImportVertexFlag
{
  MIVF_POSITION       = (1<<0),
  MIVF_NORMAL         = (1<<1),
  MIVF_COLOR          = (1<<2),
  MIVF_TEXEL1         = (1<<3),
  MIVF_TEXEL2         = (1<<4),
  MIVF_TEXEL3         = (1<<5),
  MIVF_TEXEL4         = (1<<6),
  MIVF_TANGENT        = (1<<7),
  MIVF_BINORMAL       = (1<<8),
  MIVF_BONE_WEIGHTING = (1<<9),
	MIVF_ALL = (MIVF_POSITION | MIVF_NORMAL | MIVF_COLOR | MIVF_TEXEL1 | MIVF_TEXEL2 | MIVF_TEXEL3 | MIVF_TEXEL4 | MIVF_TANGENT | MIVF_BINORMAL | MIVF_BONE_WEIGHTING)
};

class MeshImportVertex
{
public:
  MeshImportVertex(void)
  {
    mPos[0] = mPos[1] = mPos[2] = 0;
    mNormal[0] = 0; mNormal[1] = 1; mNormal[2] = 0;
    mColor = 0xFFFFFFFF;
    mTexel1[0] = mTexel1[1] = 0;
    mTexel2[0] = mTexel2[1] = 0;
    mTexel3[0] = mTexel3[1] = 0;
    mTexel4[0] = mTexel4[1] = 0;
    mTangent[0] = mTangent[1] = mTangent[2] = 0;
    mBiNormal[0] = mBiNormal[1] = mBiNormal[2] = 0;
    mWeight[0] = 1; mWeight[1] = 0; mWeight[2] = 0; mWeight[3] = 0;
    mBone[0] = mBone[1] = mBone[2] = mBone[3] = 0;
  }
  float          mPos[3];
  float          mNormal[3];
  unsigned int   mColor;
  float          mTexel1[2];
  float          mTexel2[2];
  float          mTexel3[2];
  float          mTexel4[2];
  float          mTangent[3];
  float          mBiNormal[3];
  float          mWeight[4];
  unsigned short mBone[4];
};

class MeshImportBone
{
public:
	MeshImportBone(void)
	{
		mParentIndex = -1;
		strcpy(mName,"");
		Identity();
	}

	void Identity(void)
	{
		mPosition[0] = 0;
		mPosition[1] = 0;
		mPosition[2] = 0;

		mOrientation[0] = 0;
		mOrientation[1] = 0;
		mOrientation[2] = 0;
		mOrientation[3] = 1;
		IdentityTransform();
	}

	void IdentityTransform(void)
	{
		mElement[0][0] = 1;
		mElement[1][1] = 1;
		mElement[2][2] = 1;
		mElement[3][3] = 1;

		mElement[1][0] = 0;
		mElement[2][0] = 0;
		mElement[3][0] = 0;

		mElement[0][1] = 0;
		mElement[2][1] = 0;
		mElement[3][1] = 0;

		mElement[0][2] = 0;
		mElement[1][2] = 0;
		mElement[3][2] = 0;

		mElement[0][3] = 0;
		mElement[1][3] = 0;
		mElement[2][3] = 0;
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
	}

	void Set(const char *name,int parent,const float *transform)
	{
		strncpy(mName,name,MAXSTRLEN);
		mParentIndex = parent;
		memcpy(mElement,transform,sizeof(float)*16);

		ExtractOrientation(mOrientation);

		mPosition[0] = mElement[3][0];
		mPosition[1] = mElement[3][1];
		mPosition[2] = mElement[3][2];

	}

	void Set(const char *name,int parent,const float *pos,const float *rot)
	{
		strncpy(mName,name,MAXSTRLEN);

		mParentIndex = parent;

		mPosition[0] = pos[0];
		mPosition[1] = pos[1];
		mPosition[2] = pos[2];

		mOrientation[0] = rot[0];
		mOrientation[1] = rot[1];
		mOrientation[2] = rot[2];
		mOrientation[3] = rot[3];

		ComposeTransform();

	}

	void GetPos(float *pos) const
	{
		pos[0] = mElement[3][0];
		pos[1] = mElement[3][1];
		pos[2] = mElement[3][2];
	}

	const char * GetName(void) const { return mName; };

	const float * GetTransform(void) const { return &mElement[0][0]; };

	void ExtractOrientation(float *rot)
	{
		float tr = mElement[0][0] + mElement[1][1] + mElement[2][2];
		if (tr > 0.0f )
		{
			float s = (float) sqrtf( tr + 1.0f);
			rot[3] = s * 0.5f;
			s = 0.5f / s;
			rot[0] = (mElement[1][2] - mElement[2][1]) * s;
			rot[1] = (mElement[2][0] - mElement[0][2]) * s;
			rot[2] = (mElement[0][1] - mElement[1][0]) * s;
		}
		else
		{
			// diagonal is negative
			int nxt[3] = {1, 2, 0};
			float  qa[4];

			int i = 0;

			if (mElement[1][1] > mElement[0][0]) i = 1;
			if (mElement[2][2] > mElement[i][i]) i = 2;

			int j = nxt[i];
			int k = nxt[j];

			float s = sqrtf ( ((mElement[i][i] - (mElement[j][j] + mElement[k][k])) + 1.0f) );

			qa[i] = s * 0.5f;

			if (s != 0.0f ) s = 0.5f / s;

			qa[3] = (mElement[j][k] - mElement[k][j]) * s;
			qa[j] = (mElement[i][j] + mElement[j][i]) * s;
			qa[k] = (mElement[i][k] + mElement[k][i]) * s;

			rot[0] = qa[0];
			rot[1] = qa[1];
			rot[2] = qa[2];
			rot[3] = qa[3];
		}
	}

	void ComposeTransform(void)
	{
		IdentityTransform();
		mElement[3][0] = mPosition[0];
		mElement[3][1] = mPosition[1];
		mElement[3][2] = mPosition[2];
		ComposeOrientation();
	}

	void ComposeOrientation(void)
	{
		float w = mOrientation[3];

		float xx = mOrientation[0]*mOrientation[0];
		float yy = mOrientation[1]*mOrientation[1];
		float zz = mOrientation[2]*mOrientation[2];
		float xy = mOrientation[0]*mOrientation[1];
		float xz = mOrientation[0]*mOrientation[2];
		float yz = mOrientation[1]*mOrientation[2];
		float wx = w*mOrientation[0];
		float wy = w*mOrientation[1];
		float wz = w*mOrientation[2];

		mElement[0][0] = 1 - 2 * ( yy + zz );
		mElement[1][0] =     2 * ( xy - wz );
		mElement[2][0] =     2 * ( xz + wy );

		mElement[0][1] =     2 * ( xy + wz );
		mElement[1][1] = 1 - 2 * ( xx + zz );
		mElement[2][1] =     2 * ( yz - wx );

		mElement[0][2] =     2 * ( xz - wy );
		mElement[1][2] =     2 * ( yz + wx );
		mElement[2][2] = 1 - 2 * ( xx + yy );

	}

	int GetParentIndex(void) const { return mParentIndex; };
	const float * GetPosition(void) const { return mPosition; };
	const float * GetOrientation(void) const { return mOrientation; };

  void SetTransform(const float *transform)
  {
    memcpy(mElement,transform,sizeof(mElement));
  }

	char          mName[MAXSTRLEN];
	int           mParentIndex;          // array index of parent bone
	float         mPosition[3];
	float         mOrientation[4];
	float         mElement[4][4];
};

class MeshImportEntry
{
public:
private:
	char      mName[MAXSTRLEN];
	int     mBone;         // bone this mesh is associcated
};

class MeshImportSkeleton
{
public:
	MeshImportSkeleton(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
		mBoneCount = 0;
		mBones = 0;
	}

	MeshImportSkeleton(const char *name,int bonecount)
	{
		strncpy(mName,name,MAXSTRLEN);
		mBoneCount = bonecount;
    mBones = MEMALLOC_NEW_ARRAY(MeshImportBone,bonecount)[bonecount];
	}

	MeshImportSkeleton(const MeshImportSkeleton &sk)
	{
		strncpy(mName, sk.mName, MAXSTRLEN );
		mBoneCount = sk.mBoneCount;
		mBones = 0;
		if ( mBoneCount )
		{
			mBones = MEMALLOC_NEW_ARRAY(MeshImportBone,mBoneCount)[mBoneCount];
			memcpy(mBones,sk.mBones,sizeof(MeshImportBone)*mBoneCount);
		}
	}

	~MeshImportSkeleton(void)
	{
		MEMALLOC_DELETE_ARRAY(MeshImportBone,mBones);
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
	}

	void SetBones(int bcount,MeshImportBone *bones) // memory ownership changes hands here!!!!!!!!!!
	{
		mBoneCount = bcount;
		mBones     = bones;
	}

	void ComputeDefaultWeighting(const float *pos,float *weight,unsigned short &b1,unsigned short &b2,unsigned short &b3,unsigned short &b4) const
  {
  	float closest[4];
  	int   bones[4];
  	float furthest;

  	FindFourClosest(pos,closest,bones,furthest);

  	float recip = 1.0f / furthest;

  	weight[0] = (furthest-closest[0]) * recip;
  	weight[1] = (furthest-closest[1]) * recip;
  	weight[2] = (furthest-closest[2]) * recip;
  	weight[3] = (furthest-closest[3]) * recip;

  	float total = weight[0] + weight[1] + weight[2] + weight[3];

  	recip = 1.0f / total;

  	weight[0] = weight[0]*recip;
  	weight[1] = weight[1]*recip;
  	weight[2] = weight[2]*recip;
  	weight[3] = weight[3]*recip;

  	total = weight[0] + weight[1] + weight[2] + weight[3];

  	b1 = (unsigned short)bones[0];
  	b2 = (unsigned short)bones[1];
  	b3 = (unsigned short)bones[2];
  	b4 = (unsigned short)bones[3];

  }

	void FindFourClosest(const float *pos,float *closest,int *bones,float &fifthbone) const
  {
  	closest[0] = FLT_MAX;
  	closest[1] = FLT_MAX;
  	closest[2] = FLT_MAX;
  	closest[3] = FLT_MAX;

  	fifthbone  = FLT_MAX;

  	bones[0]   = 0;
  	bones[1]   = 0;
  	bones[2]   = 0;
  	bones[3]   = 0;

  	MeshImportBone *b = mBones;
  	for (int i=0; i<mBoneCount; i++,b++)
  	{
  		float bpos[3];

  		b->GetPos(bpos);

  		float dx = bpos[0] - pos[0];
  		float dy = bpos[1] - pos[1];
  		float dz = bpos[2] - pos[2];

  		float distance = dx*dx+dy*dy+dz*dz;

  		if ( distance < closest[0] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = closest[2];
  			bones[3]   = bones[2];

  			closest[2] = closest[1];
  			bones[2]   = bones[1];

  			closest[1] = closest[0];
  			bones[1]   = bones[0];

  			closest[0] = distance;
  			bones[0]   = i;

  		}
  		else if ( distance < closest[1] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = closest[2];
  			bones[3]   = bones[2];

  			closest[2] = closest[1];
  			bones[2]   = bones[1];

  			closest[1] = distance;
  			bones[1]   = i;
  		}
  		else if ( distance < closest[2] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = closest[2];
  			bones[3]   = bones[2];

  			closest[2]  = distance;
  			bones[2]    = i;
  		}
  		else if ( distance < closest[3] )
  		{
  			fifthbone  = closest[3];
  			closest[3] = distance;
  			bones[3]   = i;
  		}
  	}

  	closest[0] = sqrtf( closest[0] );
  	closest[1] = sqrtf( closest[1] );
  	closest[2] = sqrtf( closest[2] );
  	closest[3] = sqrtf( closest[3] );

  	fifthbone  = sqrtf(fifthbone);
  }

	int GetBoneCount(void) const { return mBoneCount; };

	const MeshImportBone& GetBone(int index) const { return mBones[index]; };

	MeshImportBone * GetBonePtr(int index) const { return &mBones[index]; };

	void SetBone(int index,const MeshImportBone &b) { mBones[index] = b; };

	const char * GetName(void) const { return mName; };

private:
	char            mName[MAXSTRLEN];
	int             mBoneCount;
	MeshImportBone *mBones;
};


class MeshImportAnimPose
{
public:

	void SetPose(const float *pos,const float *quat)
	{
		mPos[0] = pos[0];
		mPos[1] = pos[1];
		mPos[2] = pos[2];
		mQuat[0] = quat[0];
		mQuat[1] = quat[1];
		mQuat[2] = quat[2];
		mQuat[3] = quat[3];
	};

	void Sample(float *pos,float *quat) const
	{
		pos[0] = mPos[0];
		pos[1] = mPos[1];
		pos[2] = mPos[2];
		quat[0] = mQuat[0];
		quat[1] = mQuat[1];
		quat[2] = mQuat[2];
		quat[3] = mQuat[3];
	}

	float mPos[3];
	float mQuat[4];
};

class MeshImportAnimTrack
{
public:

	MeshImportAnimTrack(int framecount,
						float duration,
						float dtime)
	{
		mName[0] = 0;
		mFrameCount = framecount;
		mPose = MEMALLOC_NEW_ARRAY(MeshImportAnimPose,mFrameCount)[mFrameCount];
		mDuration   = duration;
		mDtime      = dtime;
	}

	MeshImportAnimTrack(const MeshImportAnimTrack &c)
	{
		strcpy(mName, c.mName );
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mPose = MEMALLOC_NEW_ARRAY(MeshImportAnimPose,mFrameCount)[mFrameCount];
		for (int i=0; i<mFrameCount; i++)
		{
			mPose[i] = c.mPose[i];
		}
	}

	~MeshImportAnimTrack(void)
	{
		MEMALLOC_DELETE_ARRAY(MeshImportAnimPose,mPose);
	}

	void SetName(const char *name)
	{
    if ( name )
  		strncpy(mName,name,MAXSTRLEN);
    else
      mName[0] = 0;
	}

	void SetPose(int frame,const float *pos,const float *quat)
	{
		if ( frame >= 0 && frame < mFrameCount )
			mPose[frame].SetPose(pos,quat);
	}

	const char * GetName(void) const { return mName; };

	void SampleAnimation(int frame,float *pos,float *quat) const
	{
		mPose[frame].Sample(pos,quat);
	}

	int GetFrameCount(void) const { return mFrameCount; };

	MeshImportAnimPose * GetPose(int index) { return &mPose[index]; };

private:
	char      mName[MAXSTRLEN]; // name of the track.
	int       mFrameCount;
	float     mDuration;
	float     mDtime;
	MeshImportAnimPose *mPose;
};

class MeshImportAnimation
{
public:
	MeshImportAnimation(const char *name,int trackcount,int framecount,float duration,float dtime)
	{
		strncpy(mName,name,MAXSTRLEN);
		mTrackCount = trackcount;
		mFrameCount = framecount;
		mTracks = (MeshImportAnimTrack **)MEMALLOC_MALLOC(sizeof(MeshImportAnimTrack*)*mTrackCount);
		mDuration  = duration;
		mDtime     = dtime;
		for (int i=0; i<trackcount; i++)
		{
			mTracks[i] = MEMALLOC_NEW(MeshImportAnimTrack)(framecount,duration,dtime);
		}
	}

	MeshImportAnimation(const MeshImportAnimation &c) // construct animation by copying an existing one
	{
		strcpy(mName, c.mName );
		mTrackCount = c.mTrackCount;
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mTracks     = (MeshImportAnimTrack **)MEMALLOC_MALLOC(sizeof(MeshImportAnimTrack*)*mTrackCount);
		for (int i=0; i<mTrackCount; i++)
		{
			mTracks[i] = MEMALLOC_NEW(MeshImportAnimTrack)( *c.mTracks[i] );
		}
	}

	~MeshImportAnimation(void)
	{
		for (int i=0; i<mTrackCount; i++)
		{
			MeshImportAnimTrack *at = mTracks[i];
			MEMALLOC_DELETE(MeshImportAnimTrack,at);
		}
		MEMALLOC_FREE(mTracks);
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
	}

	void SetTrackName(int track,const char *name)
	{
		mTracks[track]->SetName(name);
	}

	void SetTrackPose(int track,int frame,const float *pos,const float *quat)
	{
		mTracks[track]->SetPose(frame,pos,quat);
	}

	const char * GetName(void) const { return mName; };

	const MeshImportAnimTrack * LocateTrack(const char *name) const
	{
		const MeshImportAnimTrack *ret = 0;
		for (int i=0; i<mTrackCount; i++)
		{
			const MeshImportAnimTrack *t = mTracks[i];
			if ( stricmp(t->GetName(),name) == 0 )
			{
				ret = t;
				break;
			}
		}
		return ret;
	}

	int GetFrameIndex(float t) const
	{
		t = fmodf( t, mDuration );
		int index = int(t / mDtime);
		return index;
	}

	int GetTrackCount(void) const { return mTrackCount; };
	float GetDuration(void) const { return mDuration; };

	MeshImportAnimTrack * GetTrack(int index)
	{
		MeshImportAnimTrack *ret = 0;
		if ( index >= 0 && index < mTrackCount )
		{
			ret = mTracks[index];
		}
		return ret;
	};

	int GetFrameCount(void) const { return mFrameCount; };
	float GetDtime(void) const { return mDtime; };

private:
	char        mName[MAXSTRLEN];
	int         mTrackCount;
	int         mFrameCount;
	float       mDuration;
	float       mDtime;
	MeshImportAnimTrack **mTracks;
};



class MeshImportMaterial
{
public:
  const char *mName;
  const char *mMetaData;
};

class MeshAABB
{
public:
  float mBmin[3];
  float mBmax[3];
};

class SubMesh
{
public:
  MeshImportMaterial  *mMaterial;
  MeshAABB             mAABB;
  MeshImportVertexFlag mVertexFlags; // defines which vertex components are active.
  unsigned int         mVertexCount; // number of vertices
  MeshImportVertex    *mVertices;
  unsigned int         mTriCount;    // number of triangles.
  unsigned int        *mIndices;     // indexed triange list
};

class Mesh
{
public:
  MeshImportSkeleton *mSkeleton; // the skeleton used by this mesh system.
  MeshAABB            mAABB;
  unsigned int        mSubMeshCount;
  SubMesh            *mSubMeshes;
};

class MeshRawTexture
{
public:
  const char *mName;
  unsigned char *mData;
  unsigned int   mWidth;
  unsigned int   mHeight;
};

class MeshInstance
{
public:
  Mesh   *mMesh;     
  float   mPosition[3];
  float   mRotation[4]; //quaternion
  float   mScale[3];
};

class MeshSystem
{
public:
  MeshSystem(void)
  {
    mTextureCount = 0;
    mTextures     = 0;
    mSkeletonCount = 0;
    mSkeletons    = 0;
    mAnimationCount = 0;
    mAnimations = 0;
    mMaterialCount = 0;
    mMaterials = 0;
    mMeshCount = 0;
    mMeshes = 0;
    mMeshInstanceCount = 0;
    mMeshInstances = 0;
  }

  unsigned int          mTextureCount;          // Are textures necessary? [rgd]. 
  MeshRawTexture       *mTextures;              // Texture storage in mesh data is rare, and the name is simply an attribute of the material

  unsigned int          mSkeletonCount;         // number of skeletons
  MeshImportSkeleton   *mSkeletons;             // the skeletons.

  unsigned int          mAnimationCount;
  MeshImportAnimation  *mAnimations;

  unsigned int          mMaterialCount;         // Materials are owned by this list, merely referenced later.
  MeshImportMaterial   *mMaterials;

  unsigned int          mMeshCount;
  Mesh                 *mMeshes;

  unsigned int          mMeshInstanceCount;
  MeshInstance         *mMeshInstances; 
};

typedef int MaterialRef;
typedef int MeshRef;

class MeshImportInterface
{
public:
  // Material refs are used to by import triangle functions to triangles to have
  // assigned materials regardless of the order that materials are imported by the importer.
  virtual MaterialRef createMaterialRef(const char *matName) = 0; 
  
  // Creates a slot for a mesh. Each vertex import function fills a specific mesh.
  virtual MeshRef     createMeshRef(const char *meshName) = 0;

  virtual void        importAssetName(const char *assetName,const char *info) = 0;
  virtual MaterialRef importMaterial(const char *matName,const char *metaData) = 0;
  virtual void        importTriangle(MeshRef meshref, MaterialRef matref, unsigned int vertexFlags,const MeshImportVertex verts[3]) = 0;
  virtual void        importTriangle(unsigned int vertexFlags,const MeshImportVertex verts[3]) = 0;
  virtual void        importAnimation(const MeshImportAnimation &animation) = 0;
  virtual void        importSkeleton(const MeshImportSkeleton &skeleton) = 0;
  virtual void        importRawTexture(const char *textureName,const unsigned char *pixels,unsigned int wid,unsigned int hit) = 0;
  virtual void        importMeshInstance(const char *meshName,const float pos[3],const float rotation[4],const float scale[3])= 0;

};

class MeshImporter
{
public:
  virtual const char *     getExtension(void) = 0; // report the default file name extension for this mesh type.
  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options) = 0;
};



}; // end of namespace

#pragma warning(pop)

#endif
