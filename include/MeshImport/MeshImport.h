#ifndef MESHIMPORT_H
#define MESHIMPORT_H

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <malloc.h>
#include <math.h>

#pragma warning(push)
#pragma warning(disable:4996)


// MeshImporters to write:  Wavefront OBJ
//                          EZ-Mesh
//                          Ogre3d
//                          Unfloat PSK
//                          Granny
//                          SpeedTree
//                          HeroEngine terrain
//                          HeroEngine water
//                          Leveller heightfields using RTIN
//

class MemoryServices;

namespace MESHIMPORT
{

inline float fmi_computePlane(const float *A,const float *B,const float *C,float *n) // returns D
{
	float vx = (B[0] - C[0]);
	float vy = (B[1] - C[1]);
	float vz = (B[2] - C[2]);

	float wx = (A[0] - B[0]);
	float wy = (A[1] - B[1]);
	float wz = (A[2] - B[2]);

	float vw_x = vy * wz - vz * wy;
	float vw_y = vz * wx - vx * wz;
	float vw_z = vx * wy - vy * wx;

	float mag = sqrt((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

	if ( mag < 0.000001f )
	{
		mag = 0;
	}
	else
	{
		mag = 1.0f/mag;
	}

	float x = vw_x * mag;
	float y = vw_y * mag;
	float z = vw_z * mag;


	float D = 0.0f - ((x*A[0])+(y*A[1])+(z*A[2]));

  n[0] = x;
  n[1] = y;
  n[2] = z;

	return D;
}

inline void  fmi_transform(const float matrix[16],const float v[3],float t[3]) // rotate and translate this point
{
  if ( matrix )
  {
    float tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
    float ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
    float tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
    t[0] = tx;
    t[1] = ty;
    t[2] = tz;
  }
  else
  {
    t[0] = v[0];
    t[1] = v[1];
    t[2] = v[2];
  }
}

inline void  fmi_transformRotate(const float matrix[16],const float v[3],float t[3]) // rotate only
{
  if ( matrix )
  {
    float tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]);
    float ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]);
    float tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]);
    t[0] = tx;
    t[1] = ty;
    t[2] = tz;
  }
  else
  {
    t[0] = v[0];
    t[1] = v[1];
    t[2] = v[2];
  }
}

inline float fmi_normalize(float *n) // normalize this vector
{
  float dist = (float)sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
  if ( dist > 0.0000001f )
  {
    float mag = 1.0f / dist;
    n[0]*=mag;
    n[1]*=mag;
    n[2]*=mag;
  }
  else
  {
    n[0] = 1;
    n[1] = 0;
    n[2] = 0;
  }

  return dist;
}


inline void fmi_quatToMatrix(const float *quat,float *matrix) // convert quaterinion rotation to matrix, zeros out the translation component.
{

  float xx = quat[0]*quat[0];
  float yy = quat[1]*quat[1];
  float zz = quat[2]*quat[2];
  float xy = quat[0]*quat[1];
  float xz = quat[0]*quat[2];
  float yz = quat[1]*quat[2];
  float wx = quat[3]*quat[0];
  float wy = quat[3]*quat[1];
  float wz = quat[3]*quat[2];

  matrix[0*4+0] = 1 - 2 * ( yy + zz );
  matrix[1*4+0] =     2 * ( xy - wz );
  matrix[2*4+0] =     2 * ( xz + wy );

  matrix[0*4+1] =     2 * ( xy + wz );
  matrix[1*4+1] = 1 - 2 * ( xx + zz );
  matrix[2*4+1] =     2 * ( yz - wx );

  matrix[0*4+2] =     2 * ( xz - wy );
  matrix[1*4+2] =     2 * ( yz + wx );
  matrix[2*4+2] = 1 - 2 * ( xx + yy );

  matrix[3*4+0] = matrix[3*4+1] = matrix[3*4+2] = (float) 0.0f;
  matrix[0*4+3] = matrix[1*4+3] = matrix[2*4+3] = (float) 0.0f;
  matrix[3*4+3] =(float) 1.0f;

}



// minimal support math routines
// *** Support math routines
inline void fmi_getAngleAxis(float &angle,float *axis,const float *quat)
{
  //return axis and angle of rotation of quaternion
  float x = quat[0];
  float y = quat[1];
  float z = quat[2];
  float w = quat[3];

  angle = acosf(w) * 2.0f;		//this is getAngle()
  float sa = sqrtf(1.0f - w*w);
  if (sa)
  {
    axis[0] = x/sa;
    axis[1] = y/sa;
    axis[2] = z/sa;
  }
  else
  {
    axis[0] = 1;
    axis[1] = 0;
    axis[2] = 0;
  }
}

