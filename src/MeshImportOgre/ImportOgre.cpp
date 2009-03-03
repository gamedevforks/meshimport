#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#pragma warning(disable:4702) // disabling a warning that only shows up when building VC7

#include "ImportOgre.h"
#include "MeshImport/MeshImport.h"
#include "common/snippets/stringdict.h"
#include "common/snippets/sutil.h"
#include "common/snippets/FastXml.h"

#pragma warning(disable:4100)
#pragma warning(disable:4996)

#define DEBUG_LOG 1

namespace MESHIMPORT
{

class MyKeyFrame
{
public:
  MyKeyFrame(float t)
  {
    mTime = t;
    mPosition[0] = 0;
    mPosition[1] = 0;
    mPosition[2] = 0;
    mOrientation[0] = 0;
    mOrientation[1] = 0;
    mOrientation[2] = 0;
    mOrientation[3] = 1;
    mScale[0] = 1;
    mScale[1] = 1;
    mScale[2] = 1;
  }

  void setOrientationFromAxisAngle(const float axis[3],float angle)
  {
    float x,y,z,w;

    x = axis[0];
    y = axis[1];
    z = axis[2];

    // required: Normalize the axis

    const float i_length =  float(1.0f) / sqrtf( x*x + y*y + z*z );

    x = x * i_length;
    y = y * i_length;
    z = z * i_length;

    // now make a clQuaternionernion out of it
    float Half = angle * float(0.5);

    w = cosf(Half);//this used to be w/o deg to rad.
    const float sin_theta_over_two = sinf(Half);

    x = x * sin_theta_over_two;
    y = y * sin_theta_over_two;
    z = z * sin_theta_over_two;

    mOrientation[0] = x;
    mOrientation[1] = y;
    mOrientation[2] = z;
    mOrientation[3] = w;
  }



  float  mTime;
  float  mPosition[3];
  float  mOrientation[4];
  float  mScale[3];
};

typedef USER_STL::vector< MyKeyFrame *> MyKeyFrameVector;

class MyAnimTrack
{
public:
  MyAnimTrack(const char *name)
  {
    mName = name;
    mCurrentKey = 0;
    mDtime = 0;
    mDuration = 0;
  }

  ~MyAnimTrack(void)
  {
    MyKeyFrameVector::iterator i;
    for (i=mKeys.begin(); i!=mKeys.end(); ++i)
    {
      MyKeyFrame *m = (*i);
      MEMALLOC_DELETE(MyKeyFrame,m);
    }
    MEMALLOC_DELETE_ARRAY(MeshAnimPose,mTrack.mPose);
  }

  void addKey(float t)
  {
    mDtime = t-mDuration;
    mDuration = t;
    mCurrentKey = MEMALLOC_NEW(MyKeyFrame)(t);
    mKeys.push_back(mCurrentKey);
  }

  MeshAnimTrack * gather(void)
  {
    mTrack.mName = mName;
    mTrack.mFrameCount = mKeys.size();
    mTrack.mDuration = mDuration;
    mTrack.mDtime    = mDtime;
    mTrack.mPose = MEMALLOC_NEW_ARRAY(MeshAnimPose,mTrack.mFrameCount)[mTrack.mFrameCount];
    for (int i=0; i<mTrack.mFrameCount; i++)
    {
      MeshAnimPose &dst = mTrack.mPose[i];
      const MyKeyFrame &src = *mKeys[i];
      dst.SetPose( src.mPosition, src.mOrientation, src.mScale );
    }
    return &mTrack;
  }

  const char       *mName;
  MyKeyFrame       *mCurrentKey;
  MyKeyFrameVector mKeys;
  MeshAnimTrack    mTrack;
  float            mDtime;
  float            mDuration;
};

typedef USER_STL::vector< MeshBone > MeshBoneVector;
typedef USER_STL::vector< MyAnimTrack * > MyAnimTrackVector;

class Face
{
public:
  int v1;
  int v2;
  int v3;
};

class MySubMesh
{
public:
  MySubMesh(int fcount,const StringRef &material)
  {
    mMaterial = material;
    mFaceCount = fcount;
    mFaceIndex = -1;
    mFaces = MEMALLOC_NEW_ARRAY(Face,mFaceCount);
  }
  ~MySubMesh(void)
  {
    MEMALLOC_DELETE_ARRAY(Face,mFaces);
  }

