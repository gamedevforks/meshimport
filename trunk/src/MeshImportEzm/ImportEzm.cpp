/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#pragma warning(disable:4702) // disabling a warning that only shows up when building VC7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "UserMemAlloc.h"
#include "MeshImport.h"
#include "ImportEZM.h"
#include "StringDict.h"
#include "sutil.h"
#include "stable.h"
#include "asc2bin.h"
#include "inparser.h"
#include "LegacyFastXml.h"

#pragma warning(disable:4100)
#pragma warning(disable:4996)

namespace NVSHARE
{

#define DEBUG_LOG 0

enum NodeType
{
	NT_NONE,
	NT_SCENE_GRAPH,
	NT_MESH,
	NT_ANIMATION,
	NT_SKELETON,
	NT_BONE,
	NT_MESH_SECTION,
	NT_VERTEX_BUFFER,
	NT_INDEX_BUFFER,
	NT_NODE_TRIANGLE,
	NT_NODE_INSTANCE,
	NT_ANIM_TRACK,
  NT_MESH_SYSTEM,
  NT_MESH_AABB,
  NT_SKELETONS,
  NT_ANIMATIONS,
  NT_MATERIALS,
  NT_MATERIAL,
  NT_MESHES,
  NT_MESH_COLLISION_REPRESENTATIONS,
  NT_MESH_COLLISION_REPRESENTATION,
  NT_MESH_COLLISION,
  NT_MESH_COLLISION_CONVEX,
	NT_LAST
};

enum AttributeType
{
	AT_NONE,
  AT_ASSET_NAME,
  AT_ASSET_INFO,
	AT_NAME,
	AT_COUNT,
  AT_TRIANGLE_COUNT,
	AT_PARENT,
	AT_MATERIAL,
	AT_CTYPE,
	AT_SEMANTIC,
	AT_POSITION,
	AT_ORIENTATION,
	AT_DURATION,
	AT_DTIME,
	AT_TRACK_COUNT,
	AT_FRAME_COUNT,
  AT_HAS_SCALE,
  AT_MESH_SYSTEM_VERSION,
  AT_MESH_SYSTEM_ASSET_VERSION,
  AT_MIN,
  AT_MAX,
  AT_SCALE,
  AT_META_DATA,
  AT_SKELETON,
  AT_SUBMESH_COUNT,
  AT_INFO,
  AT_TYPE,
  AT_TRANSFORM,
	AT_LAST
};

class MeshImportEZM : public MeshImporter, public FastXmlInterface, public Memalloc
{
public:
	MeshImportEZM(void)
	{
		mType     = NT_NONE;
		mBone     = 0;
		mFrameCount = 0;
		mDuration   = 0;
		mTrackCount = 0;
		mDtime      = 0;
		mTrackIndex = 0;
    mVertexFlags = 0;

		mToElement.SetCaseSensitive(false);
		mToAttribute.SetCaseSensitive(false);

		mToElement.Add("SceneGraph",                       NT_SCENE_GRAPH);
		mToElement.Add("Mesh",                             NT_MESH);
		mToElement.Add("Animation",                        NT_ANIMATION);
		mToElement.Add("Skeleton",                         NT_SKELETON);
		mToElement.Add("Bone",                             NT_BONE);
		mToElement.Add("MeshSection",                      NT_MESH_SECTION);
		mToElement.Add("VertexBuffer",                     NT_VERTEX_BUFFER);
		mToElement.Add("IndexBuffer",                      NT_INDEX_BUFFER);
		mToElement.Add("NodeTriangle",                     NT_NODE_TRIANGLE);
		mToElement.Add("NodeInstance",                     NT_NODE_INSTANCE);
		mToElement.Add("AnimTrack",                        NT_ANIM_TRACK);
    mToElement.Add("MeshSystem",                       NT_MESH_SYSTEM);
    mToElement.Add("MeshAABB",                         NT_MESH_AABB);
    mToElement.Add("Skeletons",                        NT_SKELETONS);
    mToElement.Add("Animations",                       NT_ANIMATIONS);
    mToElement.Add("Materials",                        NT_MATERIALS);
    mToElement.Add("Material",                         NT_MATERIAL);
    mToElement.Add("Meshes",                           NT_MESHES);
    mToElement.Add("MeshCollisionRepresentations",     NT_MESH_COLLISION_REPRESENTATIONS);
    mToElement.Add("MeshCollisionRepresentation",      NT_MESH_COLLISION_REPRESENTATION);
    mToElement.Add("MeshCollision",                    NT_MESH_COLLISION);
    mToElement.Add("MeshCollisionConvex",              NT_MESH_COLLISION_CONVEX);

		mToAttribute.Add("name",                           AT_NAME);
		mToAttribute.Add("count",                          AT_COUNT);
    mToAttribute.Add("triangle_count",                 AT_TRIANGLE_COUNT);
		mToAttribute.Add("parent",                         AT_PARENT);
		mToAttribute.Add("material",                       AT_MATERIAL);
		mToAttribute.Add("ctype",                          AT_CTYPE);
		mToAttribute.Add("semantic",                       AT_SEMANTIC);
		mToAttribute.Add("position",                       AT_POSITION);
		mToAttribute.Add("orientation",                    AT_ORIENTATION);
		mToAttribute.Add("duration",                       AT_DURATION);
		mToAttribute.Add("dtime",                          AT_DTIME);
		mToAttribute.Add("trackcount",                     AT_TRACK_COUNT);
		mToAttribute.Add("framecount",                     AT_FRAME_COUNT);
		mToAttribute.Add("has_scale",                      AT_HAS_SCALE);
    mToAttribute.Add("asset_name",                     AT_ASSET_NAME);
    mToAttribute.Add("asset_info",                     AT_ASSET_INFO);
    mToAttribute.Add("mesh_system_version",            AT_MESH_SYSTEM_VERSION);
    mToAttribute.Add("mesh_system_asset_version",                  AT_MESH_SYSTEM_ASSET_VERSION);
    mToAttribute.Add("min", AT_MIN);
    mToAttribute.Add("max", AT_MAX);
    mToAttribute.Add("scale", AT_SCALE);
    mToAttribute.Add("meta_data", AT_META_DATA);
    mToAttribute.Add("skeleton", AT_SKELETON);
    mToAttribute.Add("submesh_count", AT_SUBMESH_COUNT);
    mToAttribute.Add("info", AT_INFO);
    mToAttribute.Add("type", AT_TYPE);
    mToAttribute.Add("transform", AT_TRANSFORM);

    mHasScale      = false;
		mName          = 0;
		mCount         = 0;
		mParent        = 0;
		mCtype         = 0;
		mSemantic      = 0;
		mSkeleton      = 0;
		mBoneIndex     = 0;
		mIndexBuffer   = 0;
		mVertexBuffer  = 0;
    mVertices      = 0;
		mVertexCount   = 0;
		mIndexCount    = 0;
		mAnimTrack     = 0;
		mAnimation     = 0;
    mVertices      = 0;
    mMeshSystemVersion = 0;
    mMeshSystemAssetVersion = 0;
    mMeshCollisionRepresentation = 0;
    mMeshCollision = 0;
    mMeshCollisionConvex = 0;

	}