inline void fmi_setOrientationFromAxisAngle(const float axis[3],float angle,float *quat)
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

  quat[0] = x;
  quat[1] = y;
  quat[2] = z;
  quat[3] = w;
}


inline void fmi_identity(float *matrix)
{
  matrix[0*4+0] = 1;    matrix[1*4+1] = 1;    matrix[2*4+2] = 1;    matrix[3*4+3] = 1;
  matrix[1*4+0] = 0;    matrix[2*4+0] = 0;    matrix[3*4+0] = 0;
  matrix[0*4+1] = 0;    matrix[2*4+1] = 0;    matrix[3*4+1] = 0;
  matrix[0*4+2] = 0;    matrix[1*4+2] = 0;    matrix[3*4+2] = 0;
  matrix[0*4+3] = 0;    matrix[1*4+3] = 0;    matrix[2*4+3] = 0;
}


inline void fmi_fromQuat(float *matrix,const float quat[4])
{
  fmi_identity(matrix);

  float xx = quat[0]*quat[0];
  float yy = quat[1]*quat[1];
  float zz = quat[2]*quat[2];
  float xy = quat[0]*quat[1];
  float xz = quat[0]*quat[2];
  float yz = quat[1]*quat[2];
  float wx = quat[3]*quat[0];
  float wy = quat[3]*quat[1];
  float wz = quat[3]*quat[2];

  matrix[0*4+0] = 1 - 2 * ( yy + zz );
  matrix[1*4+0] =     2 * ( xy - wz );
  matrix[2*4+0] =     2 * ( xz + wy );

  matrix[0*4+1] =     2 * ( xy + wz );
  matrix[1*4+1] = 1 - 2 * ( xx + zz );
  matrix[2*4+1] =     2 * ( yz - wx );

  matrix[0*4+2] =     2 * ( xz - wy );
  matrix[1*4+2] =     2 * ( yz + wx );
  matrix[2*4+2] = 1 - 2 * ( xx + yy );

  matrix[3*4+0] = matrix[3*4+1] = matrix[3*4+2] = (float) 0.0f;
  matrix[0*4+3] = matrix[1*4+3] = matrix[2*4+3] = (float) 0.0f;
  matrix[3*4+3] =(float) 1.0f;


}

inline void fmi_matrixToQuat(const float *matrix,float *quat) // convert the 3x3 portion of a 4x4 matrix into a quaterion as x,y,z,w
{

  float tr = matrix[0*4+0] + matrix[1*4+1] + matrix[2*4+2];

  // check the diagonal

  if (tr > 0.0f )
  {
    float s = sqrtf((tr + 1.0f) );
    quat[3] = s * 0.5f;
    s = 0.5f / s;
    quat[0] = (matrix[1*4+2] - matrix[2*4+1]) * s;
    quat[1] = (matrix[2*4+0] - matrix[0*4+2]) * s;
    quat[2] = (matrix[0*4+1] - matrix[1*4+0]) * s;

  }
  else
  {
    // diagonal is negative
    int nxt[3] = {1, 2, 0};
    float  qa[4];

    int i = 0;

    if (matrix[1*4+1] > matrix[0*4+0]) i = 1;
    if (matrix[2*4+2] > matrix[i*4+i]) i = 2;

    int j = nxt[i];
    int k = nxt[j];

    float s = sqrtf( ((matrix[i*4+i] - (matrix[j*4+j] + matrix[k*4+k])) + 1.0f) );

    qa[i] = s * 0.5f;

    if (s != 0.0f ) s = 0.5f / s;

    qa[3] = (matrix[j*4+k] - matrix[k*4+j]) * s;
    qa[j] = (matrix[i*4+j] + matrix[j*4+i]) * s;
    qa[k] = (matrix[i*4+k] + matrix[k*4+i]) * s;

    quat[0] = qa[0];
    quat[1] = qa[1];
    quat[2] = qa[2];
    quat[3] = qa[3];
  }


}


inline float fmi_squared(float x) { return x*x; };

