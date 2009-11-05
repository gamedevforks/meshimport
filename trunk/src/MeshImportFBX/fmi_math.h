#ifndef FMI_MATH_H

#define FMI_MATH_H

namespace NVSHARE
{

inline float fbx_computePlane(const float *A,const float *B,const float *C,float *n) // returns D
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

inline void  fbx_transform(const float matrix[16],const float v[3],float t[3]) // rotate and translate this point
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

inline void  fbx_transformRotate(const float matrix[16],const float v[3],float t[3]) // rotate only
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

inline float fbx_normalize(float *n) // normalize this vector
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


inline void fbx_quatToMatrix(const float *quat,float *matrix) // convert quaterinion rotation to matrix, zeros out the translation component.
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
inline void fbx_getAngleAxis(float &angle,float *axis,const float *quat)
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

inline void fbx_setOrientationFromAxisAngle(const float axis[3],float angle,float *quat)
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


inline void fbx_identity(float *matrix)
{
  matrix[0*4+0] = 1;    matrix[1*4+1] = 1;    matrix[2*4+2] = 1;    matrix[3*4+3] = 1;
  matrix[1*4+0] = 0;    matrix[2*4+0] = 0;    matrix[3*4+0] = 0;
  matrix[0*4+1] = 0;    matrix[2*4+1] = 0;    matrix[3*4+1] = 0;
  matrix[0*4+2] = 0;    matrix[1*4+2] = 0;    matrix[3*4+2] = 0;
  matrix[0*4+3] = 0;    matrix[1*4+3] = 0;    matrix[2*4+3] = 0;
}


inline void fbx_fromQuat(float *matrix,const float quat[4])
{
  fbx_identity(matrix);

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

inline void fbx_matrixToQuat(const float *matrix,float *quat) // convert the 3x3 portion of a 4x4 matrix into a quaterion as x,y,z,w
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


inline float fbx_squared(float x) { return x*x; };

inline void fbx_decomposeTransform(const float local_transform[16],float trans[3],float rot[4],float scale[3])
{

  trans[0] = local_transform[12];
  trans[1] = local_transform[13];
  trans[2] = local_transform[14];

  scale[0] = sqrtf(fbx_squared(local_transform[0*4+0]) + fbx_squared(local_transform[0*4+1]) + fbx_squared(local_transform[0*4+2]));
  scale[1] = sqrtf(fbx_squared(local_transform[1*4+0]) + fbx_squared(local_transform[1*4+1]) + fbx_squared(local_transform[1*4+2]));
  scale[2] = sqrtf(fbx_squared(local_transform[2*4+0]) + fbx_squared(local_transform[2*4+1]) + fbx_squared(local_transform[2*4+2]));

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

  fbx_matrixToQuat(m,rot);

}

inline void fbx_fromScale(float *matrix,const float scale[3])
{
  fbx_identity(matrix);
  matrix[0*4+0] = scale[0];
  matrix[1*4+1] = scale[1];
  matrix[2*4+2] = scale[2];

}

inline void  fbx_multiply(const float *pA,const float *pB,float *pM)
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


inline void fbx_setTranslation(float *matrix,const float pos[3])
{
  matrix[12] = pos[0];  matrix[13] = pos[1];  matrix[14] = pos[2];
}


// compose this transform
inline void fbx_composeTransform(const float pos[3],const float quat[4],const float scale[3],float matrix[16])
{
  float mscale[16];
  float mrot[16];
  fbx_fromQuat(mrot,quat);
  fbx_fromScale(mscale,scale);
  fbx_multiply(mscale,mrot,matrix);
  fbx_setTranslation(matrix,pos);
}

inline float fbx_dot(const float *p1,const float *p2)
{
  return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2];
}

inline void fbx_cross(float *cross,const float *a,const float *b)
{
  cross[0] = a[1]*b[2] - a[2]*b[1];
  cross[1] = a[2]*b[0] - a[0]*b[2];
  cross[2] = a[0]*b[1] - a[1]*b[0];
}


inline float fbx_getDeterminant(const float matrix[16])
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

  fbx_cross(tempv,p1,p2);

  return fbx_dot(p0,tempv);

}

inline void fbx_getSubMatrix(int ki,int kj,float pDst[16],const float matrix[16])
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

inline void fbx_inverseTransform(const float matrix[16],float inverse_matrix[16])
{
  float determinant = fbx_getDeterminant(matrix);
  determinant = 1.0f / determinant;
  for (int i = 0; i < 4; i++ )
  {
    for (int j = 0; j < 4; j++ )
    {
      int sign = 1 - ( ( i + j ) % 2 ) * 2;
      float subMat[16];
      fbx_identity(subMat);
      fbx_getSubMatrix( i, j, subMat, matrix );
      float subDeterminant = fbx_getDeterminant(subMat);
      inverse_matrix[i*4+j] = ( subDeterminant * sign ) * determinant;
    }
  }
}

}; // end of namespace

#endif