  void transmit(const char *fname,MeshImportInterface *callback,const MeshVertex *buffer,int vertex_count,unsigned int vertex_flags)
  {
    for (int i=0; i<mFaceCount; i++)
    {
      Face &f = mFaces[i];

      assert( f.v1 >= 0 && f.v1 < vertex_count );
      assert( f.v2 >= 0 && f.v2 < vertex_count );
      assert( f.v3 >= 0 && f.v3 < vertex_count );

      const MeshVertex &v1 = buffer[f.v1];
      const MeshVertex &v2 = buffer[f.v2];
      const MeshVertex &v3 = buffer[f.v3];

      callback->importTriangle(fname,mMaterial.Get(), vertex_flags, v1, v2, v3 );
    }
  }

  StringRef mMaterial;
  int       mFaceIndex;
  int       mFaceCount;
  Face     *mFaces;
};

typedef USER_STL::vector< MySubMesh *> MySubMeshVector;

enum OperationType
{
  OT_TRIANGLE_LIST,
  OT_LAST
};

enum NodeElement
{
  NE_EMPTY,
  NE_MESH,
  NE_SHARED_GEOMETRY,
  NE_VERTEX_BUFFER,
  NE_VERTEX,
  NE_POSITION,
  NE_NORMAL,
	NE_BONEASSIGNMENTS, // boneasssignments
	NE_COLOR_DIFFUSE, // color_diffuse
	NE_FACE,        // face
	NE_TEXCOORD,    // texcoord
	NE_VERTEX_BONE_ASSIGNMENT, // vertexboneassignment
	NE_FACES,
	NE_SKELETON_LINK,
	NE_SUBMESH,
	NE_SUBMESHES,
  NE_SKELETON,
  NE_BONES,
  NE_BONE,
  NE_ROTATION,
  NE_AXIS,
  NE_ANIMATIONS,
  NE_ANIMATION,
  NE_TRACKS,
  NE_TRACK,
  NE_KEYFRAMES,
  NE_KEYFRAME,
  NE_SCALE,
  NE_BONEHIERARCHY,
  NE_BONEPARENT,
  NE_TRANSLATE,
  NE_ROTATE,
	NE_LAST
};

enum NodeAttribute
{
  NA_EMPTY,
  NA_VERTEX_COUNT,
  NA_X,
  NA_Y,
  NA_Z,
	NA_BONE_INDEX, // boneindex
	NA_U,         // u
	NA_USE_32BITINDEXES, // use32bitindexes
	NA_USE_SHARED_VERTICES, // usesahredvertices
	NA_V,         // v
	NA_V1,        // v1
	NA_V2,        // v2
	NA_V3,        // v3
	NA_VALUE,     // value
	NA_VERTEX_INDEX, // vertexindex
	NA_WEIGHT,       // weight
	NA_POSITIONS,    // positions
	NA_NORMALS,      // normals
	NA_COLORS_DIFFUSE, // color_diffuse
  NA_COLOR_DIFFUSE,
	NA_COUNT, // count
	NA_MATERIAL, // material
	NA_NAME, // name
	NA_OPERATION_TYPE, // operationtype
	NA_TEXTURE_COORDS, // texture_coords
	NA_TEXTURE_COORD_DIMENSIONS_0, // texture_coord_dimensions_0
  NA_ID,
  NA_ANGLE,
  NA_BONE,
  NA_PARENT,
  NA_LENGTH,
  NA_TIME,
  NA_LAST
};


class MeshImportOgre : public MeshImporter, public FastXmlInterface
{
public:
  MeshImportOgre(void)
  {
    mCallback = 0;
    mVertexCount = 0;
    mVertexIndex = 0;
    mVertexBuffer = 0;
    mElement = NE_LAST;
    mUseSharedVertices = false;
    mUse32BitIndices = false;
    mPositions = false;
    mNormals = false;
    mColors = false;
    mTextureCoordDimensions = 0;
    mTextureCoords = 0;
    mOperationType = OT_LAST;
    mCurrentSubMesh = 0;
    mBoneAssignments = false;
    mBoneCount = 0;
    mCurrentBone = -1;
    mAnimation = 0;
    mCurrentAnimTrack = 0;

    mToElement.Add("mesh", NE_MESH);
    mToElement.Add("sharedgeometry",NE_SHARED_GEOMETRY);
    mToElement.Add("vertexbuffer",NE_VERTEX_BUFFER);
    mToElement.Add("vertex",NE_VERTEX);
    mToElement.Add("position",NE_POSITION);
    mToElement.Add("normal",NE_NORMAL);
    mToElement.Add("boneassignments",NE_BONEASSIGNMENTS);
    mToElement.Add("color_diffuse",NE_COLOR_DIFFUSE);
    mToElement.Add("colour_diffuse",NE_COLOR_DIFFUSE);
    mToElement.Add("face",NE_FACE);
    mToElement.Add("texcoord",NE_TEXCOORD);
    mToElement.Add("vertexboneassignment",NE_VERTEX_BONE_ASSIGNMENT);
    mToElement.Add("faces",NE_FACES);
    mToElement.Add("skeletonlink",NE_SKELETON_LINK);
    mToElement.Add("submesh",NE_SUBMESH);
    mToElement.Add("submeshes",NE_SUBMESHES);
    mToElement.Add("skeleton",NE_SKELETON);
    mToElement.Add("bones",NE_BONES);
    mToElement.Add("bone",NE_BONE);
    mToElement.Add("rotation",NE_ROTATION);
    mToElement.Add("axis",NE_AXIS);
    mToElement.Add("animations", NE_ANIMATIONS);
    mToElement.Add("animation",NE_ANIMATION);
    mToElement.Add("tracks",NE_TRACKS);
    mToElement.Add("track", NE_TRACK);
    mToElement.Add("keyframes", NE_KEYFRAMES);
    mToElement.Add("keyframe", NE_KEYFRAME);
    mToElement.Add("scale", NE_SCALE);
    mToElement.Add("bonehierarchy",NE_BONEHIERARCHY);
    mToElement.Add("boneparent", NE_BONEPARENT);
    mToElement.Add("translate", NE_TRANSLATE);
    mToElement.Add("rotate", NE_ROTATE);

    mToAttribute.Add("vertexcount",NA_VERTEX_COUNT);
    mToAttribute.Add("vertexcount",NA_VERTEX_COUNT);
    mToAttribute.Add("x",NA_X);
    mToAttribute.Add("y",NA_Y);
    mToAttribute.Add("z",NA_Z);
    mToAttribute.Add("boneindex",NA_BONE_INDEX);
    mToAttribute.Add("u",NA_U);
    mToAttribute.Add("v",NA_V);
    mToAttribute.Add("v1",NA_V1);
    mToAttribute.Add("v2",NA_V2);
    mToAttribute.Add("v3",NA_V3);
    mToAttribute.Add("value",NA_VALUE);
    mToAttribute.Add("vertexindex",NA_VERTEX_INDEX);
    mToAttribute.Add("weight",NA_WEIGHT);
    mToAttribute.Add("positions",NA_POSITIONS);
    mToAttribute.Add("normals",NA_NORMALS);
    mToAttribute.Add("colors_diffuse",NA_COLORS_DIFFUSE);
    mToAttribute.Add("colours_diffuse",NA_COLORS_DIFFUSE);
    mToAttribute.Add("color_diffuse",NA_COLOR_DIFFUSE);
    mToAttribute.Add("colour_diffuse",NA_COLOR_DIFFUSE);
    mToAttribute.Add("count",NA_COUNT);
    mToAttribute.Add("material",NA_MATERIAL);
    mToAttribute.Add("name",NA_NAME);
    mToAttribute.Add("operationtype",NA_OPERATION_TYPE);
    mToAttribute.Add("texture_coord_dimensions_0",NA_TEXTURE_COORD_DIMENSIONS_0);
    mToAttribute.Add("texture_coords",NA_TEXTURE_COORDS);
    mToAttribute.Add("usesharedvertices",NA_USE_SHARED_VERTICES);
    mToAttribute.Add("use32bitindexes",NA_USE_32BITINDEXES);
    mToAttribute.Add("id",NA_ID);
    mToAttribute.Add("angle",NA_ANGLE);
    mToAttribute.Add("bone", NA_BONE);
    mToAttribute.Add("parent", NA_PARENT);
    mToAttribute.Add("length", NA_LENGTH);
    mToAttribute.Add("time", NA_TIME);

  }