inline void fmi_decomposeTransform(const float local_transform[16],float trans[3],float rot[4],float scale[3])
{

  trans[0] = local_transform[12];
  trans[1] = local_transform[13];
  trans[2] = local_transform[14];

  scale[0] = sqrtf(fmi_squared(local_transform[0*4+0]) + fmi_squared(local_transform[0*4+1]) + fmi_squared(local_transform[0*4+2]));
  scale[1] = sqrtf(fmi_squared(local_transform[1*4+0]) + fmi_squared(local_transform[1*4+1]) + fmi_squared(local_transform[1*4+2]));
  scale[2] = sqrtf(fmi_squared(local_transform[2*4+0]) + fmi_squared(local_transform[2*4+1]) + fmi_squared(local_transform[2*4+2]));

  float m[16];
  memcpy(m,local_transform,sizeof(float)*16);

  float sx = 1.0f / scale[0];
  float sy = 1.0f / scale[1];
  float sz = 1.0f / scale[2];

  m[0*4+0]*=sx;
  m[0*4+1]*=sx;
  m[0*4+2]*=sx;

  m[1*4+0]*=sy;
  m[1*4+1]*=sy;
  m[1*4+2]*=sy;

  m[2*4+0]*=sz;
  m[2*4+1]*=sz;
  m[2*4+2]*=sz;

  fmi_matrixToQuat(m,rot);

}

inline void fmi_fromScale(float *matrix,const float scale[3])
{
  fmi_identity(matrix);
  matrix[0*4+0] = scale[0];
  matrix[1*4+1] = scale[1];
  matrix[2*4+2] = scale[2];

}

inline void  fmi_multiply(const float *pA,const float *pB,float *pM)
{

  float a = pA[0*4+0] * pB[0*4+0] + pA[0*4+1] * pB[1*4+0] + pA[0*4+2] * pB[2*4+0] + pA[0*4+3] * pB[3*4+0];
  float b = pA[0*4+0] * pB[0*4+1] + pA[0*4+1] * pB[1*4+1] + pA[0*4+2] * pB[2*4+1] + pA[0*4+3] * pB[3*4+1];
  float c = pA[0*4+0] * pB[0*4+2] + pA[0*4+1] * pB[1*4+2] + pA[0*4+2] * pB[2*4+2] + pA[0*4+3] * pB[3*4+2];
  float d = pA[0*4+0] * pB[0*4+3] + pA[0*4+1] * pB[1*4+3] + pA[0*4+2] * pB[2*4+3] + pA[0*4+3] * pB[3*4+3];

  float e = pA[1*4+0] * pB[0*4+0] + pA[1*4+1] * pB[1*4+0] + pA[1*4+2] * pB[2*4+0] + pA[1*4+3] * pB[3*4+0];
  float f = pA[1*4+0] * pB[0*4+1] + pA[1*4+1] * pB[1*4+1] + pA[1*4+2] * pB[2*4+1] + pA[1*4+3] * pB[3*4+1];
  float g = pA[1*4+0] * pB[0*4+2] + pA[1*4+1] * pB[1*4+2] + pA[1*4+2] * pB[2*4+2] + pA[1*4+3] * pB[3*4+2];
  float h = pA[1*4+0] * pB[0*4+3] + pA[1*4+1] * pB[1*4+3] + pA[1*4+2] * pB[2*4+3] + pA[1*4+3] * pB[3*4+3];

  float i = pA[2*4+0] * pB[0*4+0] + pA[2*4+1] * pB[1*4+0] + pA[2*4+2] * pB[2*4+0] + pA[2*4+3] * pB[3*4+0];
  float j = pA[2*4+0] * pB[0*4+1] + pA[2*4+1] * pB[1*4+1] + pA[2*4+2] * pB[2*4+1] + pA[2*4+3] * pB[3*4+1];
  float k = pA[2*4+0] * pB[0*4+2] + pA[2*4+1] * pB[1*4+2] + pA[2*4+2] * pB[2*4+2] + pA[2*4+3] * pB[3*4+2];
  float l = pA[2*4+0] * pB[0*4+3] + pA[2*4+1] * pB[1*4+3] + pA[2*4+2] * pB[2*4+3] + pA[2*4+3] * pB[3*4+3];

  float m = pA[3*4+0] * pB[0*4+0] + pA[3*4+1] * pB[1*4+0] + pA[3*4+2] * pB[2*4+0] + pA[3*4+3] * pB[3*4+0];
  float n = pA[3*4+0] * pB[0*4+1] + pA[3*4+1] * pB[1*4+1] + pA[3*4+2] * pB[2*4+1] + pA[3*4+3] * pB[3*4+1];
  float o = pA[3*4+0] * pB[0*4+2] + pA[3*4+1] * pB[1*4+2] + pA[3*4+2] * pB[2*4+2] + pA[3*4+3] * pB[3*4+2];
  float p = pA[3*4+0] * pB[0*4+3] + pA[3*4+1] * pB[1*4+3] + pA[3*4+2] * pB[2*4+3] + pA[3*4+3] * pB[3*4+3];

  pM[0] = a;  pM[1] = b;  pM[2] = c;  pM[3] = d;

  pM[4] = e;  pM[5] = f;  pM[6] = g;  pM[7] = h;

  pM[8] = i;  pM[9] = j;  pM[10] = k;  pM[11] = l;

  pM[12] = m;  pM[13] = n;  pM[14] = o;  pM[15] = p;
}


