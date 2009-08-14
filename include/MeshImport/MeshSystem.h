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

enum MeshVertexFlag
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

class MeshVertex
{
public:
  MeshVertex(void)
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
  NxF32          mPos[3];
  NxF32          mNormal[3];
  NxU32   mColor;
  NxF32          mTexel1[2];
  NxF32          mTexel2[2];
  NxF32          mTexel3[2];
  NxF32          mTexel4[2];
  NxF32          mTangent[3];
  NxF32          mBiNormal[3];
  NxF32          mWeight[4];
  unsigned short mBone[4];
};

class MeshBone
{
public:
	MeshBone(void)
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

	void Set(const char *name,NxI32 parent,const NxF32 *transform)
	{
    strncpy(mName,name,MAXSTRLEN);
		mParentIndex = parent;
		memcpy(mElement,transform,sizeof(NxF32)*16);

		ExtractOrientation(mOrientation);

		mPosition[0] = mElement[3][0];
		mPosition[1] = mElement[3][1];
		mPosition[2] = mElement[3][2];

	}

	void Set(const char *name,NxI32 parent,const NxF32 *pos,const NxF32 *rot)
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

	void GetPos(NxF32 *pos) const
	{
		pos[0] = mElement[3][0];
		pos[1] = mElement[3][1];
		pos[2] = mElement[3][2];
	}

	const char * GetName(void) const { return mName; };

	const NxF32 * GetTransform(void) const { return &mElement[0][0]; };