  ~MeshImportOgre(void)
  {
    release();
  }

  NodeElement getNodeElement(const char *e)
  {
    NodeElement ret = (NodeElement) mToElement.Get(e);
//    assert( ret != NE_EMPTY );
    return ret;
  };

  NodeAttribute getNodeAttribute(const char *a)
  {
    NodeAttribute ret = (NodeAttribute) mToAttribute.Get(a);
//    assert(ret!=NA_EMPTY);
    return ret;
  }



  void release(void)
  {
    MEMALLOC_DELETE_ARRAY(MeshVertex,mVertexBuffer);
    mVertexBuffer = 0;
    mVertexIndex = 0;

    MySubMeshVector::iterator i;
    for (i=mSubMeshes.begin(); i!=mSubMeshes.end(); i++)
    {
      MySubMesh *m = (*i);
      MEMALLOC_DELETE(MySubMesh,m);
    }
    mSubMeshes.clear();
    mCurrentSubMesh = 0;
    flushAnimation();
  }

  virtual const char * getExtension(int index)  // report the default file name extension for this mesh type.
  {
    return ".xml";
  }

  virtual const char * getDescription(int index)  // report the default file name extension for this mesh type.
  {
    return "Ogre3d XML Mesh Files";
  }


  virtual bool importMesh(const char *fname,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
    bool ret = false;

    mCallback = callback;

    if ( data && mCallback )
    {
      mSkeletonName = mStrings.Get(fname);

      FastXml *f = createFastXml();
      bool ok = f->processXml((const char *)data,dlen,this);
  		if ( ok )
  		{
        mCallback->importAssetName(fname,0);
        if ( strlen(mSkeletonName.Get()) > 0  && appResource )
        {
          char scratch[512];
          strncpy(scratch,fname,512);
          char *slash = (char *)lastSlash(scratch);
          if ( slash )
          {
            slash++;
            *slash = 0;
            strcat(scratch,mSkeletonName.Get());
            strcat(scratch,".xml");
          }
          else
          {
            strcpy(scratch,mSkeletonName.Get());
            strcat(scratch,".xml");
          }
          unsigned int len;
          void *mem = appResource->getApplicationResource(fname,scratch,len);
          if ( mem )
          {
            f->processXml((const char *)mem,len,this);
            appResource->releaseApplicationResource(mem);
          }
        }

        if ( !mBones.empty() )
        {
          MeshSkeleton s;
          s.SetName( mSkeletonName.Get() );
          s.mBoneCount = mBones.size();
          s.mBones = MEMALLOC_NEW_ARRAY(MeshBone,s.mBoneCount)[s.mBoneCount];
          memcpy(s.mBones,&mBones[0],sizeof(MeshBone)*s.mBoneCount);
          callback->importSkeleton(s);
          MEMALLOC_DELETE_ARRAY(MeshBone,s.mBones);
        }

        flushAnimation();

        // ok.. send the results back..
        mCallback->importMesh(fname,mSkeletonName.Get());

        unsigned int vertex_flags = 0;
        if ( mPositions ) vertex_flags|=MIVF_POSITION;
        if ( mNormals ) vertex_flags|=MIVF_NORMAL;
        if ( mColors ) vertex_flags|=MIVF_COLOR;
        if ( mTextureCoords != 0 ) vertex_flags|=MIVF_TEXEL1;
        if ( mBoneAssignments ) vertex_flags|=MIVF_BONE_WEIGHTING;

        MySubMeshVector::iterator i;
        for (i=mSubMeshes.begin(); i!=mSubMeshes.end(); i++)
        {
          MySubMesh *m = (*i);
          m->transmit(fname,mCallback,mVertexBuffer,mVertexCount,vertex_flags);
        }

  			ret = true;
  		}

      releaseFastXml(f);


      release();

    }

    return ret;
  }