inline void fmi_setTranslation(float *matrix,const float pos[3])
{
  matrix[12] = pos[0];  matrix[13] = pos[1];  matrix[14] = pos[2];
}


// compose this transform
inline void fmi_composeTransform(const float pos[3],const float quat[4],const float scale[3],float matrix[16])
{
  float mscale[16];
  float mrot[16];
  fmi_fromQuat(mrot,quat);
  fmi_fromScale(mscale,scale);
  fmi_multiply(mscale,mrot,matrix);
  fmi_setTranslation(matrix,pos);
}

inline float fmi_dot(const float *p1,const float *p2)
{
  return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2];
}

inline void fmi_cross(float *cross,const float *a,const float *b)
{
  cross[0] = a[1]*b[2] - a[2]*b[1];
  cross[1] = a[2]*b[0] - a[0]*b[2];
  cross[2] = a[0]*b[1] - a[1]*b[0];
}


inline float fmi_getDeterminant(const float matrix[16])
{
  float tempv[3];
  float p0[3];
  float p1[3];
  float p2[3];

  p0[0] = matrix[0*4+0];
  p0[1] = matrix[0*4+1];
  p0[2] = matrix[0*4+2];

  p1[0] = matrix[1*4+0];
  p1[1] = matrix[1*4+1];
  p1[2] = matrix[1*4+2];

  p2[0] = matrix[2*4+0];
  p2[1] = matrix[2*4+1];
  p2[2] = matrix[2*4+2];

  fmi_cross(tempv,p1,p2);

  return fmi_dot(p0,tempv);

}

inline void fmi_getSubMatrix(int ki,int kj,float pDst[16],const float matrix[16])
{
  int row, col;
  int dstCol = 0, dstRow = 0;

  for ( col = 0; col < 4; col++ )
  {
    if ( col == kj )
    {
      continue;
    }
    for ( dstRow = 0, row = 0; row < 4; row++ )
    {
      if ( row == ki )
      {
        continue;
      }
      pDst[dstCol*4+dstRow] = matrix[col*4+row];
      dstRow++;
    }
    dstCol++;
  }
}

inline void fmi_inverseTransform(const float matrix[16],float inverse_matrix[16])
{
  float determinant = fmi_getDeterminant(matrix);
  determinant = 1.0f / determinant;
  for (int i = 0; i < 4; i++ )
  {
    for (int j = 0; j < 4; j++ )
    {
      int sign = 1 - ( ( i + j ) % 2 ) * 2;
      float subMat[16];
      fmi_identity(subMat);
      fmi_getSubMatrix( i, j, subMat, matrix );
      float subDeterminant = fmi_getDeterminant(subMat);
      inverse_matrix[i*4+j] = ( subDeterminant * sign ) * determinant;
    }
  }
}



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
  MIVF_RADIUS         = (1<<10),
	MIVF_ALL = (MIVF_POSITION | MIVF_NORMAL | MIVF_COLOR | MIVF_TEXEL1 | MIVF_TEXEL2 | MIVF_TEXEL3 | MIVF_TEXEL4 | MIVF_TANGENT | MIVF_BINORMAL | MIVF_BONE_WEIGHTING | MIVF_RADIUS)
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
    mRadius = 0; // use for cloth simulations
  }

  bool  operator==(const MeshVertex &v) const
  {
    bool ret = false;

    if ( memcmp(this,&v,sizeof(MeshVertex)) == 0 ) ret = true;

    return ret;
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
  float          mRadius;
};

class MeshBone
{
public:
	MeshBone(void)
	{
		mParentIndex = -1;
    mName = "";
		Identity();
	}

  void Set(const char *name,int parentIndex,const float pos[3],const float rot[4],const float scale[3])
  {
    mName = name;
    mParentIndex = parentIndex;
    mPosition[0] = pos[0];
    mPosition[1] = pos[1];
    mPosition[2] = pos[2];
    mOrientation[0] = rot[0];
    mOrientation[1] = rot[1];
    mOrientation[2] = rot[2];
    mOrientation[3] = rot[3];
    mScale[0] = scale[0];
    mScale[1] = scale[1];
    mScale[2] = scale[2];
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

    mScale[0] = 1;
    mScale[1] = 1;
    mScale[2] = 1;

	}

	void SetName(const char *name)
	{
    mName = name;
	}

	const char * GetName(void) const { return mName; };

	int GetParentIndex(void) const { return mParentIndex; };

