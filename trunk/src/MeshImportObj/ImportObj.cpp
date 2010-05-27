#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

#include "ImportObj.h"
#include "UserMemAlloc.h"
#include "inparser.h"
#include "MeshImport.h"
#include "stringdict.h"
#include "sutil.h"

#pragma warning(disable:4100 4505)

namespace NVSHARE
{

class Vec3
{
public:
  Vec3(void)
  {
    x = y = z = 0;
  }
  Vec3(const NxF32 *f)
  {
    x = f[0];
    y = f[1];
    z = f[2];
  }
  Vec3(NxF32 _x,NxF32 _y,NxF32 _z)
  {
    x = _x;
    y = _y;
    z = _z;
  }

  NxF32 ComputeNormal(const Vec3 &A,
    const Vec3 &B,
    const Vec3 &C)
  {
    NxF32 vx,vy,vz,wx,wy,wz,vw_x,vw_y,vw_z,mag;

    vx = (B.x - C.x);
    vy = (B.y - C.y);
    vz = (B.z - C.z);

    wx = (A.x - B.x);
    wy = (A.y - B.y);
    wz = (A.z - B.z);

    vw_x = vy * wz - vz * wy;
    vw_y = vz * wx - vx * wz;
    vw_z = vx * wy - vy * wx;

    mag = sqrtf((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

    if ( mag < 0.000001f )
    {
      mag = 0;
    }
    else
    {
      mag = 1.0f/mag;
    }

    x = vw_x * mag;
    y = vw_y * mag;
    z = vw_z * mag;

    return mag;
  }


  const NxF32 * Ptr(void) const { return &x; };

  NxF32 x;
  NxF32 y;
  NxF32 z;
};

class Vec2
{
public:
  Vec2(void)
  {
    x = y = 0;
  }
  NxF32 x;
  NxF32 y;
};

typedef std::vector< Vec2 > Vec2Vector;
typedef std::vector< Vec3 > Vec3Vector;

class ObjMaterial
{
public:
  ObjMaterial(const StringRef &name)
  {
    mName = name;
  }


  StringRef mName; //name of the material
  NxF32 Ns;
  Vec3          Ka;
  Vec3          Kd;
  Vec3          Ks;
  StringRef     map_kd; // texture map to use.
};

typedef std::vector< ObjMaterial *> ObjMaterialVector;

// reads file format as output from Qhull
class OBJ : public InPlaceParserInterface, public MeshImporter, public Memalloc
{
public:
  OBJ(void);
  virtual ~OBJ(void);


  const char * getExtension(NxI32 index) { return ".obj"; };
  const char * getDescription(NxI32 index) { return "Wavefront OBJ"; };

  bool importMesh(const char *meshName,const void *data,NxU32 len,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource);

  NxI32 ParseLine(NxI32 lineno,NxI32 argc,const char **argv);  // return TRUE to continue parsing, return FALSE to abort parsing process

  void release(void);

private:

  void sendTri(const MeshVertex &v1,const MeshVertex &v2,const MeshVertex &v3);
  void GetVertex(MeshVertex &v,const char *face,bool minusIndexing=true) const;

  Vec3Vector  mVerts;
  Vec2Vector  mTexels;
  Vec3Vector  mNormals;
  NxF32       mPlane[4];

  StringRef            mCurrentMesh;
  StringRef            mCurrentMat;

  MeshImportInterface *mCallback;
  bool                 mProcessMaterials; // flag to indicate I am processing material data.
  ObjMaterial         *mCurrentMaterial;
  ObjMaterialVector    mMaterials;
  StringDict           mStrings;
};


static char * nextDir(char *start)
{

	while ( *start )
	{
		if ( *start == '\\' ) return start;
		if ( *start == '/' ) return start;
		start++;
	}
	return 0;
}

#pragma warning(disable:4127)
char * skipDir(char *start)
{
	char *slash = start;
	while ( 1 )
	{
	  slash = nextDir(slash);
		if ( slash )
		{
			start = slash+1;
			slash = start;
		}
		else
			break;
	}
  return start;
}

#pragma warning(disable:4100)

bool OBJ::importMesh(const char *fname,const void *_data,NxU32 len,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
{
  bool ret = true;

  release();


  mCurrentMesh = mStrings.Get(fname);

  mCallback = callback;


  if ( appResource ) // if we have the ability to get application loaded resources
  {
    mProcessMaterials = true;
    char scratch[512];
    strcpy(scratch,fname);
    char *dot = strrchr(scratch, '.' );
    if ( dot )
  	  *dot = 0;
    strcat(scratch,".mtl");

    NxU32 len;
    void *mem = appResource->getApplicationResource(fname,scratch,len);
    if ( mem )
    {
      char *data = (char *)MEMALLOC_MALLOC(len);
      memcpy(data,mem,len);

      appResource->releaseApplicationResource(mem);

      InPlaceParser ipp(data,len);
      ipp.Parse(this);

      MEMALLOC_FREE(data);
    }
  }


  char *data = (char *)MEMALLOC_MALLOC(len);
  memcpy(data,_data,len);
  mProcessMaterials = false;
  InPlaceParser ipp(data,len);

  mCallback->importAssetName(fname,0);

  {
    ObjMaterialVector::iterator i;
    for (i=mMaterials.begin(); i!=mMaterials.end(); ++i)
    {
      ObjMaterial *o = (*i);

      char scratch[2048];
      sprintf(scratch,"diffuse=%s,Ns=%s,Ka=(%s,%s,%s),Kd=(%s,%s,%s),Ks=(%s,%s,%s)",
        o->map_kd.Get(),
        FloatString(o->Ns),
        FloatString(o->Ka.x),
        FloatString(o->Ka.y),
        FloatString(o->Ka.z),
        FloatString(o->Kd.x),
        FloatString(o->Kd.y),
        FloatString(o->Kd.z),
        FloatString(o->Ks.x),
        FloatString(o->Ks.y),
        FloatString(o->Ks.z) );

      mCallback->importMaterial( o->mName.Get(), scratch );

    }
  }

  mCallback->importMesh(fname,0);

  ipp.Parse(this);

  MEMALLOC_FREE(data);

  return ret;
}

static const char * GetArg(const char **argv,NxI32 i,NxI32 argc)
{
  const char * ret = 0;
  if ( i < argc ) ret = argv[i];
  return ret;
}

void OBJ::GetVertex(MeshVertex &v,const char *face,bool minusIndexing) const
{
  v.mPos[0] = 0;
  v.mPos[1] = 0;
  v.mPos[2] = 0;

  v.mTexel1[0] = 0;
  v.mTexel1[1] = 0;

  v.mTexel2[0] = 0;
  v.mTexel2[1] = 0;

  v.mNormal[0] = 0;
  v.mNormal[1] = 1;
  v.mNormal[2] = 0;

  NxI32 index = atoi( face );
  if ( minusIndexing )
    index--;

  if ( index < 0 )
    index = mVerts.size()+index+1;

  if ( index >= 0 && index < (NxI32)mVerts.size() )
  {

    const Vec3 &p = mVerts[index];

    v.mPos[0] = p.x;
    v.mPos[1] = p.y;
    v.mPos[2] = p.z;
  }


  const char *texel = strstr(face,"/");

  if ( texel )
  {
    NxI32 tindex = atoi( texel+1) - 1;

    if ( tindex < 0 )
      tindex = mTexels.size()+tindex+1;

    if ( tindex >=0 && tindex < (NxI32)mTexels.size() )
    {
      v.mTexel1[0] = mTexels[tindex].x;
      v.mTexel1[1] = mTexels[tindex].y;

      v.mTexel2[0] = v.mTexel1[0];
      v.mTexel2[1] = v.mTexel1[1];

    }

    const char *normal = strstr(texel+1,"/");
    if ( normal )
    {
      NxI32 nindex = atoi( normal+1 ) - 1;
      if ( nindex < 0 )
        nindex = mNormals.size()+nindex+1;

      if (nindex >= 0 && nindex < (NxI32)mNormals.size() )
      {
        v.mNormal[0] = mNormals[nindex].x;
        v.mNormal[1] = mNormals[nindex].y;
        v.mNormal[2] = mNormals[nindex].z;
      }
    }
  }

}

NxI32 OBJ::ParseLine(NxI32 lineno,NxI32 argc,const char **argv)  // return TRUE to continue parsing, return FALSE to abort parsing process
{
  NxI32 ret = 0;

  if ( argc >= 1 )
  {
    const char *foo = argv[0];
    if ( *foo != '#' )
    {

      if ( mProcessMaterials )
      {
        if ( stricmp(foo,"newmtl") == 0 )
        {
          StringRef matname;
          if ( argc >=2 ) matname = mStrings.Get(argv[1]);
          mCurrentMaterial = new ObjMaterial(matname);
          mMaterials.push_back(mCurrentMaterial);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Ns") == 0 && argc >= 2 )
        {
          mCurrentMaterial->Ns = (NxF32)atof(argv[1]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Ka") == 0 && argc >= 4 )
        {
          mCurrentMaterial->Ka.x = (NxF32)atof(argv[1]);
          mCurrentMaterial->Ka.y = (NxF32)atof(argv[2]);
          mCurrentMaterial->Ka.z = (NxF32)atof(argv[3]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Kd") == 0 && argc >= 4 )
        {
          mCurrentMaterial->Kd.x = (NxF32)atof(argv[1]);
          mCurrentMaterial->Kd.y = (NxF32)atof(argv[2]);
          mCurrentMaterial->Kd.z = (NxF32)atof(argv[3]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Ks") == 0 && argc >= 4 )
        {
          mCurrentMaterial->Ks.x = (NxF32)atof(argv[1]);
          mCurrentMaterial->Ks.y = (NxF32)atof(argv[2]);
          mCurrentMaterial->Ks.z = (NxF32)atof(argv[3]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"map_kd") == 0 && argc >= 2 )
        {
          mCurrentMaterial->map_kd = mStrings.Get( argv[1] );
        }
      }
      else
      {
        if ( strcmp(argv[0],"mtllib") == 0 && argc >= 2 )
        {
          // ok, load this material library...
          const char *fname = argv[1];
          mProcessMaterials = true;
          InPlaceParser ipp(fname,0);
          ipp.Parse(this);
          mProcessMaterials = false;
        }
        else if ( strcmp(argv[0],"usemtl") == 0 && argc>= 2 )
        {
          for (size_t i=0; i<mMaterials.size(); i++)
          {
            ObjMaterial *mat = mMaterials[i];
            if ( strcmp(argv[1],mat->mName) == 0 )
            {
              mCurrentMat = mat->mName;
              break;
            }
          }
        }
        else if ( stricmp(argv[0],"v") == 0 && argc == 4 )
        {
          Vec3 v;
          v.x = (NxF32) atof( argv[1] );
          v.y = (NxF32) atof( argv[2] );
          v.z = (NxF32) atof( argv[3] );

          mVerts.push_back(v);
        }
		else if ( stricmp(argv[0],"p") == 0 && argc == 5 )
		{
			mPlane[0] = (NxF32) atof( argv[1] );
			mPlane[1] = (NxF32) atof( argv[2] );
			mPlane[2] = (NxF32) atof( argv[3] );
			mPlane[3] = (NxF32) atof( argv[4] );
			mCallback->importPlane(mPlane);
		}

        else if ( stricmp(argv[0],"vt") == 0 && argc >= 3 )
        {
          Vec2 t;
          t.x = (NxF32)atof( argv[1] );
          t.y = (NxF32)atof( argv[2] );
					t.x = t.x;
					t.y = t.y;
          mTexels.push_back(t);
        }
        else if ( stricmp(argv[0],"vn") == 0 && argc == 4 )
        {
          Vec3 normal;
          normal.x = (NxF32) atof(argv[1]);
          normal.y = (NxF32) atof(argv[2]);
          normal.z = (NxF32) atof(argv[3]);
          mNormals.push_back(normal);
        }
        else if ( stricmp(argv[0],"t") == 0 && argc == 5 )
        {
          MeshVertex v[4];

          GetVertex(v[0],argv[1],false );
          GetVertex(v[1],argv[2],false );
          GetVertex(v[2],argv[3],false );
          GetVertex(v[3],argv[4],false );
          sendTri(v[2],v[1],v[0]);
          sendTri(v[0],v[1],v[3]);
          sendTri(v[1],v[2],v[3]);
          sendTri(v[2],v[0],v[3]);
        }
        else if ( stricmp(argv[0],"f") == 0 && argc >= 4 )
        {
          MeshVertex v[32];

          NxI32 vcount = argc-1;

          for (NxI32 i=1; i<argc; i++)
          {
            GetVertex(v[i-1],argv[i] );
          }

          // need to generate a normal!
          if ( mNormals.empty() )
          {
            Vec3 p1( v[0].mPos );
            Vec3 p2( v[1].mPos );
            Vec3 p3( v[2].mPos );

            Vec3 n;
            n.ComputeNormal(p3,p2,p1);

            for (NxI32 i=0; i<vcount; i++)
            {
              v[i].mNormal[0] = n.x;
              v[i].mNormal[1] = n.y;
              v[i].mNormal[2] = n.z;
            }

          }

          sendTri(v[0],v[1],v[2]);

          if ( vcount >=3 ) // do the fan
          {
            for (NxI32 i=2; i<(vcount-1); i++)
            {
              sendTri(v[0],v[i],v[i+1]);
            }
          }

        }
      }
    }
  }

  return ret;
}


OBJ::OBJ(void)
{
  mCurrentMaterial = 0;
}

OBJ::~OBJ(void)
{
  release();
}

void OBJ::release(void)
{
  mCurrentMaterial = 0;
  for (size_t i=0; i<mMaterials.size(); i++)
  {
    ObjMaterial *om = mMaterials[i];
    delete om;
  }
  mMaterials.clear();
  mVerts.clear();
  mTexels.clear();
  mNormals.clear();
}


static void set(NxF32 *d,const Vec3 &t)
{
  d[0] = t.x;
  d[1] = t.y;
  d[2] = t.z;
}

void OBJ::sendTri(const MeshVertex &tv1,const MeshVertex &tv2,const MeshVertex &tv3)
{

  MeshVertex v1 = tv1;
  MeshVertex v2 = tv2;
  MeshVertex v3 = tv3;

  Vec3 p1( v1.mPos );
  Vec3 p2( v2.mPos );
  Vec3 p3( v3.mPos );

  if ( mNormals.empty() )
  {
    Vec3 n;
    n.ComputeNormal(p3,p2,p1);

    set(v1.mNormal,n);
    set(v2.mNormal,n);
    set(v3.mNormal,n);
  }

  if ( mTexels.empty() ) // if we don't have any texture co-ordinates then synthesize something
  {
  	#define TSCALE1 (1.0f/4.0f)

  	const NxF32 *tp1 = p1.Ptr();
  	const NxF32 *tp2 = p2.Ptr();
  	const NxF32 *tp3 = p3.Ptr();

  	NxI32 i1 = 0;
	  NxI32 i2 = 0;

  	NxF32 nx = fabsf(v1.mNormal[0]);
  	NxF32 ny = fabsf(v1.mNormal[1]);
  	NxF32 nz = fabsf(v1.mNormal[2]);

  	if ( nx <= ny && nx <= nz )
  		i1 = 0;
  	if ( ny <= nx && ny <= nz )
  		i1 = 1;
  	if ( nz <= nx && nz <= ny )
  		i1 = 2;

  	switch ( i1 )
  	{
	  	case 0:
  			if ( ny < nz )
  				i2 = 1;
  			else
  				i2 = 2;
  			break;
  		case 1:
  			if ( nx < nz )
  				i2 = 0;
  			else
  				i2 = 2;
  			break;
  		case 2:
  			if ( nx < ny )
  				i2 = 0;
  			else
  				i2 = 1;
			break;
  	}

	  v1.mTexel1[0] = tp1[i1]*TSCALE1;
  	v1.mTexel1[1] = tp1[i2]*TSCALE1;

  	v2.mTexel1[0] = tp2[i1]*TSCALE1;
  	v2.mTexel1[1] = tp2[i2]*TSCALE1;

  	v3.mTexel1[0] = tp3[i1]*TSCALE1;
  	v3.mTexel1[1] = tp3[i2]*TSCALE1;

  }

  mCallback->importTriangle(mCurrentMesh.Get(),mCurrentMat.Get(),MIVF_POSITION | MIVF_NORMAL | MIVF_TEXEL1,v1,v2,v3);

}



MeshImporter * createMeshImportObj(void)
{
  OBJ *o = MEMALLOC_NEW(OBJ);
  return static_cast< MeshImporter *>(o);
}

void         releaseMeshImportObj(MeshImporter *iface)
{
  OBJ *o = static_cast< OBJ *>(iface);
  delete o;
}


}; // end of namespace