  void flushAnimation(void)
  {
    if ( mCallback && mAnimation )
    {
      mAnimation->mTrackCount = mAnimTracks.size();
      if ( !mAnimTracks.empty() )
      {
        mAnimation->mTracks = MEMALLOC_NEW_ARRAY(MeshAnimTrack *,mAnimation->mTrackCount)[mAnimation->mTrackCount];
        for (int i=0; i<mAnimation->mTrackCount; i++)
        {
          MyAnimTrack *mat = mAnimTracks[i];
          MeshAnimTrack *t = mat->gather();
          mAnimation->mTracks[i] = t;
        }
      }

      mCallback->importAnimation(*mAnimation);

      MEMALLOC_DELETE_ARRAY(MeshAnimTrack *,mAnimation->mTracks);

    }
    MEMALLOC_DELETE(MeshAnimation,mAnimation);
    mAnimation = 0;
    MyAnimTrackVector::iterator i;
    for (i=mAnimTracks.begin(); i!=mAnimTracks.end(); ++i)
    {
      MyAnimTrack *mat = (*i);
      MEMALLOC_DELETE(MyAnimTrack,mat);
    }
    mAnimTracks.clear();
    mCurrentAnimTrack = 0;
  }

  virtual bool processElement(const char *elementName,         // name of the element
                              int         argc,                // number of attributes
                              const char **argv,               // list of attributes.
                              const char  *elementData,        // element data, null if none
                              int         lineno)         // line number in the source XML file
  {
    if ( !elementData ) elementData = "";
    ProcessNode(elementName,elementData);
    int acount = argc/2;
    for (int i=0; i<acount; i++)
    {
      const char *key = argv[i*2];
      const char *value = argv[i*2+1];
      ProcessAttribute(key,value);
    }
    return true; // keep on going!
  }