	const float * GetPosition(void) const { return mPosition; };
	const float * GetOrientation(void) const { return mOrientation; };
  const float * GetScale(void) const { return mScale; };

  void getAngleAxis(float &angle,float *axis) const
  {
    fmi_getAngleAxis(angle,axis,mOrientation);
  }

  void setOrientationFromAxisAngle(const float axis[3],float angle)
  {
    fmi_setOrientationFromAxisAngle(axis,angle,mOrientation);
  }

	const char   *mName;
	int           mParentIndex;          // array index of parent bone
	float         mPosition[3];
	float         mOrientation[4];
  float         mScale[3];
};

class MeshEntry
{
public:
  MeshEntry(void)
  {
    mName = "";
    mBone = 0;
  }
  const char *mName;
	int         mBone;         // bone this mesh is associcated
};

class MeshSkeleton
{
public:
	MeshSkeleton(void)
	{
    mName = "";
		mBoneCount = 0;
		mBones = 0;
	}

	void SetName(const char *name)
	{
    mName = name;
	}

	void SetBones(int bcount,MeshBone *bones) // memory ownership changes hands here!!!!!!!!!!
	{
		mBoneCount = bcount;
		mBones     = bones;
	}

	int GetBoneCount(void) const { return mBoneCount; };

	const MeshBone& GetBone(int index) const { return mBones[index]; };

	MeshBone * GetBonePtr(int index) const { return &mBones[index]; };

	void SetBone(int index,const MeshBone &b) { mBones[index] = b; };

	const char * GetName(void) const { return mName; };

	const char     *mName;
	int             mBoneCount;
	MeshBone       *mBones;
};


class MeshAnimPose
{
public:
  MeshAnimPose(void)
  {
    mPos[0] = 0;
    mPos[1] = 0;
    mPos[2] = 0;
    mQuat[0] = 0;
    mQuat[1] = 0;
    mQuat[2] = 0;
    mQuat[3] = 0;
    mScale[0] = 1;
    mScale[1] = 1;
    mScale[2] = 1;
  }

	void SetPose(const float *pos,const float *quat,const float *scale)
	{
		mPos[0] = pos[0];
		mPos[1] = pos[1];
		mPos[2] = pos[2];
		mQuat[0] = quat[0];
		mQuat[1] = quat[1];
		mQuat[2] = quat[2];
		mQuat[3] = quat[3];
    mScale[0] = scale[0];
    mScale[1] = scale[1];
    mScale[2] = scale[2];
	};

	void Sample(float *pos,float *quat,float *scale) const
	{
		pos[0] = mPos[0];
		pos[1] = mPos[1];
		pos[2] = mPos[2];
		quat[0] = mQuat[0];
		quat[1] = mQuat[1];
		quat[2] = mQuat[2];
		quat[3] = mQuat[3];
    scale[0] = mScale[0];
    scale[1] = mScale[1];
    scale[2] = mScale[2];
	}

  void getAngleAxis(float &angle,float *axis) const
  {
    fmi_getAngleAxis(angle,axis,mQuat);
  }

	float mPos[3];
	float mQuat[4];
  float mScale[3];
};

class MeshAnimTrack
{
public:
  MeshAnimTrack(void)
  {
    mName = "";
    mFrameCount = 0;
    mDuration = 0;
    mDtime = 0;
    mPose = 0;
  }

	void SetName(const char *name)
	{
    mName = name;
	}

	void SetPose(int frame,const float *pos,const float *quat,const float *scale)
	{
		if ( frame >= 0 && frame < mFrameCount )
			mPose[frame].SetPose(pos,quat,scale);
	}

	const char * GetName(void) const { return mName; };

	void SampleAnimation(int frame,float *pos,float *quat,float *scale) const
	{
		mPose[frame].Sample(pos,quat,scale);
	}

	int GetFrameCount(void) const { return mFrameCount; };

	MeshAnimPose * GetPose(int index) { return &mPose[index]; };

	const char *mName;
	int       mFrameCount;
	float     mDuration;
	float     mDtime;
	MeshAnimPose *mPose;
};

class MeshAnimation
{
public:
  MeshAnimation(void)
  {
    mName = "";
    mTrackCount = 0;
    mFrameCount = 0;
    mDuration = 0;
    mDtime = 0;
    mTracks = 0;
  }


	void SetName(const char *name)
	{
    mName = name;
	}

	void SetTrackName(int track,const char *name)
	{
		mTracks[track]->SetName(name);
	}

	void SetTrackPose(int track,int frame,const float *pos,const float *quat,const float *scale)
	{
		mTracks[track]->SetPose(frame,pos,quat,scale);
	}

	const char * GetName(void) const { return mName; };

