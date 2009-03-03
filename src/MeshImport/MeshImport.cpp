#pragma warning(disable:4702) // disabling a warning that only shows up when building VC7
#include <assert.h>
#include <vector>

#include "./MeshImport/MeshImport.h"
#include "VtxWeld.h"
#include "MeshImportBuilder.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/FileInterface/FileInterface.h"
#include "common/snippets/sutil.h"
#include "common/snippets/FloatMath.h"

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

#ifndef PLUGINS_EMBEDDED
SendTextMessage *gSendTextMessage=0;
#endif

extern "C"
{
  MESHIMPORT_API MESHIMPORT::MeshImport * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services);
};

namespace MESHIMPORT
{

class MyMeshImportApplicationResource : public MeshImportApplicationResource
{
public:
  virtual void * getApplicationResource(const char *base_name,const char *resource_name,unsigned int &len)
  {
    void * ret = 0;
    len = 0;

    FILE *fph = fopen(resource_name,"rb");
    if ( fph )
    {
      fseek(fph,0L,SEEK_END);
      len = ftell(fph);
      fseek(fph,0L,SEEK_SET);
      if ( len > 0 )
      {
        ret = MEMALLOC_MALLOC(len);
        fread(ret,len,1,fph);
      }
      fclose(fph);
    }
    return ret;
  }

  virtual void   releaseApplicationResource(void *mem)
  {
    MEMALLOC_FREE(mem);
  }

};

typedef USER_STL::vector< MeshImporter * > MeshImporterVector;

class MyMeshImport : public MeshImport, public MyMeshImportApplicationResource
{
public:
  MyMeshImport(void)
  {
    mApplicationResource = this;
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

    const char *dot = lastDot(fname);
    if ( dot )
    {
      char scratch[512];
      strncpy(scratch,dot,512);
      MeshImporterVector::iterator i;
      for (i=mImporters.begin(); i!=mImporters.end(); ++i)
      {
        MeshImporter *mi = (*i);
        int count = mi->getExtensionCount();
        for (int j=0; j<count; j++)
        {
          const char *ext = mi->getExtension(j);
          if ( stricmp(ext,scratch) == 0 )
          {
            ret = mi;
            break;
          }
        }
        if ( ret ) break;
      }
    }
    return ret;
  }

  virtual void addImporter(MeshImporter *importer)  // add an additional importer
  {
    if ( importer )
    {
      mImporters.push_back(importer);
    }
    else
    {
      printf("debug me");
    }
  }

  bool importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    MeshImporter *mi = locateMeshImporter(meshName);
    if ( mi )
    {
      ret = mi->importMesh(meshName,data,dlen,callback,options,mApplicationResource);
    }

    return ret;
  }

  virtual MeshSystem * getMeshSystem(MeshSystemContainer *_b)
  {
    MeshBuilder *b = (MeshBuilder *)_b;
    return static_cast< MeshSystem *>(b);
  }

  virtual MeshSystemContainer *     createMeshSystemContainer(void)
  {
    MeshSystemContainer *ret = 0;

    MeshBuilder *b = createMeshBuilder(mApplicationResource);
    if ( b )
    {
      ret = (MeshSystemContainer *)b;
    }

    return ret;
  }

  virtual MeshSystemContainer *     createMeshSystemContainer(const char *meshName,const void *data,unsigned int dlen,const char *options) // imports and converts to a single MeshSystem data structure
  {
    MeshSystemContainer *ret = 0;

    MeshImporter *mi = locateMeshImporter(meshName);
    if ( mi )
    {
      MeshBuilder *b = createMeshBuilder(meshName,data,dlen,mi,options,mApplicationResource);
      if ( b )
      {
        ret = (MeshSystemContainer *)b;
      }
    }

    return ret;
  }

  virtual void  releaseMeshSystemContainer(MeshSystemContainer *mesh)
  {
    MeshBuilder *b = (MeshBuilder *)mesh;
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
    fi_fprintf(fph,"       <MeshAABB min=\"%s,%s,%s\" max=\"%s,%s,%s\"/>\r\n", FloatString(a.mMin[0]), FloatString(a.mMin[1]), FloatString(a.mMin[2]), FloatString(a.mMax[0]), FloatString(a.mMax[1]), FloatString(a.mMax[2]) );
  }

  void print(FILE_INTERFACE *fph,MeshRawTexture *t)
  {
    assert(0); // not yet implemented
  }

  void print(FILE_INTERFACE *fph,MeshTetra *t)
  {
    assert(0); // not yet implemented
  }

  void print(FILE_INTERFACE *fph,MeshBone &b,MeshSkeleton *s)
  {
    const char *parent = 0;

    if ( b.mParentIndex >= 0 )
    {
      assert( b.mParentIndex >= 0 && b.mParentIndex < s->mBoneCount );
      if ( b.mParentIndex >= 0 &&  b.mParentIndex < s->mBoneCount )
      {
        parent = s->mBones[b.mParentIndex].mName;
      }
    }
    if ( parent )
    {
      fi_fprintf(fph,"        <Bone name=\"%s\" parent=\"%s\" orientation=\"%s %s %s %s\" position=\"%s %s %s\" scale=\"%s %s %s\"/>\r\n",
        b.mName,
        parent,
        FloatString(b.mOrientation[0]),
        FloatString(b.mOrientation[1]),
        FloatString(b.mOrientation[2]),
        FloatString(b.mOrientation[3]),
        FloatString(b.mPosition[0]),
        FloatString(b.mPosition[1]),
        FloatString(b.mPosition[2]),
        FloatString(b.mScale[0]),
        FloatString(b.mScale[1]),
        FloatString(b.mScale[2]) );
    }
    else
    {
      fi_fprintf(fph,"        <Bone name=\"%s\" orientation=\"%s %s %s %s\" position=\"%s %s %s\" scale=\"%s %s %s\"/>\r\n",
        b.mName,
        FloatString(b.mOrientation[0]),
        FloatString(b.mOrientation[1]),
        FloatString(b.mOrientation[2]),
        FloatString(b.mOrientation[3]),
        FloatString(b.mPosition[0]),
        FloatString(b.mPosition[1]),
        FloatString(b.mPosition[2]),
        FloatString(b.mScale[0]),
        FloatString(b.mScale[1]),
        FloatString(b.mScale[2]) );
    }
  }

  void print(FILE_INTERFACE *fph,MeshSkeleton *s)
  {
    fi_fprintf(fph,"      <Skeleton name=\"%s\" count=\"%d\">\r\n", s->mName, s->mBoneCount);
    for (unsigned int i=0; i<(unsigned int)s->mBoneCount; i++)
    {
      print(fph,s->mBones[i],s);
    }
    fi_fprintf(fph,"      </Skeleton>\r\n");
  }

  void print(FILE_INTERFACE *fph,const MeshAnimPose &p)
  {
    fi_fprintf(fph,"      %s %s %s   %s %s %s %s   %s %s %s,\r\n", 
      FloatString(p.mPos[0]),
      FloatString(p.mPos[1]),
      FloatString(p.mPos[2]),
      FloatString(p.mQuat[0]),
      FloatString(p.mQuat[1]),
      FloatString(p.mQuat[2]),
      FloatString(p.mQuat[3]),
      FloatString(p.mScale[0]),
      FloatString(p.mScale[1]),
      FloatString(p.mScale[2]) );
  }

  void print(FILE_INTERFACE *fph,MeshAnimTrack *track)
  {
    fi_fprintf(fph,"        <AnimTrack name=\"%s\" count=\"%d\" has_scale=\"true\">\r\n", track->mName, track->mFrameCount);
    for (int i=0; i<track->mFrameCount; i++)
    {
      print(fph,track->mPose[i]);
    }
    fi_fprintf(fph,"        </AnimTrack>\r\n");

  }