	void ProcessNode(const char *value,const char *data)
	{
    mElement = getNodeElement(value);
    switch ( mElement )
    {
      case NE_ANIMATION:
        flushAnimation(); // if a previous animation pending..flush it
        mAnimation = MEMALLOC_NEW(MeshAnimation);
        break;
      case NE_MESH:
        break;
      case NE_SHARED_GEOMETRY:
        break;
      case NE_VERTEX_BUFFER:
        mVertexIndex = -1;
        break;
      case NE_VERTEX:
        mVertexIndex++;
        break;
      case NE_POSITION:
        break;
      case NE_NORMAL:
        break;
  	  case NE_BONEASSIGNMENTS:
        mVertexIndex = -1;
        mBoneCount = 0;
        mBoneAssignments = true;
        break;
  	  case NE_COLOR_DIFFUSE:
        break;
  	  case NE_FACE:
        assert(mCurrentSubMesh);
        if ( mCurrentSubMesh )
        {
          mCurrentSubMesh->mFaceIndex++;
        }
        break;
  	  case NE_TEXCOORD:
        break;
  	  case NE_VERTEX_BONE_ASSIGNMENT:
        break;
    }
	}

	void ProcessAttribute(const char *aname,  // the name of the attribute
												const char *savalue) // the value of the attribute
	{
    NodeAttribute a = getNodeAttribute(aname);
    switch ( a )
    {
      case NA_ID:
        if ( mElement == NE_BONE )
        {
          mCurrentBone = atoi(savalue);
          int bsize = mBones.size();
          int breserve = mCurrentBone+1;
          if ( bsize < breserve )
          {
            for (int i=bsize; i<breserve; i++)
            {
              MeshBone b;
              mBones.push_back(b);
            }
          }
        }
        break;
      case NA_COUNT:
        if ( mElement == NE_FACES )
        {
          int fcount = atoi( savalue );
          if ( fcount > 0 )
          {
            mCurrentSubMesh = MEMALLOC_NEW(MySubMesh)(fcount,mCurrentMaterial);
            mSubMeshes.push_back(mCurrentSubMesh);
          }
        }
        break;
      case NA_VERTEX_COUNT:
        if ( mElement == NE_SHARED_GEOMETRY )
        {
          MEMALLOC_DELETE_ARRAY(MeshVertex,mVertexBuffer);
          mVertexBuffer = 0;
          mVertexCount = atoi( savalue );
          if ( mVertexCount > 0 )
          {
            mVertexIndex = -1;
            mVertexBuffer = MEMALLOC_NEW_ARRAY(MeshVertex,mVertexCount)[mVertexCount];
          }
        }
        break;
      case NA_ANGLE:
        if ( mCurrentAnimTrack && mCurrentAnimTrack->mCurrentKey && mElement == NE_ROTATE)
        {
          mCurrentAnimTrack->mCurrentKey->mOrientation[3] = (float)atof(savalue);
        }
        else if ( !mBones.empty() && mCurrentBone != -1 )
        {
          MeshBone &b = mBones[mCurrentBone];
          b.mOrientation[3] = (float)atof(savalue);
        }
        break;
      case NA_X:
      case NA_Y:
      case NA_Z:
        if ( mCurrentAnimTrack && mCurrentAnimTrack->mCurrentKey )
        {
          float *dest = 0;
          if ( mElement == NE_SCALE )
            dest = mCurrentAnimTrack->mCurrentKey->mScale;
          else if ( mElement == NE_TRANSLATE )
          {
            dest = mCurrentAnimTrack->mCurrentKey->mPosition;
          }
          else if ( mElement == NE_AXIS )
          {
            dest = mCurrentAnimTrack->mCurrentKey->mOrientation;
          }
          assert(dest);
          if ( dest )
          {
            float v = (float)atof(savalue);
            if ( a == NA_X )
              dest[0] = v;
            else if ( a == NA_Y )
              dest[1] = v;
            else
            {
              dest[2] = v;
              if ( mElement == NE_AXIS )
              {
                float angle = mCurrentAnimTrack->mCurrentKey->mOrientation[3];
                float axis[3] = { dest[0], dest[1], dest[2] };
                mCurrentAnimTrack->mCurrentKey->setOrientationFromAxisAngle(axis,angle);
                // ok..now we have to multiply this tranform times the root bone
                if ( !mBones.empty() )
                {
                  MeshBoneVector::iterator i;
                  for (i=mBones.begin(); i!=mBones.end(); ++i)
                  {
                    MeshBone &b = (*i);
                    if ( strcmp(b.mName,mCurrentAnimTrack->mName) == 0 )
                    {
                      float matrix1[16];
                      float matrix2[16];
                      MyKeyFrame *key = mCurrentAnimTrack->mCurrentKey;
                      fmi_composeTransform(b.mPosition,b.mOrientation,b.mScale,matrix1);
                      fmi_composeTransform(key->mPosition,key->mOrientation,key->mScale,matrix2);
                      float combined[16];
                      fmi_multiply(matrix2,matrix1,combined);
                      fmi_decomposeTransform(combined, key->mPosition, key->mOrientation, key->mScale );
                      break;
                    }
                  }
                }
              }
            }
          }
        }
        else if ( !mBones.empty() && mCurrentBone != -1 )
        {
          MeshBone &b = mBones[mCurrentBone];
          float v = (float)atof(savalue);
          float *dest = 0;
          if ( mElement == NE_POSITION )
            dest = b.mPosition;
          else if ( mElement == NE_AXIS )
            dest = b.mOrientation;
          else if ( mElement == NE_SCALE )
            dest = b.mScale;
          else if ( mElement == NE_TRANSLATE )
          {
            if ( mCurrentAnimTrack && mCurrentAnimTrack->mCurrentKey )
              dest = mCurrentAnimTrack->mCurrentKey->mPosition;
          }
          else if ( mElement == NE_ROTATE )
          {
            if ( mCurrentAnimTrack && mCurrentAnimTrack->mCurrentKey )
              dest = mCurrentAnimTrack->mCurrentKey->mOrientation;
          }


          if ( dest )
          {
            if ( a == NA_X )
              dest[0] = v;
            else if ( a == NA_Y )
              dest[1] = v;
            else
            {
              dest[2] = v;

              if ( mElement == NE_AXIS ) // need to convert from axis angle into a quaternion!!
              {
                // into quat here...
                float angle = dest[3];
                float axis[3] = { dest[0], dest[1], dest[2] };
                b.setOrientationFromAxisAngle(axis,angle);
              }
              else if ( mElement == NE_ROTATE )
              {
                if ( mCurrentAnimTrack && mCurrentAnimTrack->mCurrentKey )
                {
                  float angle = mCurrentAnimTrack->mCurrentKey->mOrientation[3];
                  float axis[3] = { dest[0], dest[1], dest[2] };
                  mCurrentAnimTrack->mCurrentKey->setOrientationFromAxisAngle(axis,angle);
                }
              }
            }
          }

        }
        else
        {
          assert( mVertexIndex >= 0 && mVertexIndex < mVertexCount );
          if ( mVertexIndex >= 0 && mVertexIndex < mVertexCount )
          {
            MeshVertex &vtx = mVertexBuffer[mVertexIndex];
            float v = (float)atof(savalue);
            float *dest = 0;
            if ( mElement == NE_POSITION )
              dest = vtx.mPos;
            else if ( mElement == NE_NORMAL )
              dest = vtx.mNormal;
            assert( dest );
            if ( dest )
            {
              if ( a == NA_X )
                dest[0] = v;
              else if ( a == NA_Y )
                dest[1] = v;
              else
                dest[2] = v;
            }
          }
        }
        break;
	    case NA_BONE_INDEX:
        assert( mVertexIndex >= 0 && mVertexIndex < mVertexCount && mBoneCount >= 0 && mBoneCount < 4 );
        if ( mVertexIndex >= 0 && mVertexIndex < mVertexCount && mBoneCount >= 0 && mBoneCount < 4 )
        {
          int bindex = atoi( savalue );
          assert( bindex >=0 && bindex < 65536 );
          mVertexBuffer[mVertexIndex].mBone[mBoneCount] = (unsigned short)bindex;
        }
        break;
	    case NA_U:
      case NA_V:
        assert( mVertexIndex >= 0 && mVertexIndex < mVertexCount );
        if ( mVertexIndex >= 0 && mVertexIndex < mVertexCount )
        {
          MeshVertex &v = mVertexBuffer[mVertexIndex];
          float tv = (float)atof(savalue);
          if ( a == NA_U )
            v.mTexel1[0] = tv;
          else
            v.mTexel1[1] = tv;
        }
        break;
	    case NA_USE_32BITINDEXES:
        mUse32BitIndices = getBool(savalue);
        break;
	    case NA_USE_SHARED_VERTICES:
        mUseSharedVertices = getBool(savalue);
        break;
	    case NA_V1:
	    case NA_V2:
	    case NA_V3:
        if ( mElement == NE_FACE )
        {
          assert( mCurrentSubMesh );
          if ( mCurrentSubMesh )
          {
            int i1 = atoi(savalue);
            assert( i1 >= 0 && i1 < mVertexCount );
            assert( mCurrentSubMesh->mFaceIndex >= 0 && mCurrentSubMesh->mFaceIndex < mCurrentSubMesh->mFaceCount );
            if ( mCurrentSubMesh->mFaceIndex >= 0 && mCurrentSubMesh->mFaceIndex < mCurrentSubMesh->mFaceCount )
            {
              Face &f = mCurrentSubMesh->mFaces[mCurrentSubMesh->mFaceIndex];
              if ( a == NA_V1 )
                f.v1 = i1;
              else if ( a == NA_V2 )
                f.v2 = i1;
              else
                f.v3 = i1;
            }
          }
        }
        break;
	    case NA_VALUE:
        break;
      case NA_NAME:
        if ( mElement == NE_SKELETON_LINK )
        {
          mSkeletonName = mStrings.Get(savalue);
        }
        else if ( mElement == NE_BONE )
        {
          if ( !mBones.empty() && mCurrentBone != -1 )
          {
            MeshBone &b = mBones[mCurrentBone];
            StringRef ref = mStrings.Get(savalue);
            b.SetName(ref.Get());
          }
        }
        else if ( mElement == NE_ANIMATION )
        {
          assert( mAnimation );
          StringRef ref = mStrings.Get(savalue);
          mAnimation->mName = ref.Get();
        }
        break;
      case NA_LENGTH:
        if ( mElement == NE_ANIMATION )
        {
          assert( mAnimation );
          if ( mAnimation )
          {
            int len = atoi(savalue);
            if ( len > 0 )
            {
              mAnimation->mFrameCount = len;
              mAnimation->mDtime      = 1.0f / 30.0f;
              mAnimation->mDuration   = (float)len*mAnimation->mDtime;
            }
          }
        }
        break;
      case NA_PARENT:
        if ( mElement == NE_BONEPARENT )
        {
          if ( mCurrentBone != -1 )
          {
            StringRef ref = mStrings.Get(savalue);
            int bcount = mBones.size();
            MeshBone &bp = mBones[mCurrentBone];
            for (int i=0; i<bcount; i++)
            {
              MeshBone &b = mBones[i];
              if ( b.mName == ref.Get() )
              {
                bp.mParentIndex = i;
                break;
              }
            }
          }
        }
        break;
      case NA_BONE:
        if ( mElement == NE_BONEPARENT )
        {
          StringRef ref = mStrings.Get(savalue);
          int bcount = mBones.size();
          mCurrentBone = -1;
          for (int i=0; i<bcount; i++)
          {
            MeshBone &b = mBones[i];
            if ( b.mName == ref.Get() )
            {
              mCurrentBone = i;
              break;
            }
          }
        }
        else if ( mElement == NE_TRACK )
        {
          StringRef ref = mStrings.Get(savalue);
          MyAnimTrack *mat = MEMALLOC_NEW(MyAnimTrack)(ref.Get());
          mCurrentAnimTrack = mat;
          mAnimTracks.push_back(mat);
        }
        break;
	    case NA_VERTEX_INDEX:
        if ( mElement == NE_VERTEX_BONE_ASSIGNMENT )
        {
          int v = atoi( savalue );
          if ( v != mVertexIndex )
          {
            mBoneCount = 0;
          }
          mVertexIndex = atoi( savalue );
          assert( mVertexIndex >= 0 && mVertexIndex < mVertexCount );
        }
        break;
	    case NA_WEIGHT:
        assert( mVertexIndex >= 0 && mVertexIndex < mVertexCount && mBoneCount >= 0 && mBoneCount < 4 );
        if ( mVertexIndex >= 0 && mVertexIndex < mVertexCount && mBoneCount >= 0 && mBoneCount < 4 )
        {
          float w = (float)atof( savalue );
          assert( w >= 0 && w <= 1 );
          mVertexBuffer[mVertexIndex].mWeight[mBoneCount] = w;
          mBoneCount++;
        }
        break;
			case NA_POSITIONS:    // positions
        mPositions = getBool(savalue);
				break;
      case NA_TIME:
        if ( mElement == NE_KEYFRAME )
        {
          assert( mCurrentAnimTrack );
          if ( mCurrentAnimTrack )
          {
            float t = (float)atof(savalue);
            mCurrentAnimTrack->addKey(t);
          }
        }
        break;
			case NA_NORMALS:
        mNormals = getBool(savalue);
				break;
      case NA_COLOR_DIFFUSE:
        assert( mVertexIndex >= 0 && mVertexIndex < mVertexCount );
        if ( mVertexIndex >= 0 && mVertexIndex < mVertexCount )
        {
          const char *a = savalue;
          const char *r = strstr(a," ");
          if ( r )
          {
            r++;
            const char *g = strstr(r," ");
            if ( g )
            {
              g++;
              const char *b = strstr(g," ");
              if ( b )
              {
                float af = (float)atof(a);
                float rf = (float)atof(r);
                float gf = (float)atof(g);
                float bf = (float)atof(b);
                assert( af >= 0 && af <= 1 );
                assert( rf >= 0 && rf <= 1 );
                assert( gf >= 0 && gf <= 1 );
                assert( bf >= 0 && bf <= 1 );
                unsigned int ai = (unsigned int)(af*255.0f);
                unsigned int ri = (unsigned int)(rf*255.0f);
                unsigned int gi = (unsigned int)(gf*255.0f);
                unsigned int bi = (unsigned int)(bf*255.0f);
                unsigned int color = (ai<<24)|(ri<<16)||(gi<<8)|bi;
                mVertexBuffer[mVertexIndex].mColor = color;
              }
            }
          }
        }
        break;
			case NA_COLORS_DIFFUSE:
        mColors = getBool(savalue);
				break;
      case NA_MATERIAL:
        mCurrentMaterial = mStrings.Get(savalue);
        mCallback->importMaterial(mCurrentMaterial.Get(),0);
        break;
      case NA_OPERATION_TYPE:
        if ( strcmp(savalue,"triangle_list") == 0 )
          mOperationType = OT_TRIANGLE_LIST;
        else
        {
          assert(0);
          mOperationType = OT_LAST;
        }
        break;
      case NA_TEXTURE_COORD_DIMENSIONS_0:
        mTextureCoordDimensions = atoi(savalue);
        assert(mTextureCoordDimensions==2);
        break;
      case NA_TEXTURE_COORDS:
        mTextureCoords = atoi(savalue);
        assert(mTextureCoords==1);
        break;

    }

	}

private:
  bool                 mBoneAssignments;
  bool                 mUseSharedVertices;
  bool                 mUse32BitIndices;
  bool                 mPositions;
  bool                 mNormals;
  bool                 mColors;
  int                  mTextureCoordDimensions;
  int                  mTextureCoords;