  virtual ~MeshImportEZM(void)
  {

  }

  const NxU8 * getVertex(const NxU8 *src,MeshVertex &v,const char **types,NxI32 tcount)
  {
    bool firstTexel =true;

    for (NxI32 i=0; i<tcount; i++)
    {
      const char *type = types[i];
      if ( stricmp(type,"position") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mPos[0] = scan[0];
        v.mPos[1] = scan[1];
        v.mPos[2] = scan[2];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"normal") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mNormal[0] = scan[0];
        v.mNormal[1] = scan[1];
        v.mNormal[2] = scan[2];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"color") == 0 )
      {
        const NxU32 *scan = (const NxU32 *)src;
        v.mColor = scan[0];
        src+=sizeof(NxU32);
      }
      else if ( stricmp(type,"texcoord") == 0 || stricmp(type,"texcoord1") == 0 || stricmp(type,"texel1") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        if ( firstTexel )
        {
          v.mTexel1[0] = scan[0];
          v.mTexel1[1] = scan[1];
          firstTexel =false;
        }
        else
        {
          v.mTexel2[0] = scan[0];
          v.mTexel2[1] = scan[1];
        }
        src+=sizeof(NxF32)*2;
      }
      else if ( stricmp(type,"texcoord2") == 0 || stricmp(type,"texel2") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mTexel2[0] = scan[0];
        v.mTexel2[1] = scan[1];
        src+=sizeof(NxF32)*2;
      }
      else if ( stricmp(type,"texcoord3") == 0 || stricmp(type,"texel3") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mTexel3[0] = scan[0];
        v.mTexel3[1] = scan[1];
        src+=sizeof(NxF32)*2;
      }
      else if ( stricmp(type,"texcoord4") == 0 || stricmp(type,"texel4") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mTexel4[0] = scan[0];
        v.mTexel4[1] = scan[1];
        src+=sizeof(NxF32)*2;
      }
      else if ( stricmp(type,"interp1") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp1[0] = scan[0];
        v.mInterp1[1] = scan[1];
        v.mInterp1[2] = scan[2];
        v.mInterp1[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"interp2") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp2[0] = scan[0];
        v.mInterp2[1] = scan[1];
        v.mInterp2[2] = scan[2];
        v.mInterp2[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"interp3") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp3[0] = scan[0];
        v.mInterp3[1] = scan[1];
        v.mInterp3[2] = scan[2];
        v.mInterp3[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"interp4") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp4[0] = scan[0];
        v.mInterp4[1] = scan[1];
        v.mInterp4[2] = scan[2];
        v.mInterp4[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"interp5") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp5[0] = scan[0];
        v.mInterp5[1] = scan[1];
        v.mInterp5[2] = scan[2];
        v.mInterp5[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"interp6") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp6[0] = scan[0];
        v.mInterp6[1] = scan[1];
        v.mInterp6[2] = scan[2];
        v.mInterp6[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"interp7") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp7[0] = scan[0];
        v.mInterp7[1] = scan[1];
        v.mInterp7[2] = scan[2];
        v.mInterp7[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"interp8") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mInterp8[0] = scan[0];
        v.mInterp8[1] = scan[1];
        v.mInterp8[2] = scan[2];
        v.mInterp8[3] = scan[3];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"tangent") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mTangent[0] = scan[0];
        v.mTangent[1] = scan[1];
        v.mTangent[2] = scan[2];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"binormal") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mBiNormal[0] = scan[0];
        v.mBiNormal[1] = scan[1];
        v.mBiNormal[2] = scan[2];
        src+=sizeof(NxF32)*3;
      }
      else if ( stricmp(type,"blendweights") == 0 || stricmp(type,"blendweighting") == 0 || stricmp(type,"boneweighting") == 0 )
      {
        const NxF32 * scan = (const NxF32 *)src;
        v.mWeight[0] = scan[0];
        v.mWeight[1] = scan[1];
        v.mWeight[2] = scan[2];
        v.mWeight[3] = scan[3];
        src+=sizeof(NxF32)*4;
      }
      else if ( stricmp(type,"blendindices") == 0 || stricmp(type,"blendindices") == 0 )
      {
        const unsigned short * scan = (const unsigned short *)src;
        v.mBone[0] = scan[0];
        v.mBone[1] = scan[1];
        v.mBone[2] = scan[2];
        v.mBone[3] = scan[3];
        if ( v.mWeight[0] == 0 )
        {
          v.mBone[0] = 0;
        }
        if ( v.mWeight[1] == 0 )
        {
          v.mBone[1] = 0;
        }
        if ( v.mWeight[2] == 0 )
        {
          v.mBone[2] = 0;
        }
        if ( v.mWeight[3] == 0 )
        {
          v.mBone[3] = 0;
        }

        assert( v.mBone[0] < 256 );
        assert( v.mBone[1] < 256 );
        assert( v.mBone[2] < 256 );
        assert( v.mBone[3] < 256 );
        src+=sizeof(unsigned short)*4;
      }
      else if ( stricmp(type,"radius") == 0 )
      {
        const NxF32 *scan = (const NxF32 *)src;
        v.mRadius = scan[0];
        src+=sizeof(NxF32);
      }
      else
      {
        assert(0);
      }

    }
    return src;
  }

  NxU32 validateSemantics(const char **a1,const char **a2,NxI32 count)
  {
    bool ret = true;

    for (NxI32 i=0; i<count; i++)
    {
      const char *p = a1[i];
      const char *t = a2[i];

      if ( stricmp(t,"position") == 0 ||
           stricmp(t,"normal") == 0 ||
           stricmp(t,"tangent") == 0 ||
           stricmp(t,"binormal") == 0 ||
           stricmp(t,"interp1") == 0 ||
           stricmp(t,"interp2") == 0 ||
           stricmp(t,"interp3") == 0 ||
           stricmp(t,"interp4") == 0 ||
           stricmp(t,"interp5") == 0 ||
           stricmp(t,"interp6") == 0 ||
           stricmp(t,"interp7") == 0 ||
           stricmp(t,"interp8") == 0 )
      {
        if ( stricmp(p,"fff") != 0 )
        {
          ret = false;
          break;
        }
      }
      else if ( stricmp(t,"color") == 0 )
      {
        if ( stricmp(p,"x4") != 0 )
        {
          ret = false;
          break;
        }
      }
      else if ( stricmp(t,"texel1") == 0 ||
                stricmp(t,"texel2") == 0 ||
                stricmp(t,"texel3") == 0 ||
                stricmp(t,"texel4") == 0 ||
                stricmp(t,"tecoord") == 0 ||
                stricmp(t,"texcoord1") == 0 ||
                stricmp(t,"texcoord2") == 0 ||
                stricmp(t,"texcoord3") == 0 ||
                stricmp(t,"texcoord4") == 0 )
      {
        if ( stricmp(p,"ff") != 0 )
        {
          ret =false;
          break;
        }
      }
      else if ( stricmp(t,"blendweights") == 0 )
      {
        if ( stricmp(p,"ffff") != 0 )
        {
          ret = false;
          break;
        }
      }
      else if ( stricmp(t,"blendindices") == 0 )
      {
        if ( stricmp(p,"hhhh") != 0 )
        {
          ret = false;
          break;
        }
      }
      else if ( stricmp(t,"radius") == 0 )
      {
        if ( stricmp(p,"f") != 0 )
        {
          ret = false;
          break;
        }
      }
    }

    NxU32 flags = 0;

    if ( ret )
    {
      for (NxI32 i=0; i<count; i++)
      {
        const char *t = a2[i];

             if ( stricmp(t,"position") == 0 ) flags|=MIVF_POSITION;
        else if ( stricmp(t,"normal") == 0 )   flags|=MIVF_NORMAL;
        else if ( stricmp(t,"color") == 0 )    flags|=MIVF_COLOR;
        else if ( stricmp(t,"texel1") == 0 )   flags|=MIVF_TEXEL1;
        else if ( stricmp(t,"texel2") == 0 )   flags|=MIVF_TEXEL2;
        else if ( stricmp(t,"texel3") == 0 )   flags|=MIVF_TEXEL3;
        else if ( stricmp(t,"texel4") == 0 )   flags|=MIVF_TEXEL4;
        else if ( stricmp(t,"texcoord1") == 0 )   flags|=MIVF_TEXEL1;
        else if ( stricmp(t,"texcoord2") == 0 )   flags|=MIVF_TEXEL2;
        else if ( stricmp(t,"texcoord3") == 0 )   flags|=MIVF_TEXEL3;
        else if ( stricmp(t,"texcoord4") == 0 )   flags|=MIVF_TEXEL4;
        else if ( stricmp(t,"texcoord") == 0 )
        {
          if ( flags & MIVF_TEXEL1 )
            flags|=MIVF_TEXEL2;
          else
            flags|=MIVF_TEXEL1;
        }
        else if ( stricmp(t,"tangent") == 0) flags|=MIVF_TANGENT;
        else if ( stricmp(t,"interp1") == 0) flags|=MIVF_INTERP1;
        else if ( stricmp(t,"interp2") == 0) flags|=MIVF_INTERP2;
        else if ( stricmp(t,"interp3") == 0) flags|=MIVF_INTERP3;
        else if ( stricmp(t,"interp4") == 0) flags|=MIVF_INTERP4;
        else if ( stricmp(t,"interp5") == 0) flags|=MIVF_INTERP5;
        else if ( stricmp(t,"interp6") == 0) flags|=MIVF_INTERP6;
        else if ( stricmp(t,"interp7") == 0) flags|=MIVF_INTERP7;
        else if ( stricmp(t,"interp8") == 0) flags|=MIVF_INTERP8;
        else if ( stricmp(t,"binormal") == 0 ) flags|=MIVF_BINORMAL;
        else if ( stricmp(t,"blendweights") == 0 ) flags|=MIVF_BONE_WEIGHTING;
        else if ( stricmp(t,"blendindices") == 0 ) flags|=MIVF_BONE_WEIGHTING;
        else if ( stricmp(t,"boneweights") == 0 ) flags|=MIVF_BONE_WEIGHTING;
        else if ( stricmp(t,"boneindices") == 0 ) flags|=MIVF_BONE_WEIGHTING;
        else if ( stricmp(t,"radius") == 0 ) flags|=MIVF_RADIUS;
      }
    }

    return flags;
  }


  virtual const char * getExtension(NxI32 index)  // report the default file name extension for this mesh type.
  {
    return ".ezm";
  }

  virtual const char * getDescription(NxI32 index)
  {
    return "PhysX Rocket EZ-Mesh format";
  }

  virtual bool  importMesh(const char *meshName,const void *data,NxU32 dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
	  bool ret = false;

	  mCallback = callback;

	  if ( data && mCallback )
	  {
		  FastXml *f = createFastXml();
		  bool ok = f->processXml((const char *)data,dlen,this);
		  if ( ok )
		  {
			  mCallback->importAssetName(mAssetName.Get(), mAssetInfo.Get());
			  ret = true;
		  }
		  if ( mAnimation )
		  {
			  mCallback->importAnimation(*mAnimation);
			  for (NxI32 i=0; i<mAnimation->mTrackCount; i++)
			  {
				  MeshAnimTrack *t = mAnimation->mTracks[i];
				  delete []t->mPose;
				  delete t;
			  }
			  delete []mAnimation->mTracks;
			  delete mAnimation;
			  mAnimation = 0;
		  }

		  delete mMeshCollisionRepresentation;
		  delete mMeshCollision;
		  delete mMeshCollisionConvex;
		  mMeshCollisionRepresentation = 0;
		  mMeshCollision = 0;
		  mMeshCollisionConvex = 0;

		  releaseFastXml(f);

	  }

	  return ret;
  }

	void ProcessNode(const char *svalue)
	{
    mType = (NodeType)mToElement.Get(svalue);
		switch ( mType )
		{
      case NT_NONE:
        assert(0);
        break;
      case NT_MESH_COLLISION_REPRESENTATION:
        delete mMeshCollisionRepresentation;
        mMeshCollisionRepresentation = MEMALLOC_NEW(MeshCollisionRepresentation);
        break;
      case NT_MESH_COLLISION:
        if ( mMeshCollisionRepresentation )
        {
          mCallback->importCollisionRepresentation( mMeshCollisionRepresentation->mName, mMeshCollisionRepresentation->mInfo );
          mCollisionRepName = SGET(mMeshCollisionRepresentation->mName);
          delete mMeshCollisionRepresentation;
          mMeshCollisionRepresentation = 0;
        }
        delete mMeshCollision;
        mMeshCollision = MEMALLOC_NEW(MeshCollision);
        break;
      case NT_MESH_COLLISION_CONVEX:
        assert(mMeshCollision);
        if ( mMeshCollision )
        {
          delete mMeshCollisionConvex;
          mMeshCollisionConvex = MEMALLOC_NEW(MeshCollisionConvex);
          MeshCollision *d = static_cast< MeshCollision *>(mMeshCollisionConvex);
          *d = *mMeshCollision;
          delete mMeshCollision;
          mMeshCollision = 0;
        }
        break;
			case NT_ANIMATION:
				if ( mAnimation )
				{
					mCallback->importAnimation(*mAnimation);
					delete mAnimation;
					mAnimation = 0;
				}
				mName       = 0;
				mFrameCount = 0;
				mDuration   = 0;
				mTrackCount = 0;
				mDtime      = 0;
				mTrackIndex = 0;
				break;
			case NT_ANIM_TRACK:
				if ( mAnimation == 0 )
				{
					if ( mName && mFrameCount && mDuration && mTrackCount && mDtime )
					{
						NxI32 framecount = atoi( mFrameCount );
						NxF32 duration = (NxF32) atof( mDuration );
						NxI32 trackcount = atoi(mTrackCount);
						NxF32 dtime = (NxF32) atof(mDtime);
						if ( trackcount >= 1 && framecount >= 1 )
						{
							mAnimation = MEMALLOC_NEW(MeshAnimation);
                  mAnimation->mName = mName;
                  mAnimation->mTrackCount = trackcount;
                  mAnimation->mFrameCount = framecount;
                  mAnimation->mDuration = duration;
                  mAnimation->mDtime = dtime;
                  mAnimation->mTracks = (MeshAnimTrack **)MEMALLOC_MALLOC(sizeof(MeshAnimTrack *)*mAnimation->mTrackCount);
                  for (NxI32 i=0; i<mAnimation->mTrackCount; i++)
                  {
                    MeshAnimTrack *track = MEMALLOC_NEW(MeshAnimTrack);
                    track->mDtime = mAnimation->mDuration;
                    track->mFrameCount = mAnimation->mFrameCount;
                    track->mDuration = mAnimation->mDuration;
                    track->mPose = MEMALLOC_NEW(MeshAnimPose)[track->mFrameCount];
                    mAnimation->mTracks[i] = track;
                  }
						}
					}
				}
				if ( mAnimation )
				{
					mAnimTrack = mAnimation->GetTrack(mTrackIndex);
					mTrackIndex++;
				}
				break;
			case NT_SKELETON:
				{
					delete mSkeleton;
					mSkeleton = MEMALLOC_NEW(MeshSkeleton);
				}
			case NT_BONE:
				if ( mSkeleton )
				{
					mBone = mSkeleton->GetBonePtr(mBoneIndex);
				}
				break;
		}
  }
  void ProcessData(const char *svalue)
  {
    if ( svalue )
    {
  		switch ( mType )
  		{
  			case NT_ANIM_TRACK:
  				if ( mAnimTrack )
  				{
  					mAnimTrack->SetName(mStrings.Get(mName).Get());
  					NxI32 count = atoi( mCount );
  					if ( count == mAnimTrack->GetFrameCount() )
  					{
                if ( mHasScale )
                {
  								NxF32 *buff = (NxF32 *) MEMALLOC_MALLOC(sizeof(NxF32)*10*count);
  								Asc2Bin(svalue, count, "fff ffff fff", buff );
  								for (NxI32 i=0; i<count; i++)
  								{
  									MeshAnimPose *p = mAnimTrack->GetPose(i);
  									const NxF32 *src = &buff[i*10];

  									p->mPos[0]  = src[0];
  									p->mPos[1]  = src[1];
  									p->mPos[2]  = src[2];

  									p->mQuat[0] = src[3];
  									p->mQuat[1] = src[4];
  									p->mQuat[2] = src[5];
  									p->mQuat[3] = src[6];

                    p->mScale[0] = src[7];
                    p->mScale[1] = src[8];
                    p->mScale[2] = src[9];
  								}
                }
                else
                {
  								NxF32 *buff = (NxF32 *) MEMALLOC_MALLOC(sizeof(NxF32)*7*count);
  								Asc2Bin(svalue, count, "fff ffff", buff );
  								for (NxI32 i=0; i<count; i++)
  								{
  									MeshAnimPose *p = mAnimTrack->GetPose(i);
  									const NxF32 *src = &buff[i*7];

  									p->mPos[0]  = src[0];
  									p->mPos[1]  = src[1];
  									p->mPos[2]  = src[2];

  									p->mQuat[0] = src[3];
  									p->mQuat[1] = src[4];
  									p->mQuat[2] = src[5];
  									p->mQuat[3] = src[6];

                    p->mScale[0] = 1;
                    p->mScale[1] = 1;
                    p->mScale[2] = 1;
  								}
                }
  					}
  				}
  				break;
  			case NT_NODE_INSTANCE:
            #if 0 // TODO TODO
  				if ( mName )
  				{
  					NxF32 transform[4*4];
  					Asc2Bin(svalue, 4, "ffff", transform );
              MeshBone b;
              b.SetTransform(transform);
              NxF32 pos[3];
              NxF32 quat[3];
              NxF32 scale[3] = { 1, 1, 1 };
              b.ExtractOrientation(quat);
              b.GetPos(pos);
  					mCallback->importMeshInstance(mName,pos,quat,scale);
  					mName = 0;
  				}
            #endif
  				break;
  			case NT_NODE_TRIANGLE:
  				if ( mCtype && mSemantic )
  				{
              NxI32 c1,c2;
              char scratch1[2048];
              char scratch2[2048];
              strcpy(scratch1,mCtype);
              strcpy(scratch2,mSemantic);
              const char **a1 = mParser1.GetArglist(scratch1,c1);
              const char **a2 = mParser2.GetArglist(scratch2,c2);
              if ( c1 > 0 && c2 > 0 && c1 == c2 )
              {
                mVertexFlags = validateSemantics(a1,a2,c1);
                if ( mVertexFlags )
                {
  								MeshVertex vtx[3];
  								const NxU8 *temp = (const NxU8 *)Asc2Bin(svalue, 3, mCtype, 0 );
                  temp = getVertex(temp,vtx[0],a2,c2);
                  temp = getVertex(temp,vtx[1],a2,c2);
                  temp = getVertex(temp,vtx[2],a2,c2);
                  mCallback->importTriangle(mCurrentMesh.Get(),mCurrentMaterial.Get(),mVertexFlags,vtx[0],vtx[1],vtx[2]);
                  MEMALLOC_FREE((void *)temp);
                }
  					}
  					mCtype = 0;
  					mSemantic = 0;
  				}
  				break;
  			case NT_VERTEX_BUFFER:
  	      MEMALLOC_FREE( mVertexBuffer);
            delete []mVertices;
            mVertices = 0;
  				mVertexCount = 0;
  				mVertexBuffer = 0;

  				if ( mCtype && mCount )
  				{
  					mVertexCount  = atoi(mCount);
  					if ( mVertexCount > 0 )
  					{
  						mVertexBuffer = Asc2Bin(svalue, mVertexCount, mCtype, 0 );

                if ( mVertexBuffer )
                {

                  NxI32 c1,c2;
                  char scratch1[2048];
                  char scratch2[2048];
                  strcpy(scratch1,mCtype);
                  strcpy(scratch2,mSemantic);

                  const char **a1 = mParser1.GetArglist(scratch1,c1);
                  const char **a2 = mParser2.GetArglist(scratch2,c2);

                  if ( c1 > 0 && c2 > 0 && c1 == c2 )
                  {
                    mVertexFlags = validateSemantics(a1,a2,c1);
                    if ( mVertexFlags )
                    {
                      mVertices = MEMALLOC_NEW(MeshVertex)[mVertexCount];
                      const NxU8 *scan = (const NxU8 *)mVertexBuffer;
                      for (NxI32 i=0; i<mVertexCount; i++)
                      {
                        scan = getVertex(scan,mVertices[i],a2,c2);
                      }
                    }
                  }
      			      MEMALLOC_FREE( mVertexBuffer);
                  mVertexBuffer = 0;
                }

  					}
  					mCtype = 0;
  					mCount = 0;
  				}
  				break;
  			case NT_INDEX_BUFFER:
  				if ( mCount )
  				{
  					mIndexCount = atoi(mCount);
  					if ( mIndexCount > 0 )
  					{
  						mIndexBuffer = Asc2Bin(svalue, mIndexCount, "ddd", 0 );
  					}
  				}

  				if ( mIndexBuffer && mVertices )
  				{
              if ( mMeshCollisionConvex )
              {
                NxF32 *vertices = (NxF32 *)MEMALLOC_MALLOC(sizeof(NxF32)*mVertexCount*3);
                NxF32 *dest = vertices;
                for (NxI32 i=0; i<mVertexCount; i++)
                {
                  dest[0] = mVertices[i].mPos[0];
                  dest[1] = mVertices[i].mPos[1];
                  dest[2] = mVertices[i].mPos[2];
                  dest+=3;
                }

                mCallback->importConvexHull(mCollisionRepName.Get(),
                                            mMeshCollisionConvex->mName,
                                            mMeshCollisionConvex->mTransform,
                                            mVertexCount,
                                            vertices,
                                            mIndexCount,
                                            (const NxU32 *)mIndexBuffer);

                delete []vertices;
                delete mMeshCollisionConvex;
                mMeshCollisionConvex = 0;
              }
              else
              {
                mCallback->importIndexedTriangleList(mCurrentMesh.Get(),mCurrentMaterial.Get(),mVertexFlags,mVertexCount,mVertices,mIndexCount,(const NxU32 *)mIndexBuffer );
              }
  					}


  				MEMALLOC_FREE( mIndexBuffer);
  				mIndexBuffer = 0;
  				mIndexCount = 0;
  				break;
  		}
		}
	}

	void ProcessAttribute(const char *aname,  // the name of the attribute
												const char *savalue) // the value of the attribute
	{
		AttributeType attrib = (AttributeType) mToAttribute.Get(aname);
		switch ( attrib )
		{
      case AT_NONE:
        assert(0);
        break;
      case AT_MESH_SYSTEM_VERSION:
        mMeshSystemVersion = atoi(savalue);
        break;
      case AT_MESH_SYSTEM_ASSET_VERSION:
        mMeshSystemAssetVersion = atoi(savalue);
        break;
      case AT_ASSET_NAME:
        mAssetName = mStrings.Get(savalue);
        break;
      case AT_ASSET_INFO:
        mAssetInfo = mStrings.Get(savalue);
        break;
	  case AT_SCALE:
		  if ( mType == NT_BONE && mBone )
		  {
			  Asc2Bin(savalue,1,"fff", mBone->mScale );
		  }
		  break;
		case AT_POSITION:
			if ( mType == NT_BONE && mBone )
			{
				Asc2Bin(savalue,1,"fff", mBone->mPosition );
				mBoneIndex++;
				if ( mBoneIndex == mSkeleton->GetBoneCount() )
				{
					mCallback->importSkeleton(*mSkeleton);
					delete mSkeleton;
					mSkeleton = 0;
					mBoneIndex = 0;
				}
			}
			break;
			case AT_ORIENTATION:
				if ( mType == NT_BONE && mBone )
				{
					Asc2Bin(savalue,1,"ffff", mBone->mOrientation );
				}
				break;
      case AT_HAS_SCALE:
        mHasScale = getBool(savalue);
        break;
			case AT_DURATION:
				mDuration = savalue;
				break;

			case AT_DTIME:
				mDtime = savalue;
				break;

			case AT_TRACK_COUNT:
				mTrackCount = savalue;
				break;

			case AT_FRAME_COUNT:
				mFrameCount = savalue;
				break;
      case AT_INFO:
        switch ( mType )
        {
          case NT_MESH_COLLISION_REPRESENTATION:
            assert(mMeshCollisionRepresentation);
            if ( mMeshCollisionRepresentation )
            {
              mMeshCollisionRepresentation->mInfo = mStrings.Get(savalue).Get();
            }
            break;
        }
        break;
      case AT_TRANSFORM:
        if (mType == NT_MESH_COLLISION )
        {
          assert( mMeshCollision );
          if ( mMeshCollision )
          {
            Asc2Bin(savalue,4,"ffff", mMeshCollision->mTransform );
          }
        }
        break;

			case AT_NAME:
				mName = savalue;

				switch ( mType )
				{
          case NT_MESH_COLLISION:
            assert( mMeshCollision );
            if ( mMeshCollision )
            {
              mMeshCollision->mName = mStrings.Get(savalue).Get();
            }
            break;
          case NT_MESH_COLLISION_REPRESENTATION:
            assert(mMeshCollisionRepresentation);
            if ( mMeshCollisionRepresentation )
            {
              mMeshCollisionRepresentation->mName = mStrings.Get(savalue).Get();
            }
            break;
					case NT_MESH:
            mCurrentMesh = mStrings.Get(savalue);
            mCallback->importMesh(savalue,0);
						break;
					case NT_SKELETON:
						if ( mSkeleton )
						{
							mSkeleton->SetName(mStrings.Get(savalue).Get());
						}
						break;
					case NT_BONE:
						if ( mBone )
						{
							mBone->SetName(mStrings.Get(savalue).Get());
						}
						break;
				}
				break;
      case AT_TRIANGLE_COUNT:
        mCount = savalue;
        break;
			case AT_COUNT:
				mCount = savalue;
				if ( mType == NT_SKELETON )
				{
					if ( mSkeleton )
					{
						NxI32 count = atoi( savalue );
						if ( count > 0 )
						{
							MeshBone *bones;
							bones = MEMALLOC_NEW(MeshBone)[count];
							mSkeleton->SetBones(count,bones);
							mBoneIndex = 0;
						}
					}
				}
				break;
			case AT_PARENT:
				mParent = savalue;
				if ( mBone )
				{
					for (NxI32 i=0; i<mBoneIndex; i++)
					{
						const MeshBone &b = mSkeleton->GetBone(i);
						if ( strcmp(mParent,b.mName) == 0 )
						{
							mBone->mParentIndex = i;
							break;
						}
					}
				}
				break;
			case AT_MATERIAL:
				if ( mType == NT_MESH_SECTION )
				{
					mCallback->importMaterial(savalue,0);
          mCurrentMaterial = mStrings.Get(savalue);
				}
				break;
			case AT_CTYPE:
				mCtype = savalue;
				break;
			case AT_SEMANTIC:
				mSemantic = savalue;
				break;
		}

	}

  virtual bool processElement(const char *elementName,         // name of the element
                              NxI32         argc,                // number of attributes
                              const char **argv,               // list of attributes.
                              const char  *elementData,        // element data, null if none
                              NxI32         lineno)         // line number in the source XML file
  {
    ProcessNode(elementName);
    NxI32 acount = argc/2;
    for (NxI32 i=0; i<acount; i++)
    {
      const char *key = argv[i*2];
      const char *value = argv[i*2+1];
      ProcessAttribute(key,value);
    }
    ProcessData(elementData);
    return true;
  }