  void print(FILE_INTERFACE *fph,MeshAnimation *a)
  {
    fi_fprintf(fph,"      <Animation name=\"%s\" trackcount=\"%d\" framecount=\"%d\" duration=\"%s\" dtime=\"%s\">\r\n", a->mName, a->mTrackCount, a->mFrameCount, FloatString( a->mDuration ), FloatString( a->mDtime) );

    for (int i=0; i<a->mTrackCount; i++)
    {
      print(fph,a->mTracks[i]);
    }

    fi_fprintf(fph,"      </Animation>\r\n");

  }

  void print(FILE_INTERFACE *fph,const MeshMaterial &m)
  {
    fi_fprintf(fph,"      <Material name=\"%s\" meta_data=\"%s\"/>\r\n", m.mName, m.mMetaData );
  }

  void print(FILE_INTERFACE *fph,MeshUserData *d)
  {
  }

  void print(FILE_INTERFACE *fph,MeshUserBinaryData *d)
  {
  }

  const char * getCtype(unsigned int flags)
  {
    mCtype.clear();

    if ( flags & MIVF_POSITION ) { mCtype+="fff ";  };
    if ( flags & MIVF_NORMAL ) { mCtype+="fff ";  };
    if ( flags & MIVF_COLOR ) { mCtype+="x4 ";  };
    if ( flags & MIVF_TEXEL1 ) { mCtype+="ff ";  };
    if ( flags & MIVF_TEXEL2 ) { mCtype+="ff ";  };
    if ( flags & MIVF_TEXEL3 ) { mCtype+="ff ";  };
    if ( flags & MIVF_TEXEL4 ) { mCtype+="ff ";  };
    if ( flags & MIVF_TANGENT ) { mCtype+="fff ";  };
    if ( flags & MIVF_BINORMAL ) { mCtype+="fff ";  };
    if ( flags & MIVF_BONE_WEIGHTING ) { mCtype+="ffff hhhh ";  };
    if ( flags & MIVF_RADIUS ) mCtype+="f ";

    if ( !mCtype.empty() )
    {
      char *foo = (char *)mCtype.c_str();
      int len =  strlen(foo);
      if ( foo[len-1] == ' ' )
      {
        foo[len-1] = 0;
      }
    }

    return mCtype.c_str();
  }

  const char * getSemantics(unsigned int flags)
  {
    mSemantic.clear();

    if ( flags & MIVF_POSITION ) { mSemantic+="position ";  };
    if ( flags & MIVF_NORMAL ) { mSemantic+="normal ";  };
    if ( flags & MIVF_COLOR ) { mSemantic+="color ";  };
    if ( flags & MIVF_TEXEL1 ) { mSemantic+="texcoord1 ";  };
    if ( flags & MIVF_TEXEL2 ) { mSemantic+="texcoord2 ";  };
    if ( flags & MIVF_TEXEL3 ) { mSemantic+="texcoord3 ";  };
    if ( flags & MIVF_TEXEL4 ) { mSemantic+="texcoord4 ";  };
    if ( flags & MIVF_TANGENT ) { mSemantic+="tangent ";  };
    if ( flags & MIVF_BINORMAL ) { mSemantic+="binormal ";  };
    if ( flags & MIVF_BONE_WEIGHTING ) { mSemantic+="blendweights blendindices ";  };
    if ( flags & MIVF_RADIUS ) mSemantic+="radius ";

    if ( !mSemantic.empty() )
    {
      char *foo = (char *)mSemantic.c_str();
      int len =  strlen(foo);
      if ( foo[len-1] == ' ' )
      {
        foo[len-1] = 0;
      }
    }


    return mSemantic.c_str();
  }

  void printVertex(FILE_INTERFACE *fph,unsigned int flags,const MeshVertex &v,unsigned int &column,bool &newRow)
  {
    if ( newRow )
    {
      if ( column != 0 )
      {
        fi_fprintf(fph,"\r\n");
      }
      newRow = false;
      column = 0;
      fi_fprintf(fph,"          ");
    }
    char scratch[1024] = { 0 };
    char temp[1024];

    if ( flags & MIVF_POSITION )
    {
      sprintf(temp,"%s %s %s ", FloatString(v.mPos[0]), FloatString(v.mPos[1]), FloatString(v.mPos[2]) );
      strcat(scratch,temp);
    }

    if ( flags & MIVF_NORMAL )
    {
      sprintf(temp,"%s %s %s ", FloatString(v.mNormal[0]), FloatString(v.mNormal[1]), FloatString(v.mNormal[2]) );
      strcat(scratch,temp);
    }

    if ( flags & MIVF_COLOR )
    {
      sprintf(temp,"%08X ", v.mColor );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_TEXEL1 )
    {
      sprintf(temp,"%s %s ", FloatString(v.mTexel1[0]), FloatString(v.mTexel1[1]) );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_TEXEL2 )
    {
      sprintf(temp,"%s %s ", FloatString(v.mTexel2[0]), FloatString(v.mTexel2[1]) );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_TEXEL3 )
    {
      sprintf(temp,"%s %s ", FloatString(v.mTexel3[0]), FloatString(v.mTexel3[1]) );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_TEXEL4 )
    {
      sprintf(temp,"%s %s ", FloatString(v.mTexel4[0]), FloatString(v.mTexel4[1]) );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_TANGENT )
    {
      sprintf(temp,"%s %s %s ", FloatString(v.mTangent[0]), FloatString(v.mTangent[1]), FloatString(v.mTangent[2]) );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_BINORMAL )
    {
      sprintf(temp,"%s %s %s ", FloatString(v.mBiNormal[0]), FloatString(v.mBiNormal[1]), FloatString(v.mBiNormal[2]) );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_BONE_WEIGHTING )
    {
      sprintf(temp,"%s %s %s %s ", FloatString(v.mWeight[0]), FloatString(v.mWeight[1]), FloatString(v.mWeight[2]), FloatString(v.mWeight[3]) );
      strcat(scratch,temp);
      sprintf(temp,"%d %d %d %d ", v.mBone[0], v.mBone[1], v.mBone[2], v.mBone[3] );
      strcat(scratch,temp);
    }
    if ( flags & MIVF_RADIUS )
    {
      sprintf(temp,"%s ", FloatString(v.mRadius) );
      strcat(scratch,temp);
    }
    strcat(scratch,",    ");
    unsigned int slen = strlen(scratch);
    fi_fprintf(fph,"%s", scratch );
    column+=slen;
    if ( column >= 160 )
      newRow = true;
  }

    void printIndex(FILE_INTERFACE *fph,const unsigned int *idx,unsigned int &column,bool &newRow)
  {
    if ( newRow )
    {
      if ( column != 0 )
      {
        fi_fprintf(fph,"\r\n");
      }
      newRow = false;
      column = 0;
      fi_fprintf(fph,"          ");
    }
    char temp[1024];
    sprintf(temp,"%d %d %d,  ", idx[0], idx[1], idx[2] );
    fi_fprintf(fph,"%s",temp);
    unsigned int slen = strlen(temp);
    column+=slen;
    if ( column >= 160 )
      newRow = true;
  }


  void print(FILE_INTERFACE *fph,SubMesh *m)
  {
    fi_fprintf(fph,"      <MeshSection material=\"%s\" ctype=\"%s\" semantic=\"%s\">\r\n", m->mMaterialName, getCtype(m->mVertexFlags), getSemantics(m->mVertexFlags) );
    printAABB(fph,m->mAABB);

    fi_fprintf(fph,"        <indexbuffer triangle_count=\"%d\">\r\n", m->mTriCount );
    const unsigned int *scan = m->mIndices;
    bool newRow = true;
    unsigned int column = 0;
    for (unsigned int i=0; i<m->mTriCount; i++)
    {
      printIndex(fph,scan,column,newRow);
      scan+=3;
    }
    fi_fprintf(fph,"\r\n");
    fi_fprintf(fph,"        </indexbuffer>\r\n");

    fi_fprintf(fph,"      </MeshSection>\r\n");
  }