	const MeshAnimTrack * LocateTrack(const char *name) const
	{
		const MeshAnimTrack *ret = 0;
		for (int i=0; i<mTrackCount; i++)
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

	int GetFrameIndex(float t) const
	{
		t = fmodf( t, mDuration );
		int index = int(t / mDtime);
		return index;
	}

	int GetTrackCount(void) const { return mTrackCount; };
	float GetDuration(void) const { return mDuration; };

	MeshAnimTrack * GetTrack(int index)
	{
		MeshAnimTrack *ret = 0;
		if ( index >= 0 && index < mTrackCount )
		{
			ret = mTracks[index];
		}
		return ret;
	};

	int GetFrameCount(void) const { return mFrameCount; };
	float GetDtime(void) const { return mDtime; };

  const char *mName;
	int         mTrackCount;
	int         mFrameCount;
	float       mDuration;
	float       mDtime;
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

  void include(const float pos[3])
  {
    if ( pos[0] < mMin[0] ) mMin[0] = pos[0];
    if ( pos[1] < mMin[1] ) mMin[1] = pos[1];
    if ( pos[2] < mMin[2] ) mMin[2] = pos[2];
    if ( pos[0] > mMax[0] ) mMax[0] = pos[0];
    if ( pos[1] > mMax[1] ) mMax[1] = pos[1];
    if ( pos[2] > mMax[2] ) mMax[2] = pos[2];
  }
  float mMin[3];
  float mMax[3];
};

class SubMesh
{
public:
  SubMesh(void)
  {
    mMaterialName = 0;
    mMaterial     = 0;
    mVertexFlags  = 0;
    mTriCount     = 0;
    mIndices      = 0;
  }

  const char          *mMaterialName;
  MeshMaterial        *mMaterial;
  MeshAABB             mAABB;
  unsigned int         mVertexFlags; // defines which vertex components are active.
  unsigned int         mTriCount;    // number of triangles.
  unsigned int        *mIndices;     // indexed triange list
};

class Mesh
{
public:
  Mesh(void)
  {
    mName         = 0;
    mSkeletonName = 0;
    mSkeleton     = 0;
    mSubMeshCount = 0;
    mSubMeshes    = 0;
    mVertexFlags  = 0;
    mVertexCount  = 0;
    mVertices     = 0;
  }
  const char         *mName;
  const char         *mSkeletonName;
  MeshSkeleton       *mSkeleton; // the skeleton used by this mesh system.
  MeshAABB            mAABB;
  unsigned int        mSubMeshCount;
  SubMesh           **mSubMeshes;

  unsigned int       mVertexFlags;  // combined vertex usage flags for all sub-meshes
  unsigned int       mVertexCount;
  MeshVertex        *mVertices;

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
  unsigned char *mData;
  unsigned int   mWidth;
  unsigned int   mHeight;
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
  float        mPosition[3];
  float        mRotation[4]; //quaternion XYZW
  float        mScale[3];
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
  unsigned int   mUserLen;
  unsigned char *mUserData;
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
  unsigned int mTetraCount; // number of tetrahedrons
  float       *mTetraData;
};

#define MESH_SYSTEM_VERSION 1 // version number of this data structure, used for binary serialization

enum MeshCollisionType
{
  MCT_BOX,
  MCT_SPHERE,
  MCT_CAPSULE,
  MCT_CONVEX,
  MCT_LAST
};

class MeshCollision
{
public:
  MeshCollision(void)
  {
    mType = MCT_LAST;
    mName = 0;
    fmi_identity(mTransform);
  }

  MeshCollisionType getType(void) const { return mType; };

