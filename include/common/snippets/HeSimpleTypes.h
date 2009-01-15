#ifndef HE_FOUNDATION_NXSIMPLETYPES
#define HE_FOUNDATION_NXSIMPLETYPES
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/


//-ignore_file      Do not allow SCANCPP to process this file!

#include "He.h"
// Platform specific types:
//Design note: Its OK to use int for general loop variables and temps.

#ifdef WIN32
	typedef __int64				HeI64;
	typedef signed int			HeI32;
	typedef signed short		HeI16;
	typedef signed char			HeI8;

	typedef unsigned __int64	HeU64;
	typedef unsigned int		HeU32;
	typedef unsigned short		HeU16;
	typedef unsigned char		HeU8;

	typedef float				HeF32;
	typedef double				HeF64;
		
#elif LINUX
	typedef long long			HeI64;
	typedef signed int			HeI32;
	typedef signed short		HeI16;
	typedef signed char			HeI8;

	typedef unsigned long long	HeU64;
	typedef unsigned int		HeU32;
	typedef unsigned short		HeU16;
	typedef unsigned char		HeU8;

	typedef float				HeF32;
	typedef double				HeF64;

#elif __APPLE__
	typedef long long			HeI64;
	typedef signed int			HeI32;
	typedef signed short		HeI16;
	typedef signed char			HeI8;

	typedef unsigned long long	HeU64;
	typedef unsigned int		HeU32;
	typedef unsigned short		HeU16;
	typedef unsigned char		HeU8;

	typedef float				HeF32;
	typedef double				HeF64;

#elif __CELLOS_LV2__
	typedef long long			HeI64;
	typedef signed int			HeI32;
	typedef signed short		HeI16;
	typedef signed char			HeI8;

	typedef unsigned long long	HeU64;
	typedef unsigned int		HeU32;
	typedef unsigned short		HeU16;
	typedef unsigned char		HeU8;

	typedef float				HeF32;
	typedef double				HeF64;

#elif _XBOX
	typedef __int64				HeI64;
	typedef signed int			HeI32;
	typedef signed short		HeI16;
	typedef signed char			HeI8;

	typedef unsigned __int64	HeU64;
	typedef unsigned int		HeU32;
	typedef unsigned short		HeU16;
	typedef unsigned char		HeU8;

	typedef float				HeF32;
	typedef double				HeF64;

#else
	#error Unknown platform!
#endif

#if __APPLE__
    HE_COMPILE_TIME_ASSERT(sizeof(bool)==4);    // PPC has 4 byte bools
#else
	HE_COMPILE_TIME_ASSERT(sizeof(bool)==1);	// ...otherwise things might fail with VC++ 4.2 !
#endif
	HE_COMPILE_TIME_ASSERT(sizeof(HeI8)==1);
	HE_COMPILE_TIME_ASSERT(sizeof(HeU8)==1);
	HE_COMPILE_TIME_ASSERT(sizeof(HeI16)==2);
	HE_COMPILE_TIME_ASSERT(sizeof(HeU16)==2);
	HE_COMPILE_TIME_ASSERT(sizeof(HeI32)==4);
	HE_COMPILE_TIME_ASSERT(sizeof(HeU32)==4);
	HE_COMPILE_TIME_ASSERT(sizeof(HeI64)==8);
	HE_COMPILE_TIME_ASSERT(sizeof(HeU64)==8);
#if defined(NX64)
	HE_COMPILE_TIME_ASSERT(sizeof(void*)==8);
#else
	HE_COMPILE_TIME_ASSERT(sizeof(void*)==4);
#endif

	// Type ranges
	#define	HE_MAX_I8			0x7f			//max possible sbyte value
	#define	HE_MIN_I8			0x80			//min possible sbyte value
	#define	HE_MAX_U8			0xff			//max possible ubyte value
	#define	HE_MIN_U8			0x00			//min possible ubyte value
	#define	HE_MAX_I16			0x7fff			//max possible sword value
	#define	HE_MIN_I16			0x8000			//min possible sword value
	#define	HE_MAX_U16			0xffff			//max possible uword value
	#define	HE_MIN_U16			0x0000			//min possible uword value
	#define	HE_MAX_I32			0x7fffffff		//max possible sdword value
	#define	HE_MIN_I32			0x80000000		//min possible sdword value
	#define	HE_MAX_U32			0xffffffff		//max possible udword value
	#define	HE_MIN_U32			0x00000000		//min possible udword value
	#define	HE_MAX_F32			FLT_MAX			//max possible float value
	#define	HE_MIN_F32			(-FLT_MAX)		//min possible float value
	#define	HE_MAX_F64			DBL_MAX			//max possible double value
	#define	HE_MIN_F64			(-DBL_MAX)		//min possible double value
  #define HE_MAX_I64      0x7fffffffffffffffLL
  #define HE_MAX_U64      0xffffffffffffffffLL
  #define HE_MIN_U64      0
  #define HE_MIN_I64     0x8000000000000000LL

	#define HE_EPS_F32			FLT_EPSILON		//smallest number not zero
	#define HE_EPS_F64			DBL_EPSILON		//smallest number not zero

	#define HE_IEEE_1_0			0x3f800000		//integer representation of 1.0
	#define HE_IEEE_255_0		0x437f0000		//integer representation of 255.0
	#define HE_IEEE_MAX_F32		0x7f7fffff		//integer representation of MAX_NXFLOAT
	#define HE_IEEE_MIN_F32		0xff7fffff		//integer representation of MIN_NXFLOAT

	typedef int	HE_BOOL;
	#define HE_FALSE			0
	#define HE_TRUE				1

	#define	HE_MIN(a, b)		((a) < (b) ? (a) : (b))			//Returns the min value between a and b
	#define	HE_MAX(a, b)		((a) > (b) ? (a) : (b))			//Returns the max value between a and b

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