  OperationType        mOperationType;
  MeshImportInterface *mCallback;
  int                  mVertexCount;
  int                  mVertexIndex;
  MeshVertex          *mVertexBuffer;
  NodeElement          mElement; // current element being processed
  StringRef            mSkeletonName;
  StringDict           mStrings;
  StringRef            mCurrentMaterial;
  StringRef            mCurrentMesh;
  MySubMesh           *mCurrentSubMesh;
  MySubMeshVector      mSubMeshes;
  int                  mBoneCount; // which bone we are

	StringTableInt         mToElement;         // convert string to element enumeration.
	StringTableInt         mToAttribute;       // convert string to attribute enumeration

  int                    mCurrentBone;
  MeshBoneVector         mBones;
  MeshAnimation       *mAnimation;
  MyAnimTrack         *mCurrentAnimTrack;
  MyAnimTrackVector    mAnimTracks;
};

MeshImporter * createMeshImportOgre(void)
{
  MeshImportOgre *m = MEMALLOC_NEW(MeshImportOgre);
  return static_cast< MeshImporter *>(m);
}

void         releaseMeshImportOgre(MeshImporter *iface)
{
  MeshImportOgre *m = static_cast< MeshImportOgre *>(iface);
  MEMALLOC_DELETE(MeshImportOgre,m);
}

};// end of namespace