  MeshCollisionType mType;
  const char       *mName;  // the bone this collision geometry is associated with.
  float             mTransform[16];   // local transform.
};

class MeshCollisionBox : public MeshCollision
{
public:
  MeshCollisionBox(void)
  {
    mType = MCT_BOX;
    mSides[0] = mSides[1] = mSides[2] = 1;
  }
  float mSides[3];
};

class MeshCollisionSphere : public MeshCollision
{
public:
  MeshCollisionSphere(void)
  {
    mType = MCT_SPHERE;
    mRadius = 1;
  }
  float mRadius;
};

class MeshCollisionCapsule : public MeshCollision
{
public:
  MeshCollisionCapsule(void)
  {
    mType = MCT_CAPSULE;
    mRadius = 1;
    mHeight = 1;
  }
  float  mRadius;
  float  mHeight;
};

class MeshConvex
{
public:
  MeshConvex(void)
  {
    mVertexCount = 0;
    mVertices = 0;
    mTriCount = 0;
    mIndices = 0;
  }
  unsigned int  mVertexCount;
  float        *mVertices;
  unsigned int  mTriCount;
  unsigned int *mIndices;
};

class MeshCollisionConvex : public MeshCollision, public MeshConvex
{
public:
  MeshCollisionConvex(void)
  {
    mType = MCT_CONVEX;
  }


};

class MeshCollisionRepresentation
{
public:
  MeshCollisionRepresentation(void)
  {
    mName = 0;
    mInfo = 0;
    mCollisionCount = 0;
    mCollisionGeometry = 0;
  }
  const char     *mName;
  const char     *mInfo;
  unsigned int    mCollisionCount;
  MeshCollision **mCollisionGeometry;
};

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
    mMeshCollisionCount  = 0;
    mMeshCollisionRepresentations = 0;
  }


  const char           *mAssetName;
  const char           *mAssetInfo;
  int                   mMeshSystemVersion;
  int                   mAssetVersion;
  MeshAABB              mAABB;
  unsigned int          mTextureCount;          // Are textures necessary? [rgd].
  MeshRawTexture      **mTextures;              // Texture storage in mesh data is rare, and the name is simply an attribute of the material

  unsigned int          mTetraMeshCount;        // number of tetrahedral meshes
  MeshTetra           **mTetraMeshes;           // tetraheadral meshes

  unsigned int          mSkeletonCount;         // number of skeletons
  MeshSkeleton        **mSkeletons;             // the skeletons.

  unsigned int          mAnimationCount;
  MeshAnimation       **mAnimations;

  unsigned int          mMaterialCount;         // Materials are owned by this list, merely referenced later.
  MeshMaterial         *mMaterials;

  unsigned int          mUserDataCount;
  MeshUserData        **mUserData;

  unsigned int          mUserBinaryDataCount;
  MeshUserBinaryData  **mUserBinaryData;

  unsigned int          mMeshCount;
  Mesh                **mMeshes;

  unsigned int          mMeshInstanceCount;
  MeshInstance         *mMeshInstances;

  unsigned int          mMeshCollisionCount;
  MeshCollisionRepresentation **mMeshCollisionRepresentations;

};


class MeshImportInterface
{
public:
  virtual void        importMaterial(const char *matName,const char *metaData) = 0;        // one material
  virtual void        importUserData(const char *userKey,const char *userValue) = 0;       // carry along raw user data as ASCII strings only..
  virtual void        importUserBinaryData(const char *name,unsigned int len,const unsigned char *data) = 0;
  virtual void        importTetraMesh(const char *tetraName,const char *meshName,unsigned int tcount,const float *tetraData) = 0;

  virtual void        importAssetName(const char *assetName,const char *info) = 0;         // name of the overall asset.
  virtual void        importMesh(const char *meshName,const char *skeletonName) = 0;       // name of a mesh and the skeleton it refers to.

  virtual void        importTriangle(const char *meshName,
                                     const char *materialName,
                                     unsigned int vertexFlags,
                                     const MeshVertex &v1,
                                     const MeshVertex &v2,
                                     const MeshVertex &v3) = 0;

  virtual void        importIndexedTriangleList(const char *meshName,
                                                const char *materialName,
                                                unsigned int vertexFlags,
                                                unsigned int vcount,
                                                const MeshVertex *vertices,
                                                unsigned int tcount,
                                                const unsigned int *indices) = 0;

  virtual void        importAnimation(const MeshAnimation &animation) = 0;
  virtual void        importSkeleton(const MeshSkeleton &skeleton) = 0;
  virtual void        importRawTexture(const char *textureName,const unsigned char *pixels,unsigned int wid,unsigned int hit) = 0;
  virtual void        importMeshInstance(const char *meshName,const float pos[3],const float rotation[4],const float scale[3])= 0;

  virtual void importCollisionRepresentation(const char *name,const char *info) = 0; // the name of a new collision representation.

  virtual void importConvexHull(const char *collision_rep,    // the collision representation it is associated with
                                const char *boneName,         // the name of the bone it is associated with in a skeleton.
                                const float *transform,       // the full 4x4 transform for this hull, null if in world space.
                                unsigned int vertex_count,
                                const float *vertices,
                                unsigned int tri_count,
                                const unsigned int *indices) = 0;

  virtual int getSerializeFrame(void) = 0;


};

// allows the application to load external resources.
// For example, when loading wavefront OBJ files, the materials are saved in a seperate file.
// This interface allows the application to load the resource.
class MeshImportApplicationResource
{
public:
  virtual void * getApplicationResource(const char *base_name,const char *resource_name,unsigned int &len) = 0;
  virtual void   releaseApplicationResource(void *mem) = 0;
};