  void print(FILE_INTERFACE *fph,Mesh *m)
  {
    fi_fprintf(fph,"      <Mesh name=\"%s\" skeleton=\"%s\" submesh_count=\"%d\">\r\n", m->mName, m->mSkeletonName, m->mSubMeshCount );
    printAABB(fph,m->mAABB);

    fi_fprintf(fph,"        <vertexbuffer count=\"%d\" ctype=\"%s\" semantic=\"%s\">\r\n", m->mVertexCount, getCtype(m->mVertexFlags), getSemantics(m->mVertexFlags) );

    bool newRow=true;
    unsigned int column=0;

    for (unsigned int i=0; i<m->mVertexCount; i++)
    {
      printVertex(fph, m->mVertexFlags, m->mVertices[i], column, newRow );

    }
    fi_fprintf(fph,"\r\n");
    fi_fprintf(fph,"        </vertexbuffer>\r\n");


    for (unsigned int i=0; i<m->mSubMeshCount; i++)
    {
      print(fph,m->mSubMeshes[i]);
    }

    fi_fprintf(fph,"      </Mesh>\r\n");
  }

  void print(FILE_INTERFACE *fph,MeshInstance &m)
  {
    fi_fprintf(fph,"        <MeshInstance mesh=\"%s\" position=\"%s,%s,%s\" rotation=\"%s,%s,%s,%s\" scale=\"%s,%s,%s\"/>\r\n",
      m.mMeshName,
      FloatString( m.mPosition[0] ),
      FloatString( m.mPosition[1] ),
      FloatString( m.mPosition[2] ),
      FloatString( m.mRotation[0] ),
      FloatString( m.mRotation[1] ),
      FloatString( m.mRotation[2] ),
      FloatString( m.mRotation[3] ),
      FloatString( m.mScale[0] ),
      FloatString( m.mScale[1] ),
      FloatString( m.mScale[2] ) );
  }

  const char * getTypeString(MeshCollisionType t)
  {
    const char *ret = "unknown";
    switch ( t )
    {
      case MCT_BOX: ret = "BOX"; break;
      case MCT_SPHERE: ret = "SPHERE"; break;
      case MCT_CAPSULE: ret = "CAPSULE"; break;
      case MCT_CONVEX: ret = "CONVEX"; break;
    }
    return ret;
  }

  void print(FILE_INTERFACE *fph,MeshCollisionBox *b)
  {
    fi_fprintf(fph,"       <MeshCollisionBox >\r\n");
    fi_fprintf(fph,"       </MeshCollisionBox>\r\n");
  }

  void print(FILE_INTERFACE *fph,MeshCollisionSphere *b)
  {
    fi_fprintf(fph,"       <MeshCollisionSphere >\r\n");
    fi_fprintf(fph,"       </MeshCollisionSphere>\r\n");
  }

  void print(FILE_INTERFACE *fph,MeshCollisionCapsule *b)
  {
    fi_fprintf(fph,"       <MeshCollisionCapsule >\r\n");
    fi_fprintf(fph,"       </MeshCollisionCapsule>\r\n");
  }

  void print(FILE_INTERFACE *fph,MeshCollisionConvex *m)
  {
    fi_fprintf(fph,"       <MeshCollisionConvex >\r\n");

    {
      bool newRow = true;
      fi_fprintf(fph,"         <vertexbuffer count=\"%d\" ctype=\"fff\" semantic=\"position\">\r\n", m->mVertexCount );
      for (unsigned int i=0; i<m->mVertexCount; i++)
      {
        const float *p = &m->mVertices[i*3];
        if ( newRow )
        {
          fi_fprintf(fph,"          ");
          newRow = false;
        }

        fi_fprintf(fph,"%s %s %s, ",FloatString(p[0]), FloatString(p[1]), FloatString(p[2]) );
        if ( (i&7) == 0 )
        {
          fi_fprintf(fph,"\r\n");
          newRow = true;
        }
      }
      if ( !newRow )
        fi_fprintf(fph,"\r\n");
      fi_fprintf(fph,"        </vertexbuffer>\r\n");
    }

    {
      fi_fprintf(fph,"         <indexbuffer triangle_count=\"%d\">\r\n", m->mTriCount );
      const unsigned int *scan = m->mIndices;
      bool newRow = true;
      unsigned int column = 0;
      for (unsigned int i=0; i<m->mTriCount; i++)
      {
        printIndex(fph,scan,column,newRow);
        scan+=3;
      }
      fi_fprintf(fph,"\r\n");
      fi_fprintf(fph,"        </indexbuffer>\r\n");
    }

    fi_fprintf(fph,"       </MeshCollisionConvex>\r\n");
  }

  void print(FILE_INTERFACE *fph,MeshCollision *m)
  {
    fi_fprintf(fph,"        <MeshCollision name=\"%s\" type=\"%s\" transform=\"%s %s %s %s   %s %s %s %s   %s %s %s %s   %s %s %s %s\">\r\n",
      m->mName,getTypeString(m->mType),
      FloatString( m->mTransform[0] ),
      FloatString( m->mTransform[1] ),
      FloatString( m->mTransform[2] ),
      FloatString( m->mTransform[3] ),
      FloatString( m->mTransform[4] ),
      FloatString( m->mTransform[5] ),
      FloatString( m->mTransform[6] ),
      FloatString( m->mTransform[7] ),
      FloatString( m->mTransform[8] ),
      FloatString( m->mTransform[9] ),
      FloatString( m->mTransform[10] ),
      FloatString( m->mTransform[11] ),
      FloatString( m->mTransform[12] ),
      FloatString( m->mTransform[13] ),
      FloatString( m->mTransform[14] ),
      FloatString( m->mTransform[15] ) );

    switch ( m->mType )
    {
      case MCT_BOX:
        {
          MeshCollisionBox *b = static_cast< MeshCollisionBox *>(m);
          print(fph,b);
        }
        break;
      case MCT_SPHERE:
        {
          MeshCollisionSphere *b = static_cast< MeshCollisionSphere *>(m);
          print(fph,b);
        }
        break;
      case MCT_CAPSULE:
        {
          MeshCollisionCapsule *b = static_cast< MeshCollisionCapsule *>(m);
          print(fph,b);
        }
        break;
      case MCT_CONVEX:
        {
          MeshCollisionConvex *b = static_cast< MeshCollisionConvex *>(m);
          print(fph,b);
        }
        break;
    }

    fi_fprintf(fph,"        </MeshCollision>\r\n");
  }

  void print(FILE_INTERFACE *fph,MeshCollisionRepresentation *m)
  {
    fi_fprintf(fph,"      <MeshCollisionRepresentation name=\"%s\" info=\"%s\" count=\"%d\">\r\n", m->mName, m->mInfo, m->mCollisionCount );
    for (unsigned int i=0; i<m->mCollisionCount; i++)
    {
      print(fph,m->mCollisionGeometry[i]);
    }
    fi_fprintf(fph,"      </MeshCollisionRepresentation>\r\n");

  }