	void ExtractOrientation(NxF32 *rot)
	{
		NxF32 tr = mElement[0][0] + mElement[1][1] + mElement[2][2];
		if (tr > 0.0f )
		{
			NxF32 s = (NxF32) sqrtf( tr + 1.0f);
			rot[3] = s * 0.5f;
			s = 0.5f / s;
			rot[0] = (mElement[1][2] - mElement[2][1]) * s;
			rot[1] = (mElement[2][0] - mElement[0][2]) * s;
			rot[2] = (mElement[0][1] - mElement[1][0]) * s;
		}
		else
		{
			// diagonal is negative
			NxI32 nxt[3] = {1, 2, 0};
			NxF32  qa[4];

			NxI32 i = 0;

			if (mElement[1][1] > mElement[0][0]) i = 1;
			if (mElement[2][2] > mElement[i][i]) i = 2;

			NxI32 j = nxt[i];
			NxI32 k = nxt[j];

			NxF32 s = sqrtf ( ((mElement[i][i] - (mElement[j][j] + mElement[k][k])) + 1.0f) );

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
		NxF32 w = mOrientation[3];

		NxF32 xx = mOrientation[0]*mOrientation[0];
		NxF32 yy = mOrientation[1]*mOrientation[1];
		NxF32 zz = mOrientation[2]*mOrientation[2];
		NxF32 xy = mOrientation[0]*mOrientation[1];
		NxF32 xz = mOrientation[0]*mOrientation[2];
		NxF32 yz = mOrientation[1]*mOrientation[2];
		NxF32 wx = w*mOrientation[0];
		NxF32 wy = w*mOrientation[1];
		NxF32 wz = w*mOrientation[2];

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

	NxI32 GetParentIndex(void) const { return mParentIndex; };
	const NxF32 * GetPosition(void) const { return mPosition; };
	const NxF32 * GetOrientation(void) const { return mOrientation; };

  void SetTransform(const NxF32 *transform)
  {
    memcpy(mElement,transform,sizeof(mElement));
  }

	char          mName[MAXSTRLEN];
	NxI32           mParentIndex;          // array index of parent bone
	NxF32         mPosition[3];
	NxF32         mOrientation[4];
	NxF32         mElement[4][4];
};

class MeshEntry
{
public:
  MeshEntry(void)
  {
    strcpy(mName,"");
    mBone = 0;
  }
private:
  char     mName[MAXSTRLEN];
	NxI32     mBone;         // bone this mesh is associcated
};

class MeshSkeleton
{
public:
	MeshSkeleton(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
		mBoneCount = 0;
		mBones = 0;
	}

	MeshSkeleton(const char *name,NxI32 bonecount)
	{
		strncpy(mName,name,MAXSTRLEN);
		mBoneCount = bonecount;
    mBones = MEMALLOC_NEW_ARRAY(MeshBone,bonecount)[bonecount];
	}

	MeshSkeleton(const MeshSkeleton &sk)
	{
		strncpy(mName, sk.mName, MAXSTRLEN );
		mBoneCount = sk.mBoneCount;
		mBones = 0;
		if ( mBoneCount )
		{
			mBones = MEMALLOC_NEW_ARRAY(MeshBone,mBoneCount)[mBoneCount];
			memcpy(mBones,sk.mBones,sizeof(MeshBone)*mBoneCount);
		}
	}

	~MeshSkeleton(void)
	{
		MEMALLOC_DELETE_ARRAY(MeshBone,mBones);
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
	}

	void SetBones(NxI32 bcount,MeshBone *bones) // memory ownership changes hands here!!!!!!!!!!
	{
		mBoneCount = bcount;
		mBones     = bones;
	}

	void ComputeDefaultWeighting(const NxF32 *pos,NxF32 *weight,unsigned short &b1,unsigned short &b2,unsigned short &b3,unsigned short &b4) const
  {
  	NxF32 closest[4];
  	NxI32   bones[4];
  	NxF32 furthest;

  	FindFourClosest(pos,closest,bones,furthest);

  	NxF32 recip = 1.0f / furthest;

  	weight[0] = (furthest-closest[0]) * recip;
  	weight[1] = (furthest-closest[1]) * recip;
  	weight[2] = (furthest-closest[2]) * recip;
  	weight[3] = (furthest-closest[3]) * recip;

  	NxF32 total = weight[0] + weight[1] + weight[2] + weight[3];

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

	void FindFourClosest(const NxF32 *pos,NxF32 *closest,NxI32 *bones,NxF32 &fifthbone) const
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

  	MeshBone *b = mBones;
  	for (NxI32 i=0; i<mBoneCount; i++,b++)
  	{
  		NxF32 bpos[3];

  		b->GetPos(bpos);

  		NxF32 dx = bpos[0] - pos[0];
  		NxF32 dy = bpos[1] - pos[1];
  		NxF32 dz = bpos[2] - pos[2];

  		NxF32 distance = dx*dx+dy*dy+dz*dz;

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

	NxI32 GetBoneCount(void) const { return mBoneCount; };

	const MeshBone& GetBone(NxI32 index) const { return mBones[index]; };

	MeshBone * GetBonePtr(NxI32 index) const { return &mBones[index]; };

	void SetBone(NxI32 index,const MeshBone &b) { mBones[index] = b; };

	const char * GetName(void) const { return mName; };

private:
	char            mName[MAXSTRLEN];
	NxI32             mBoneCount;
	MeshBone *mBones;
};


class MeshAnimPose
{
public:

	void SetPose(const NxF32 *pos,const NxF32 *quat)
	{
		mPos[0] = pos[0];
		mPos[1] = pos[1];
		mPos[2] = pos[2];
		mQuat[0] = quat[0];
		mQuat[1] = quat[1];
		mQuat[2] = quat[2];
		mQuat[3] = quat[3];
	};

	void Sample(NxF32 *pos,NxF32 *quat) const
	{
		pos[0] = mPos[0];
		pos[1] = mPos[1];
		pos[2] = mPos[2];
		quat[0] = mQuat[0];
		quat[1] = mQuat[1];
		quat[2] = mQuat[2];
		quat[3] = mQuat[3];
	}

	NxF32 mPos[3];
	NxF32 mQuat[4];
};

class MeshAnimTrack
{
public:

	MeshAnimTrack(NxI32 framecount,
						NxF32 duration,
						NxF32 dtime)
	{
		mName[0] = 0;
		mFrameCount = framecount;
		mPose = MEMALLOC_NEW_ARRAY(MeshAnimPose,mFrameCount)[mFrameCount];
		mDuration   = duration;
		mDtime      = dtime;
	}

	MeshAnimTrack(const MeshAnimTrack &c)
	{
		strcpy(mName, c.mName );
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mPose = MEMALLOC_NEW_ARRAY(MeshAnimPose,mFrameCount)[mFrameCount];
		for (NxI32 i=0; i<mFrameCount; i++)
		{
			mPose[i] = c.mPose[i];
		}
	}

	~MeshAnimTrack(void)
	{
		MEMALLOC_DELETE_ARRAY(MeshAnimPose,mPose);
	}

	void SetName(const char *name)
	{
    if ( name )
  		strncpy(mName,name,MAXSTRLEN);
    else
      mName[0] = 0;
	}

	void SetPose(NxI32 frame,const NxF32 *pos,const NxF32 *quat)
	{
		if ( frame >= 0 && frame < mFrameCount )
			mPose[frame].SetPose(pos,quat);
	}

	const char * GetName(void) const { return mName; };

	void SampleAnimation(NxI32 frame,NxF32 *pos,NxF32 *quat) const
	{
		mPose[frame].Sample(pos,quat);
	}

	NxI32 GetFrameCount(void) const { return mFrameCount; };

	MeshAnimPose * GetPose(NxI32 index) { return &mPose[index]; };

private:
	char      mName[MAXSTRLEN]; // name of the track.
	NxI32       mFrameCount;
	NxF32     mDuration;
	NxF32     mDtime;
	MeshAnimPose *mPose;
};

class MeshAnimation
{
public:
	MeshAnimation(const char *name,NxI32 trackcount,NxI32 framecount,NxF32 duration,NxF32 dtime)
	{
		strncpy(mName,name,MAXSTRLEN);
		mTrackCount = trackcount;
		mFrameCount = framecount;
		mTracks = (MeshAnimTrack **)MEMALLOC_MALLOC(sizeof(MeshAnimTrack*)*mTrackCount);
		mDuration  = duration;
		mDtime     = dtime;
		for (NxI32 i=0; i<trackcount; i++)
		{
			mTracks[i] = MEMALLOC_NEW(MeshAnimTrack)(framecount,duration,dtime);
		}
	}

	MeshAnimation(const MeshAnimation &c) // construct animation by copying an existing one
	{
		strcpy(mName, c.mName );
		mTrackCount = c.mTrackCount;
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mTracks     = (MeshAnimTrack **)MEMALLOC_MALLOC(sizeof(MeshAnimTrack*)*mTrackCount);
		for (NxI32 i=0; i<mTrackCount; i++)
		{
			mTracks[i] = MEMALLOC_NEW(MeshAnimTrack)( *c.mTracks[i] );
		}
	}

	~MeshAnimation(void)
	{
		for (NxI32 i=0; i<mTrackCount; i++)
		{
			MeshAnimTrack *at = mTracks[i];
			MEMALLOC_DELETE(MeshAnimTrack,at);
		}
		MEMALLOC_FREE(mTracks);
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
	}

	void SetTrackName(NxI32 track,const char *name)
	{
		mTracks[track]->SetName(name);
	}

	void SetTrackPose(NxI32 track,NxI32 frame,const NxF32 *pos,const NxF32 *quat)
	{
		mTracks[track]->SetPose(frame,pos,quat);
	}

	const char * GetName(void) const { return mName; };

	const MeshAnimTrack * LocateTrack(const char *name) const
	{
		const MeshAnimTrack *ret = 0;
		for (NxI32 i=0; i<mTrackCount; i++)
		{
			const MeshAnimTrack *t = mTracks[i];
			if ( stricmp(t->GetName(),name) == 0 )
			{
				ret = t;
				break;
			}
		}
		return ret;
	}

	NxI32 GetFrameIndex(NxF32 t) const
	{
		t = fmodf( t, mDuration );
		NxI32 index = NxI32(t / mDtime);
		return index;
	}

	NxI32 GetTrackCount(void) const { return mTrackCount; };
	NxF32 GetDuration(void) const { return mDuration; };

	MeshAnimTrack * GetTrack(NxI32 index)
	{
		MeshAnimTrack *ret = 0;
		if ( index >= 0 && index < mTrackCount )
		{
			ret = mTracks[index];
		}
		return ret;
	};

	NxI32 GetFrameCount(void) const { return mFrameCount; };
	NxF32 GetDtime(void) const { return mDtime; };

private:
	char        mName[MAXSTRLEN];
	NxI32         mTrackCount;
	NxI32         mFrameCount;
	NxF32       mDuration;
	NxF32       mDtime;
	MeshAnimTrack **mTracks;
};



class MeshMaterial
{
public:
  MeshMaterial(void)
  {
    mName = 0;
    mMetaData = 0;
  }
  const char *mName;
  const char *mMetaData;
};

class MeshAABB
{
public:
  MeshAABB(void)
  {
    mMin[0] = FLT_MAX;
    mMin[1] = FLT_MAX;
    mMin[2] = FLT_MAX;
    mMax[0] = FLT_MIN;
    mMax[1] = FLT_MIN;
    mMax[2] = FLT_MIN;
  }

  void include(const NxF32 pos[3])
  {
    if ( pos[0] < mMin[0] ) mMin[0] = pos[0];
    if ( pos[1] < mMin[1] ) mMin[1] = pos[1];
    if ( pos[2] < mMin[2] ) mMin[2] = pos[2];
    if ( pos[0] > mMax[0] ) mMax[0] = pos[0];
    if ( pos[1] > mMax[1] ) mMax[1] = pos[1];
    if ( pos[2] > mMax[2] ) mMax[2] = pos[2];
  }
  NxF32 mMin[3];
  NxF32 mMax[3];
};

class SubMesh
{
public:
  SubMesh(void)
  {
    mMaterialName = 0;
    mMaterial     = 0;
    mVertexFlags  = (MESHIMPORT::MeshVertexFlag)0;
    mVertexCount  = 0;
    mVertices     = 0;
    mTriCount     = 0;
    mIndices      = 0;
  }

  const char          *mMaterialName;
  MeshMaterial        *mMaterial;
  MeshAABB             mAABB;
  MeshVertexFlag       mVertexFlags; // defines which vertex components are active.
  NxU32         mVertexCount; // number of vertices
  MeshVertex          *mVertices;
  NxU32         mTriCount;    // number of triangles.
  NxU32        *mIndices;     // indexed triange list
};

class Mesh
{
public:
  Mesh(void)
  {
    mMeshName     = 0;
    mSkeletonName = 0;
    mSkeleton     = 0;
    mSubMeshCount = 0;
    mSubMeshes    = 0;
  }
  const char         *mMeshName;
  const char         *mSkeletonName;
  MeshSkeleton       *mSkeleton; // the skeleton used by this mesh system.
  MeshAABB            mAABB;
  NxU32        mSubMeshCount;
  SubMesh           **mSubMeshes;
};

class MeshRawTexture
{
public:
  MeshRawTexture(void)
  {
    mName = 0;
    mData = 0;
    mWidth = 0;
    mHeight = 0;
  }
  const char    *mName;
  NxU8 *mData;
  NxU32   mWidth;
  NxU32   mHeight;
};

class MeshInstance
{
public:
  MeshInstance(void)
  {
    mMeshName = 0;
    mMesh     = 0;
    mPosition[0] = mPosition[1] = mPosition[2] = 0;
    mRotation[0] = mRotation[1] = mRotation[2] = mRotation[3] = 0;
    mScale[0] = mScale[1] = mScale[2] = 0;
  }
  const char  *mMeshName;
  Mesh        *mMesh;
  NxF32        mPosition[3];
  NxF32        mRotation[4]; //quaternion XYZW
  NxF32        mScale[3];
};

class MeshUserData
{
public:
  MeshUserData(void)
  {
    mUserKey = 0;
    mUserValue = 0;
  }
  const char *mUserKey;
  const char *mUserValue;
};

class MeshUserBinaryData
{
public:
  MeshUserBinaryData(void)
  {
    mName     = 0;
    mUserData = 0;
    mUserLen  = 0;
  }
  const char    *mName;
  NxU32   mUserLen;
  NxU8 *mUserData;
};

class MeshTetra
{
public:
  MeshTetra(void)
  {
    mTetraName  = 0;
    mMeshName   = 0;  // mesh the tetraheadral mesh is associated with.
    mMesh       = 0;
    mTetraCount = 0;
    mTetraData  = 0;
  }

  const char  *mTetraName;
  const char  *mMeshName;
  MeshAABB     mAABB;
  Mesh        *mMesh;
  NxU32 mTetraCount; // number of tetrahedrons
  NxF32       *mTetraData;
};

#define MESH_SYSTEM_VERSION 1 // version number of this data structure, used for binary serialization

class MeshSystem
{
public:
  MeshSystem(void)
  {
    mAssetName           = 0;
    mAssetInfo           = 0;
    mTextureCount        = 0;
    mTextures            = 0;
    mSkeletonCount       = 0;
    mSkeletons           = 0;
    mAnimationCount      = 0;
    mAnimations          = 0;
    mMaterialCount       = 0;
    mMaterials           = 0;
    mMeshCount           = 0;
    mMeshes              = 0;
    mMeshInstanceCount   = 0;
    mMeshInstances       = 0;
    mUserDataCount       = 0;
    mUserData            = 0;
    mUserBinaryDataCount = 0;
    mUserBinaryData      = 0;
    mTetraMeshCount      = 0;
    mTetraMeshes         = 0;
    mMeshSystemVersion   = MESH_SYSTEM_VERSION;
    mAssetVersion        = 0;
  }


  const char           *mAssetName;
  const char           *mAssetInfo;
  NxI32                   mMeshSystemVersion;
  NxI32                   mAssetVersion;
  MeshAABB              mAABB;
  NxU32          mTextureCount;          // Are textures necessary? [rgd].
  MeshRawTexture       *mTextures;              // Texture storage in mesh data is rare, and the name is simply an attribute of the material

  NxU32          mTetraMeshCount;        // number of tetrahedral meshes
  MeshTetra            *mTetraMeshes;           // tetraheadral meshes

  NxU32          mSkeletonCount;         // number of skeletons
  MeshSkeleton        **mSkeletons;             // the skeletons.

  NxU32          mAnimationCount;
  MeshAnimation       **mAnimations;

  NxU32          mMaterialCount;         // Materials are owned by this list, merely referenced later.
  MeshMaterial         *mMaterials;

  NxU32          mUserDataCount;
  MeshUserData        **mUserData;

  NxU32          mUserBinaryDataCount;
  MeshUserBinaryData  **mUserBinaryData;

  NxU32          mMeshCount;
  Mesh                **mMeshes;

  NxU32          mMeshInstanceCount;
  MeshInstance         *mMeshInstances;
};



}; // end of namespace

#pragma warning(pop)

#endif