class MeshImporter
{
public:
  virtual int              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.
  virtual const char *     getExtension(int index=0) = 0; // report the default file name extension for this mesh type.
  virtual const char *     getDescription(int index=0) = 0; // report the ascii description of the import type.

  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource) = 0;

};

enum MeshSerializeFormat
{
  MSF_EZMESH, // save it back out into ez-mesh, lossless XML format.
  MSF_OGRE3D, // save it back out into the Ogre3d XML format.
  MSF_WAVEFRONT, // save as wavefront OBJ
  MSF_LAST
};


class MeshBoneInstance
{
public:
  MeshBoneInstance(void)
  {
    mBoneName = "";
  }

  void composeInverse(void)
  {
    fmi_inverseTransform(mTransform,mInverseTransform);
  }

  const char *mBoneName;                     // the name of the bone
  int         mParentIndex;                  // the parent index
  float       mLocalTransform[16];
  float       mTransform[16];                // the transform in world space
  float       mAnimTransform[16];            // the sampled animation transform, multiplied times the inverse root transform.
  float       mCompositeAnimTransform[16];   // teh composite transform
  float       mInverseTransform[16];         // the inverse transform
};

class MeshSkeletonInstance
{
public:
  MeshSkeletonInstance(void)
  {
    mName      = "";
    mBoneCount = 0;
    mBones     = 0;
  }

  const char        *mName;
  int                mBoneCount;
  MeshBoneInstance  *mBones;
};

class MeshSerialize
{
public:
  MeshSerialize(MeshSerializeFormat format)
  {
    mFormat = format;
    mBaseData = 0;
    mBaseLen  = 0;
    mExtendedData = 0;
    mExtendedLen = 0;
    mSaveFileName = 0;
    fmi_identity(mExportTransform);
  }
  MeshSerializeFormat mFormat;
  unsigned char      *mBaseData;
  unsigned int        mBaseLen;
  unsigned char      *mExtendedData;
  unsigned int        mExtendedLen;
  const char         *mSaveFileName; // need to know the name of the save file name for OBJ and Ogre3d format.
  float               mExportTransform[16]; // matrix transform on export
};


class MeshSystemContainer;

class MeshImport
{
public:
  virtual void             addImporter(MeshImporter *importer) = 0; // add an additional importer

  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options) = 0;

  virtual MeshSystemContainer *     createMeshSystemContainer(void) = 0;

  virtual MeshSystemContainer *     createMeshSystemContainer(const char *meshName,
                                                              const void *data,
                                                              unsigned int dlen,
                                                              const char *options) = 0; // imports and converts to a single MeshSystem data structure

  virtual void             releaseMeshSystemContainer(MeshSystemContainer *mesh) = 0;

  virtual MeshSystem *     getMeshSystem(MeshSystemContainer *mb) = 0;

  virtual bool             serializeMeshSystem(MeshSystem *mesh,MeshSerialize &data) = 0;
  virtual void             releaseSerializeMemory(MeshSerialize &data) = 0;


  virtual int              getImporterCount(void) = 0;
  virtual MeshImporter    *getImporter(int index) = 0;

  virtual MeshImporter *   locateMeshImporter(const char *fname) = 0; // based on this file name, find a matching mesh importer.

  virtual const char  *     getFileRequestDialogString(void) = 0;

  virtual void             setMeshImportApplicationResource(MeshImportApplicationResource *resource) = 0;

// convenience helper functions.
  virtual MeshSkeletonInstance *createMeshSkeletonInstance(const MeshSkeleton &sk) = 0;
  virtual bool                  sampleAnimationTrack(int trackIndex,const MeshSystem *msystem,MeshSkeletonInstance *skeleton) = 0;
  virtual void                  releaseMeshSkeletonInstance(MeshSkeletonInstance *sk) = 0;

  // apply bone weighting transforms to this vertex buffer.
  virtual void transformVertices(unsigned int vcount,
                                 const MeshVertex *source_vertices,
                                 MeshVertex *dest_vertices,
                                 MeshSkeletonInstance *skeleton) = 0;

  virtual MeshImportInterface * getMeshImportInterface(MeshSystemContainer *msc) = 0;

  virtual void gather(MeshSystemContainer *msc) = 0;

  virtual void scale(MeshSystemContainer *msc,float scale) = 0;


};

}; // End of namespace for MESHIMPORT

#define MESHIMPORT_VERSION 5  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORT::MeshImport *gMeshImport; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.

namespace SYSTEM_SERVICES
{
class SystemServices;
};

MESHIMPORT::MeshImport * loadMeshImporters(const char *directory,SYSTEM_SERVICES::SystemServices *services); // loads the mesh import library (dll) and all available importers from the same directory.

#pragma warning(pop)

#endif