private:
  MeshImportInterface     *mCallback;

  bool                   mHasScale;

	StringTableInt         mToElement;         // convert string to element enumeration.
	StringTableInt         mToAttribute;       // convert string to attribute enumeration
	NodeType               mType;

  InPlaceParser mParser1;
  InPlaceParser mParser2;

	const char * mName;
	const char * mCount;
	const char * mParent;
	const char * mCtype;
	const char * mSemantic;

	const char * mFrameCount;
	const char * mDuration;
	const char * mTrackCount;
	const char * mDtime;

	NxI32          mTrackIndex;
	NxI32          mBoneIndex;
	MeshBone       * mBone;

	MeshAnimation  * mAnimation;
	MeshAnimTrack  * mAnimTrack;
	MeshSkeleton   * mSkeleton;
	NxI32          mVertexCount;
	NxI32          mIndexCount;
	void       * mVertexBuffer;
	void       * mIndexBuffer;

  NxI32          mMeshSystemVersion;
  NxI32          mMeshSystemAssetVersion;

  StringRef    mCurrentMesh;
  StringRef    mCurrentMaterial;
  StringDict   mStrings;
  StringRef    mAssetName;
  StringRef    mAssetInfo;

  NxU32 mVertexFlags;
  MeshVertex  *mVertices;

  StringRef                    mCollisionRepName;
  MeshCollisionRepresentation *mMeshCollisionRepresentation;
  MeshCollision               *mMeshCollision;
  MeshCollisionConvex         *mMeshCollisionConvex;


};


MeshImporter * createMeshImportEZM(void)
{
  MeshImportEZM *m = MEMALLOC_NEW(MeshImportEZM);
  return static_cast< MeshImporter *>(m);
}

void         releaseMeshImportEZM(MeshImporter *iface)
{
  MeshImportEZM *m = static_cast< MeshImportEZM *>(iface);
  delete m;
}

}; // end of namepsace