  void serializeEzm(FILE_INTERFACE *fph,MeshSystem *mesh)
  {
    fi_fprintf(fph,"<?xml version=\"1.0\"?>\r\n");
    fi_fprintf(fph,"  <MeshSystem asset_name=\"%s\" asset_info=\"%s\" mesh_system_version=\"%d\" mesh_system_asset_version=\"%d\">\r\n", getStr(mesh->mAssetName), getStr(mesh->mAssetInfo), mesh->mMeshSystemVersion, mesh->mAssetVersion );
    printAABB(fph,mesh->mAABB);

    //*******************************************************************
    //***
    //***  Output Textures
    //***
    //*******************************************************************
    if ( mesh->mTextureCount )
    {
      fi_fprintf(fph,"    <Textures count=\"%d\">\r\n", mesh->mTextureCount );
      for (unsigned int i=0; i<mesh->mTextureCount; i++)
      {
        print(fph,mesh->mTextures[i]);
      }
      fi_fprintf(fph,"    </Textures>\r\n");
    }


    //*******************************************************************
    //***
    //***  Tetraheadral meshes
    //***
    //*******************************************************************
    if ( mesh->mTetraMeshCount )
    {
      fi_fprintf(fph,"    <TetraMeshes count=\"%d\">\r\n", mesh->mTetraMeshCount );
      for (unsigned int i=0; i<mesh->mTetraMeshCount; i++)
      {
        print(fph,mesh->mTetraMeshes[i]);
      }
      fi_fprintf(fph,"    </TetraMeshes>\r\n");
    }

    //*******************************************************************
    //***
    //***  Output skeletons
    //***
    //*******************************************************************
    if ( mesh->mSkeletonCount )
    {
      fi_fprintf(fph,"    <Skeletons count=\"%d\">\r\n", mesh->mSkeletonCount);
      for (unsigned int i=0; i<mesh->mSkeletonCount; i++)
      {
        print(fph,mesh->mSkeletons[i]);
      }
      fi_fprintf(fph,"    </Skeletons>\r\n");
    }

    //*******************************************************************
    //***
    //***  Output Animations
    //***
    //*******************************************************************
    if ( mesh->mAnimationCount )
    {
      fi_fprintf(fph,"    <Animations count=\"%d\">\r\n", mesh->mAnimationCount );
      for (unsigned int i=0; i<mesh->mAnimationCount; i++)
      {
        print(fph,mesh->mAnimations[i]);
      }
      fi_fprintf(fph,"    </Animations>\r\n");
    }

    //*******************************************************************
    //***
    //***  Output Materials
    //***
    //*******************************************************************
    if ( mesh->mMaterialCount )
    {
      fi_fprintf(fph,"    <Materials count=\"%d\">\r\n", mesh->mMaterialCount );
      for (unsigned int i=0; i<mesh->mMaterialCount; i++)
      {
        print(fph,mesh->mMaterials[i]);
      }
      fi_fprintf(fph,"    </Materials>\r\n", mesh->mMaterialCount );
    }


    //*******************************************************************
    //***
    //***  Output UserData
    //***
    //*******************************************************************
    // user data
    if ( mesh->mUserDataCount )
    {
      fi_fprintf(fph,"    <UserData count=\"%d\">\r\n", mesh->mUserDataCount );
      for (unsigned int i=0; i<mesh->mUserDataCount; i++)
      {
        print(fph,mesh->mUserData[i]);
      }
      fi_fprintf(fph,"    </UserData>\r\n");
    }

    //*******************************************************************
    //***
    //***  Output UserBinaryData
    //***
    //*******************************************************************
    // user data
    if ( mesh->mUserBinaryDataCount )
    {
      fi_fprintf(fph,"    <UserBinaryData count=\"%d\">\r\n", mesh->mUserBinaryDataCount );
      for (unsigned int i=0; i<mesh->mUserBinaryDataCount; i++)
      {
        print(fph,mesh->mUserBinaryData[i]);
      }
      fi_fprintf(fph,"    </UserBinaryData>\r\n");
    }


    //*******************************************************************
    //***
    //***  Output Meshes
    //***
    //*******************************************************************
    if ( mesh->mMeshCount )
    {
      fi_fprintf(fph,"    <Meshes count=\"%d\">\r\n", mesh->mMeshCount );
      for (unsigned int i=0; i<mesh->mMeshCount; i++)
      {
        print(fph,mesh->mMeshes[i]);
      }
      fi_fprintf(fph,"    </Meshes>\r\n");
    }

    //*******************************************************************
    //***
    //***  Output MeshInstances
    //***
    //*******************************************************************
    if ( mesh->mMeshInstanceCount )
    {
      fi_fprintf(fph,"    <MeshInstances count=\"%d\">\r\n", mesh->mMeshInstanceCount );
      for (unsigned int i=0; i<mesh->mMeshInstanceCount; i++)
      {
        print(fph,mesh->mMeshInstances[i]);
      }
      fi_fprintf(fph,"    </MeshInstances>\r\n");
    }

    //*******************************************************************
    //***
    //***  Output MeshCollisionRepresentations
    //***
    //*******************************************************************
    if ( mesh->mMeshCollisionCount )
    {
      fi_fprintf(fph,"    <MeshCollisionRepresentations count=\"%d\">\r\n", mesh->mMeshCollisionCount );
      for (unsigned int i=0; i<mesh->mMeshCollisionCount; i++)
      {
        print(fph,mesh->mMeshCollisionRepresentations[i]);
      }
      fi_fprintf(fph,"    </MeshCollisionRepresentations>\r\n");
    }

    fi_fprintf(fph,"  </MeshSystem>\r\n");
  }

  // ok..ready to serialize in the Ogre format..
  void serializeOgre(FILE_INTERFACE *fph,FILE_INTERFACE *exfph,MeshSystem *mesh,const char *saveName)
  {
    // ogre wants all the vertices in one big buffer..
    VertexPool< MeshVertex > bigPool;

    unsigned int vertexFlags = 0;
    for (unsigned int i=0; i<mesh->mMeshCount; i++)
    {
      Mesh *m = mesh->mMeshes[i];
      vertexFlags|=m->mVertexFlags;
      for (unsigned int k=0; k<m->mVertexCount; k++)
      {
        const MeshVertex &v = m->mVertices[k];
        bigPool.GetVertex(v);
      }
    }

    fi_fprintf(fph,"<mesh>\r\n");
    fi_fprintf(fph,"  <sharedgeometry vertexcount=\"%d\">\r\n", bigPool.GetSize() );

    fi_fprintf(fph,"    <vertexbuffer positions=\"%s\" normals=\"%s\">\r\n", (vertexFlags & MIVF_POSITION) ? "true" : "false", (vertexFlags & MIVF_NORMAL) ? "true" : "false" );
    int vcount = bigPool.GetSize();
    if ( vcount )
    {
      MeshVertex *data = bigPool.GetBuffer();
      for (int i=0; i<vcount; i++)
      {
        fi_fprintf(fph,"      <vertex>\r\n");
        fi_fprintf(fph,"        <position x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString(data->mPos[0]), FloatString(data->mPos[1]), FloatString(data->mPos[2]) );
        fi_fprintf(fph,"        <normal x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString(data->mNormal[0]), FloatString(data->mNormal[1]), FloatString(data->mNormal[2]) );
        fi_fprintf(fph,"      </vertex>\r\n");
        data++;
      }
    }
    fi_fprintf(fph,"    </vertexbuffer>\r\n");

    if ( vertexFlags & MIVF_COLOR )
    {
      fi_fprintf(fph,"    <vertexbuffer colours_diffuse=\"true\">\r\n");
      int vcount = bigPool.GetSize();
      MeshVertex *data = bigPool.GetBuffer();
      for (int i=0; i<vcount; i++)
      {
        fi_fprintf(fph,"      <vertex>\r\n");

        unsigned int a = data->mColor>>24;
        unsigned int r = (data->mColor>>16)&0xFF;
        unsigned int g = (data->mColor>>8)&0xFF;
        unsigned int b = (data->mColor&0xFF);
        float fa = (float)a*(1.0f/255.0f);
        float fr = (float)r*(1.0f/255.0f);
        float fg = (float)g*(1.0f/255.0f);
        float fb = (float)b*(1.0f/255.0f);
        fi_fprintf(fph,"        <colour_diffuse value=\"%s %s %s %s\" />\r\n",
          FloatString(fa),
          FloatString(fr),
          FloatString(fg),
          FloatString(fb) );

        fi_fprintf(fph,"      </vertex>\r\n");
        data++;
      }
      fi_fprintf(fph,"    </vertexbuffer>\r\n");
    }

    if ( vertexFlags & MIVF_TEXEL1 )
    {

      fi_fprintf(fph,"    <vertexbuffer texture_coord_dimensions_0=\"2\" texture_coords=\"1\">\r\n");
      int vcount = bigPool.GetSize();
      MeshVertex *data = bigPool.GetBuffer();
      for (int i=0; i<vcount; i++)
      {
        fi_fprintf(fph,"      <vertex>\r\n");
        fi_fprintf(fph,"        <texcoord u=\"%s\" v=\"%s\" />\r\n", FloatString(data->mTexel1[0]), FloatString(data->mTexel1[1]) );
        fi_fprintf(fph,"      </vertex>\r\n");
        data++;
      }
      fi_fprintf(fph,"    </vertexbuffer>\r\n");
    }

    fi_fprintf(fph,"   </sharedgeometry>\r\n");
    fi_fprintf(fph,"   <submeshes>\r\n");

    for (unsigned int i=0; i<mesh->mMeshCount; i++)
    {
      Mesh *m = mesh->mMeshes[i];
      for (unsigned int j=0; j<m->mSubMeshCount; j++)
      {
        SubMesh *sm = m->mSubMeshes[j];
        fi_fprintf(fph,"      <submesh material=\"%s\" usesharedvertices=\"true\" operationtype=\"triangle_list\">\r\n", sm->mMaterialName );
        fi_fprintf(fph,"        <faces count=\"%d\">\r\n", sm->mTriCount );
        for (unsigned int k=0; k<sm->mTriCount; k++)
        {
          unsigned int i1 = sm->mIndices[k*3+0];
          unsigned int i2 = sm->mIndices[k*3+1];
          unsigned int i3 = sm->mIndices[k*3+2];
          const MeshVertex &v1 = m->mVertices[i1];
          const MeshVertex &v2 = m->mVertices[i2];
          const MeshVertex &v3 = m->mVertices[i3];
          i1 = bigPool.GetVertex(v1);
          i2 = bigPool.GetVertex(v2);
          i3 = bigPool.GetVertex(v3);
          fi_fprintf(fph,"          <face v1=\"%d\" v2=\"%d\" v3=\"%d\" />\r\n", i1, i2, i3 );
        }
        fi_fprintf(fph,"       </faces>\r\n");
        fi_fprintf(fph,"       <boneassignments />\r\n");
        fi_fprintf(fph,"     </submesh>\r\n");
      }
    }
    fi_fprintf(fph,"  </submeshes>\r\n");
    if ( mesh->mSkeletonCount )
    {
      MeshSkeleton *sk = mesh->mSkeletons[0];
      if ( saveName )
      {
        const char *slash = lastSlash(saveName);
        if ( slash == 0 )
          slash = saveName;
        else
          slash++;

        char scratch[512];
        strcpy(scratch,slash);
        char *dot = stristr(scratch,".mesh.xml");
        if ( dot )
        {
          *dot = 0;
        }

        fi_fprintf(fph,"      <skeletonlink name=\"%s.skeleton\" />\r\n", scratch );
      }
      else
        fi_fprintf(fph,"      <skeletonlink name=\"%s\" />\r\n", sk->mName );
    }

    if ( vertexFlags & MIVF_BONE_WEIGHTING )
    {
      fi_fprintf(fph,"   <boneassignments>\r\n");
      int vcount = bigPool.GetSize();
      MeshVertex *data = bigPool.GetBuffer();
      for (int i=0; i<vcount; i++)
      {
        for (int j=0; j<4; j++)
        {
          if ( data->mWeight[j] == 0 ) break;
          fi_fprintf(fph,"       <vertexboneassignment vertexindex=\"%d\" boneindex=\"%d\" weight=\"%s\" />\r\n", i, data->mBone[j], FloatString(data->mWeight[j] ) );
        }
        data++;
      }
      fi_fprintf(fph,"   </boneassignments>\r\n");
    }
    fi_fprintf(fph,"</mesh>\r\n");

    // ok..now if we have a skeleton..
    fi_fprintf(exfph,"<skeleton>\r\n");
    if ( mesh->mSkeletonCount )
    {
      MeshSkeleton *skeleton = mesh->mSkeletons[0]; // only serialize one skeleton!
      fi_fprintf(exfph,"  <bones>\r\n");
      for (int i=0; i<skeleton->mBoneCount; i++)
      {
        MeshBone &b = skeleton->mBones[i];
        fi_fprintf(exfph,"    <bone id=\"%d\" name=\"%s\">\r\n", i, b.mName );
        fi_fprintf(exfph,"      <position x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString( b.mPosition[0] ), FloatString( b.mPosition[1] ), FloatString( b.mPosition[2] ) );
        float angle = 0;
        float axis[3] = { 0, 0, 0 };
        b.getAngleAxis(angle,axis);
        fi_fprintf(exfph,"      <rotation angle=\"%s\">\r\n", FloatString(angle) );
        fi_fprintf(exfph,"         <axis x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString( axis[0] ), FloatString( axis[1] ), FloatString( axis[2] ) );
        fi_fprintf(exfph,"      </rotation>\r\n");
        fi_fprintf(exfph,"      <scale x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString( b.mScale[0] ), FloatString( b.mScale[1] ), FloatString( b.mScale[2] ) );
        fi_fprintf(exfph,"    </bone>\r\n");
      }
      fi_fprintf(exfph,"  </bones>\r\n");
      fi_fprintf(exfph,"  <bonehierarchy>\r\n");
      for (int i=0; i<skeleton->mBoneCount; i++)
      {
        MeshBone &b = skeleton->mBones[i];
        if ( b.mParentIndex != -1 )
        {
          MeshBone &p = skeleton->mBones[b.mParentIndex];
          fi_fprintf(exfph,"    <boneparent bone=\"%s\" parent=\"%s\" />\r\n", b.mName, p.mName );
        }
      }
      fi_fprintf(exfph,"  </bonehierarchy>\r\n");
    }
    if ( mesh->mAnimationCount )
    {
      fi_fprintf(exfph,"   <animations>\r\n");

      for (int i=0; i<(int)mesh->mAnimationCount; i++)
      {
        MeshAnimation *anim = mesh->mAnimations[i]; // only serialize one animation
        fi_fprintf(exfph,"   <animation name=\"%s\" length=\"%d\">\r\n", anim->mName, anim->mFrameCount );
        fi_fprintf(exfph,"     <tracks>\r\n");
        for (int j=0; j<anim->mTrackCount; j++)
        {
          MeshAnimTrack *track = anim->mTracks[j];

          fi_fprintf(exfph,"  <track bone=\"%s\">\r\n", track->mName );
          fi_fprintf(exfph,"     <keyframes>\r\n");

          float tm = 0;

          float base_inverse[16];
          fmi_identity(base_inverse);
          if ( mesh->mSkeletonCount )
          {
            MeshSkeleton *sk = mesh->mSkeletons[i];
            for (int i=0; i<sk->mBoneCount; i++)
            {
              MeshBone &b = sk->mBones[i];
              if ( strcmp(b.mName,track->mName) == 0 )
              {
                // ok..compose the local space transform..
                float local_matrix[16];
                fmi_composeTransform( b.mPosition, b.mOrientation, b.mScale, local_matrix );
                fmi_inverseTransform(local_matrix,base_inverse);
              }
            }
          }


          for (int k=0; k<track->mFrameCount; k++)
          {
            MeshAnimPose &p = track->mPose[k];

            float local_matrix[16];
            fmi_composeTransform(p.mPos,p.mQuat,p.mScale,local_matrix);
            fmi_multiply(local_matrix,base_inverse,local_matrix);

            float trans[3] = { 0, 0, 0 };
            float scale[3] = { 0, 0, 0 };
            float rot[4];

            fmi_decomposeTransform(local_matrix,trans,rot,scale);

            float angle = 0;
            float axis[3] = { 0, 0, 0 };

            fmi_getAngleAxis(angle,axis,rot);

            fi_fprintf(exfph,"  <keyframe time=\"%s\">\r\n", FloatString(tm) );
            fi_fprintf(exfph,"    <translate x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString(trans[0]), FloatString(trans[1]), FloatString(trans[2]) );
            fi_fprintf(exfph,"    <rotate angle=\"%s\">\r\n", FloatString(angle) );
            fi_fprintf(exfph,"      <axis x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString(axis[0]), FloatString(axis[1]), FloatString(axis[2]) );
            fi_fprintf(exfph,"    </rotate>\r\n");
            fi_fprintf(exfph,"    <scale x=\"%s\" y=\"%s\" z=\"%s\" />\r\n", FloatString( scale[0] ), FloatString(scale[1]), FloatString(scale[2]) );
            fi_fprintf(exfph,"  </keyframe>\r\n");

            tm+=track->mDtime;
          }
          fi_fprintf(exfph,"    </keyframes>\r\n");
          fi_fprintf(exfph,"  </track>\r\n");
        }
        fi_fprintf(exfph,"     </tracks>\r\n");
        fi_fprintf(exfph,"   </animation>\r\n");
      }

      fi_fprintf(exfph,"   </animations>\r\n");
    }
    fi_fprintf(exfph,"</skeleton>\r\n");
  }

  typedef USER_STL::vector< HeU32 > HeU32Vector;

  class Msave
  {
    public:
      const char *mMaterialName;
      HeU32Vector mIndices;
  };

  typedef USER_STL::vector< Msave > MsaveVector;

  void serializeWavefront(FILE_INTERFACE *fph,FILE_INTERFACE *exfph,MeshSystem *mesh,const char *saveName,const float *exportTransform)
  {
    fi_fprintf(fph,"# Asset '%s'\r\n", mesh->mAssetName );
    char scratch[512];
    strcpy(scratch,saveName);
    char *dot = (char *)lastDot(scratch);
    if ( dot )
    {
      *dot = 0;
    }
    strcat(scratch,".mtl");
    fi_fprintf(fph,"mtllib %s\r\n", scratch );

    for (HeU32 i=0; i<mesh->mMaterialCount; i++)
    {
      const MeshMaterial &m = mesh->mMaterials[i];
      fi_fprintf(exfph,"newmtl %s\r\n", m.mName );
      char scratch[512];
      strncpy(scratch,m.mName,512);
      char *plus = strstr(scratch,"+");
      if ( plus )
        *plus = 0;
      const char *diffuse = scratch;
      fi_fprintf(exfph,"map_Ka %s\r\n", diffuse );
    }

    if  ( mesh->mMeshInstanceCount )
    {

      MsaveVector meshes;
      VertexPool< MeshVertex > bigPool;

      for (unsigned int i=0; i<mesh->mMeshInstanceCount; i++)
      {
        MeshInstance &inst =  mesh->mMeshInstances[i];
        for (unsigned int j=0; j<mesh->mMeshCount; j++)
        {
          Mesh *m = mesh->mMeshes[j];
          if ( strcmp(m->mName,inst.mMeshName) == 0 )
          {
            float matrix[16];
            fmi_composeTransform(inst.mPosition,inst.mRotation,inst.mScale,matrix);
            float rotate[16];
            fmi_quatToMatrix(inst.mRotation,rotate);

            bool compute_normal = false;

            for (unsigned int k=0; k<m->mSubMeshCount; k++)
            {
              SubMesh *sm = m->mSubMeshes[k];
              Msave ms;
              ms.mMaterialName = sm->mMaterialName;
              for (unsigned int l=0; l<sm->mTriCount; l++)
              {
                unsigned int i1 = sm->mIndices[l*3+0];
                unsigned int i2 = sm->mIndices[l*3+1];
                unsigned int i3 = sm->mIndices[l*3+2];

                MeshVertex v1 = m->mVertices[i1];
                MeshVertex v2 = m->mVertices[i2];
                MeshVertex v3 = m->mVertices[i3];

                fmi_transform(matrix,v1.mPos,v1.mPos);
                fmi_transform(matrix,v2.mPos,v2.mPos);
                fmi_transform(matrix,v3.mPos,v3.mPos);
                fmi_transform(exportTransform,v1.mPos,v1.mPos);
                fmi_transform(exportTransform,v2.mPos,v2.mPos);
                fmi_transform(exportTransform,v3.mPos,v3.mPos);

                if ( l == 0 )
                {
                  if ( v1.mRadius == 1 || v2.mRadius == 1 || v3.mRadius == 1 )
                  {
                    compute_normal = true;
                  }
                  if ( v1.mNormal[0] == 0 && v1.mNormal[1] == 0 && v1.mNormal[2] == 0 ) compute_normal = true;
                }

                if ( compute_normal )
                {

                  v1.mRadius = 1;
                  v2.mRadius = 1;
                  v3.mRadius = 1;

                  float n[3];
                  fmi_computePlane(v3.mPos,v2.mPos,v1.mPos,n);

                  v1.mNormal[0]+=n[0];
                  v1.mNormal[1]+=n[1];
                  v1.mNormal[2]+=n[2];

                  v2.mNormal[0]+=n[0];
                  v2.mNormal[1]+=n[1];
                  v2.mNormal[2]+=n[2];

                  v3.mNormal[0]+=n[0];
                  v3.mNormal[1]+=n[1];
                  v3.mNormal[2]+=n[2];

                }
                else
                {

                  fmi_transform(rotate,v1.mNormal,v1.mNormal);
                  fmi_transform(rotate,v2.mNormal,v2.mNormal);
                  fmi_transform(rotate,v3.mNormal,v3.mNormal);

                  fmi_transformRotate(exportTransform,v1.mNormal,v1.mNormal);
                  fmi_transformRotate(exportTransform,v2.mNormal,v2.mNormal);
                  fmi_transformRotate(exportTransform,v3.mNormal,v3.mNormal);

                }

                i1 = bigPool.GetVertex(v1)+1;
                i2 = bigPool.GetVertex(v2)+1;
                i3 = bigPool.GetVertex(v3)+1;

                ms.mIndices.push_back(i1);
                ms.mIndices.push_back(i2);
                ms.mIndices.push_back(i3);

              }
              meshes.push_back(ms);
            }
            break;
          }
        }
      }

      HeI32 vcount = bigPool.GetVertexCount();

      if ( vcount )
      {
        MeshVertex *vb = bigPool.GetBuffer();
        for (HeI32 i=0; i<vcount; i++)
        {
          const MeshVertex &v = vb[i];
          fi_fprintf(fph,"v %s %s %s\r\n", FloatString(v.mPos[0]), FloatString(v.mPos[1]), FloatString(v.mPos[2]));
        }
        for (HeI32 i=0; i<vcount; i++)
        {
          const MeshVertex &v = vb[i];
          fi_fprintf(fph,"vt %s %s\r\n", FloatString(v.mTexel1[0]), FloatString(v.mTexel1[1]));
        }
        for (HeI32 i=0; i<vcount; i++)
        {
          MeshVertex &v = vb[i];
          fmi_normalize(v.mNormal);
          fi_fprintf(fph,"vn %s %s %s\r\n", FloatString(v.mNormal[0]), FloatString(v.mNormal[1]), FloatString(v.mNormal[2]));
        }
        MsaveVector::iterator i;
        for (i=meshes.begin(); i!=meshes.end(); ++i)
        {
          Msave &ms = (*i);
          fi_fprintf(fph,"usemtl %s\r\n", ms.mMaterialName );
          HeU32 tcount = ms.mIndices.size()/3;
          HeU32 *indices = &ms.mIndices[0];
          for (HeU32 k=0; k<tcount; k++)
          {
            HeU32 i1 = indices[k*3+0];
            HeU32 i2 = indices[k*3+1];
            HeU32 i3 = indices[k*3+2];
            fi_fprintf(fph,"f %d/%d/%d %d/%d/%d %d/%d/%d\r\n", i1, i1, i1, i2, i2, i2, i3, i3, i3 );
          }
        }
      }
    }
    else
    {
      MsaveVector meshes;
      VertexPool< MeshVertex > bigPool;

      for (unsigned int j=0; j<mesh->mMeshCount; j++)
      {
        Mesh *mm = mesh->mMeshes[j];
        bool compute_normal = false;
        for (unsigned int k=0; k<mm->mSubMeshCount; k++)
        {
          SubMesh *sm = mm->mSubMeshes[k];
          Msave ms;
          ms.mMaterialName = sm->mMaterialName;
          for (unsigned int l=0; l<sm->mTriCount; l++)
          {
            unsigned int i1 = sm->mIndices[l*3+0];
            unsigned int i2 = sm->mIndices[l*3+1];
            unsigned int i3 = sm->mIndices[l*3+2];

            MeshVertex v1 = mm->mVertices[i1];
            MeshVertex v2 = mm->mVertices[i2];
            MeshVertex v3 = mm->mVertices[i3];

            fmi_transform(exportTransform,v1.mPos,v1.mPos);
            fmi_transform(exportTransform,v2.mPos,v2.mPos);
            fmi_transform(exportTransform,v3.mPos,v3.mPos);

            if ( l == 0 )
            {
              if ( v1.mRadius == 1 || v2.mRadius == 1 || v3.mRadius == 1 )
              {
                compute_normal = true;
              }
              if ( v1.mNormal[0] == 0 && v1.mNormal[1] == 0 && v1.mNormal[2] == 0 ) compute_normal = true;
            }

            if ( compute_normal )
            {

              v1.mRadius = 1;
              v2.mRadius = 1;
              v3.mRadius = 1;

              float n[3];
              fmi_computePlane(v3.mPos,v2.mPos,v1.mPos,n);

              v1.mNormal[0]+=n[0];
              v1.mNormal[1]+=n[1];
              v1.mNormal[2]+=n[2];

              v2.mNormal[0]+=n[0];
              v2.mNormal[1]+=n[1];
              v2.mNormal[2]+=n[2];

              v3.mNormal[0]+=n[0];
              v3.mNormal[1]+=n[1];
              v3.mNormal[2]+=n[2];

            }
            else
            {
              fmi_transformRotate(exportTransform,v1.mNormal,v1.mNormal);
              fmi_transformRotate(exportTransform,v2.mNormal,v2.mNormal);
              fmi_transformRotate(exportTransform,v3.mNormal,v3.mNormal);
            }

            i1 = bigPool.GetVertex(v1)+1;
            i2 = bigPool.GetVertex(v2)+1;
            i3 = bigPool.GetVertex(v3)+1;

            ms.mIndices.push_back(i1);
            ms.mIndices.push_back(i2);
            ms.mIndices.push_back(i3);

          }
          meshes.push_back(ms);
        }
      }
      HeI32 vcount = bigPool.GetVertexCount();

      if ( vcount )
      {
        MeshVertex *vb = bigPool.GetBuffer();
        for (HeI32 i=0; i<vcount; i++)
        {
          const MeshVertex &v = vb[i];
          fi_fprintf(fph,"v %s %s %s\r\n", FloatString(v.mPos[0]), FloatString(v.mPos[1]), FloatString(v.mPos[2]));
        }
        for (HeI32 i=0; i<vcount; i++)
        {
          const MeshVertex &v = vb[i];
          fi_fprintf(fph,"vt %s %s\r\n", FloatString(v.mTexel1[0]), FloatString(v.mTexel1[1]));
        }
        for (HeI32 i=0; i<vcount; i++)
        {
          MeshVertex &v = vb[i];
          fmi_normalize(v.mNormal);
          fi_fprintf(fph,"vn %s %s %s\r\n", FloatString(v.mNormal[0]), FloatString(v.mNormal[1]), FloatString(v.mNormal[2]));
        }
        MsaveVector::iterator i;
        for (i=meshes.begin(); i!=meshes.end(); ++i)
        {
          Msave &ms = (*i);
          fi_fprintf(fph,"usemtl %s\r\n", ms.mMaterialName );
          HeU32 tcount = ms.mIndices.size()/3;
          HeU32 *indices = &ms.mIndices[0];
          for (HeU32 k=0; k<tcount; k++)
          {
            HeU32 i1 = indices[k*3+0];
            HeU32 i2 = indices[k*3+1];
            HeU32 i3 = indices[k*3+2];
            fi_fprintf(fph,"f %d/%d/%d %d/%d/%d %d/%d/%d\r\n", i1, i1, i1, i2, i2, i2, i3, i3, i3 );
          }
        }
      }
    }
  }

  virtual bool serializeMeshSystem(MeshSystem *mesh,MeshSerialize &data)
  {
    bool ret = false;

    FILE_INTERFACE *fph = fi_fopen("foo", "wmem", 0, 0);

    if ( fph )
    {
      if ( data.mFormat == MSF_OGRE3D )
      {
        FILE_INTERFACE *exfph = fi_fopen("foo", "wmem", 0, 0);
        serializeOgre(fph,exfph,mesh,data.mSaveFileName);
        size_t olen;
        void *temp = fi_getMemBuffer(exfph,&olen);
        if ( temp )
        {
          data.mExtendedData = (unsigned char *)MEMALLOC_MALLOC(olen);
          memcpy(data.mExtendedData,temp,olen);
          data.mExtendedLen = olen;
        }
        fi_fclose(exfph);
      }
      else if ( data.mFormat == MSF_EZMESH )
      {
        serializeEzm(fph,mesh);
      }
      else if ( data.mFormat == MSF_WAVEFRONT )
      {
        FILE_INTERFACE *exfph = fi_fopen("foo", "wmem", 0, 0);
        serializeWavefront(fph,exfph,mesh,data.mSaveFileName,data.mExportTransform);
        size_t olen;
        void *temp = fi_getMemBuffer(exfph,&olen);
        if ( temp )
        {
          data.mExtendedData = (unsigned char *)MEMALLOC_MALLOC(olen);
          memcpy(data.mExtendedData,temp,olen);
          data.mExtendedLen = olen;
        }
        fi_fclose(exfph);
      }

      size_t olen;
      void *temp = fi_getMemBuffer(fph,&olen);
      if ( temp )
      {
        data.mBaseData = (unsigned char *)MEMALLOC_MALLOC(olen);
        memcpy(data.mBaseData,temp,olen);
        data.mBaseLen = olen;
        ret = true;
      }
      fi_fclose(fph);
    }

    return ret;
  }

  virtual  void             releaseSerializeMemory(MeshSerialize &data)
  {
    MEMALLOC_FREE(data.mBaseData);
    MEMALLOC_FREE(data.mExtendedData);
    data.mBaseData = 0;
    data.mBaseLen = 0;
    data.mExtendedData = 0;
    data.mExtendedLen = 0;
  }


  virtual const char   *    getFileRequestDialogString(void)
  {
    typedef USER_STL::vector< std::string > StringVector;
    StringVector descriptions;
    StringVector extensions;

    unsigned int count = getImporterCount();
    for (unsigned i=0; i<count; i++)
    {
      MESHIMPORT::MeshImporter *imp = getImporter(i);
      unsigned int ecount = imp->getExtensionCount();
      for (unsigned int j=0; j<ecount; j++)
      {
        const char *description = imp->getDescription(j);
        const char *itype = imp->getExtension(j);
        std::string desc = description;
        std::string ext = itype;
        descriptions.push_back(desc);
        extensions.push_back(ext);
      }
    }
    mFileRequest.clear();
    mFileRequest+="All (";
    count = descriptions.size();
    for (unsigned int i=0; i<count; i++)
    {
      mFileRequest+="*";
      mFileRequest+=extensions[i];
      if ( i != (count-1) )
        mFileRequest+=";";
    }
    mFileRequest+=")|";
    for (unsigned int i=0; i<count; i++)
    {
      mFileRequest+="*";
      mFileRequest+=extensions[i];
      if ( i != (count-1) )
        mFileRequest+=";";
    }
    mFileRequest+="|";
    for (unsigned int i=0; i<count; i++)
    {
      mFileRequest+=descriptions[i];
      mFileRequest+=" (*";
      mFileRequest+=extensions[i];
      mFileRequest+=")*";
      mFileRequest+="|*";
      mFileRequest+=extensions[i];
      if ( i != (count-1) )
        mFileRequest+="|";
    }
    return mFileRequest.c_str();
  }

  virtual void             setMeshImportApplicationResource(MeshImportApplicationResource *resource)
  {
    mApplicationResource = resource;
  }

  virtual MeshSkeletonInstance *createMeshSkeletonInstance(const MeshSkeleton &sk)
  {
    MeshSkeletonInstance *ret = 0;

    if ( sk.mBoneCount )
    {
      ret = MEMALLOC_NEW(MeshSkeletonInstance);
      ret->mName = sk.mName;
      ret->mBoneCount = sk.mBoneCount;
      ret->mBones = MEMALLOC_NEW_ARRAY(MeshBoneInstance,sk.mBoneCount)[sk.mBoneCount];
      for (int i=0; i<ret->mBoneCount; i++)
      {
        const MeshBone &src   = sk.mBones[i];
        MeshBoneInstance &dst = ret->mBones[i];
        dst.mBoneName = src.mName;
        dst.mParentIndex = src.mParentIndex;
        fmi_composeTransform(src.mPosition,src.mOrientation,src.mScale,dst.mLocalTransform);
        if ( src.mParentIndex != -1 )
        {
          MeshBoneInstance &parent = ret->mBones[src.mParentIndex];
          fmi_multiply(dst.mLocalTransform,parent.mTransform,dst.mTransform); // multiply times the parent matrix.
        }
        else
        {
          memcpy(dst.mTransform,dst.mLocalTransform,sizeof(float)*16);
        }
        dst.composeInverse(); // compose the inverse transform.
      }
    }
    return ret;
  }

  virtual  void  releaseMeshSkeletonInstance(MeshSkeletonInstance *sk)
  {
    if ( sk )
    {
      MEMALLOC_DELETE_ARRAY(MeshBoneInstance,sk->mBones);
      MEMALLOC_DELETE(MeshSkeletonInstance,sk);
    }
  }

  virtual bool  sampleAnimationTrack(int trackIndex,const MeshSystem *msystem,MeshSkeletonInstance *skeleton)
  {
    bool ret = false;

    if ( msystem && skeleton && msystem->mAnimationCount )
    {
      MeshAnimation *anim = msystem->mAnimations[0]; // got the animation.
      for (int i=0; i<skeleton->mBoneCount; i++)
      {
        MeshBoneInstance &b = skeleton->mBones[i];
        // ok..look for this track in the animation...
        float transform[16];

        MeshAnimTrack *track = 0;
        for (int j=0; j<anim->mTrackCount; j++)
        {
          MeshAnimTrack *t = anim->mTracks[j];
          if ( strcmp(t->mName,b.mBoneName) == 0 ) // if the names match
          {
            track = t;
            break;
          }
        }
        if ( track && track->mFrameCount )
        {
          int tindex = trackIndex% track->mFrameCount;
          MeshAnimPose &p = track->mPose[tindex];
          fmi_composeTransform(p.mPos,p.mQuat,p.mScale,transform);
        }
        else
        {
          memcpy(transform,b.mLocalTransform,sizeof(float)*16);
        }
        if ( b.mParentIndex != -1 )
        {
          MeshBoneInstance &parent = skeleton->mBones[b.mParentIndex];
          fmi_multiply(transform,parent.mAnimTransform,b.mAnimTransform); // multiply times the parent matrix.
        }
        else
        {
          memcpy(b.mAnimTransform,transform,sizeof(float)*16);
//          fmi_identity(b.mAnimTransform);
        }
        fmi_multiply(b.mInverseTransform,b.mAnimTransform,b.mCompositeAnimTransform);
      }
      ret = true;
    }

    return ret;
  }

  void transformPoint(const float v[3],float t[3],const float matrix[16])
  {
    float tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
    float ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
    float tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
    t[0] = tx;
    t[1] = ty;
    t[2] = tz;
  }

  void transformVertex(const MeshVertex &src,MeshVertex &dst,MeshSkeletonInstance *skeleton)
  {
    int bone = src.mBone[0];
    float weight = src.mWeight[0];
    assert (bone >= 0 && bone < skeleton->mBoneCount );

    if ( weight > 0 && bone >= 0 && bone < skeleton->mBoneCount )
    {
      float result[3];
      dst.mPos[0] = 0;
      dst.mPos[1] = 0;
      dst.mPos[2] = 0;
      for (int i=0; i<4; i++)
      {
        bone = src.mBone[i];
        weight = src.mWeight[i];
        if ( weight == 0 )
          break;
        transformPoint(src.mPos,result,skeleton->mBones[bone].mCompositeAnimTransform);
        dst.mPos[0]+=result[0]*weight;
        dst.mPos[1]+=result[1]*weight;
        dst.mPos[2]+=result[2]*weight;
      }
    }
    else
    {
      memcpy(&dst,&src,sizeof(MeshVertex));
    }
  }

  virtual void transformVertices(unsigned int vcount,
                                 const MeshVertex *source_vertices,
                                 MeshVertex *dest_vertices,
                                 MeshSkeletonInstance *skeleton)
  {
    for (unsigned int i=0; i<vcount; i++)
    {
      transformVertex(*source_vertices,*dest_vertices,skeleton);
      source_vertices++;
      dest_vertices++;
    }
  }

  virtual void scale(MeshSystemContainer *msc,float s)
  {
    MeshBuilder *b = (MeshBuilder *)msc;
    if ( b )
    {
      b->scale(s);
    }
  }

  virtual MeshImportInterface * getMeshImportInterface(MeshSystemContainer *msc)
  {
    MeshImportInterface *ret = 0;

    if ( msc )
    {
      MeshBuilder *b = (MeshBuilder *)msc;
      ret = static_cast< MeshImportInterface * >(b);
    }
    return ret;
  }

  virtual void gather(MeshSystemContainer *msc)
  {
    if ( msc )
    {
      MeshBuilder *b = (MeshBuilder *)msc;
      b->gather();
    }
  }

private:
  std::string         mCtype;
  std::string         mSemantic;
  std::string         mFileRequest;
  MeshImporterVector  mImporters;
  MeshImportApplicationResource *mApplicationResource;

};

};  // End of Namespace


using namespace MESHIMPORT;


static MyMeshImport *gInterface=0;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
  MeshImport * getInterfaceMeshImport(int version_number,SYSTEM_SERVICES::SystemServices *services)
#else
  MESHIMPORT_API MeshImport * getInterface(int version_number,SYSTEM_SERVICES::SystemServices *services)
#endif
{

  if ( services )
  {
    SYSTEM_SERVICES::gSystemServices = services;
  }

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
