#ifndef NX_FOUNDATION_NXMATH
#define NX_FOUNDATION_NXMATH
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
#ifdef __PPCGEKKO__
#include "wii/NxMath_Wii.h"
#else
#include <math.h>
#include <float.h>
#include <stdlib.h>	//for rand()

#ifdef _XBOX
#include <ppcintrinsics.h> //for fpmin,fpmax, sqrt etc
#endif

#include "Nx.h"

#ifdef log2
#undef log2
#endif

//constants
static const PxF64 NxPiF64		= 3.141592653589793;
static const PxF64 NxHalfPiF64	= 1.57079632679489661923;
static const PxF64 NxTwoPiF64	= 6.28318530717958647692;
static const PxF64 NxInvPiF64	= 0.31830988618379067154;
//we can get bad range checks if we use PxF64 prec consts to check single prec results.
static const PxF32 NxPiF32		= 3.141592653589793f;
static const PxF32 NxHalfPiF32	= 1.57079632679489661923f;
static const PxF32 NxTwoPiF32	= 6.28318530717958647692f;
static const PxF32 NxInvPiF32	= 0.31830988618379067154f;


#if defined(min) || defined(max)
#error Error: min or max is #defined, probably in <windows.h>.  Put #define NOMINMAX before including windows.h to suppress windows global min,max macros.
#endif

/**
\brief Static class with stateless scalar math routines.
*/
class NxMath
	{
	public:

// Type conversion and rounding
		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		NX_INLINE static bool equals(PxF32,PxF32,PxF32 eps);

		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		NX_INLINE static bool equals(PxF64,PxF64,PxF64 eps);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		NX_INLINE static PxF32 floor(PxF32);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		NX_INLINE static PxF64 floor(PxF64);


		/**
		\brief The ceil function returns a single value representing the smallest integer that is greater than or equal to x. 
		*/
		NX_INLINE static PxF32 ceil(PxF32);
		/**
		\brief The ceil function returns a PxF64 value representing the smallest integer that is greater than or equal to x. 
		*/
		NX_INLINE static PxF64 ceil(PxF64);

		/**
		\brief Truncates the PxF32 to an integer.
		*/
		NX_INLINE static PxI32 trunc(PxF32);
		/**
		\brief Truncates the PxF64 precision PxF32 to an integer.
		*/
		NX_INLINE static PxI32 trunc(PxF64);


		/**
		\brief abs returns the absolute value of its argument. 
		*/
		NX_INLINE static PxF32 abs(PxF32);
		/**
		\brief abs returns the absolute value of its argument. 
		*/
		NX_INLINE static PxF64 abs(PxF64);
		/**
		\brief abs returns the absolute value of its argument. 
		*/
		NX_INLINE static PxI32 abs(PxI32);


		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		NX_INLINE static PxF32 sign(PxF32);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		NX_INLINE static PxF64 sign(PxF64);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		NX_INLINE static PxI32 sign(PxI32);


		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static PxF32 max(PxF32,PxF32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static PxF64 max(PxF64,PxF64);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static PxI32 max(PxI32,PxI32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static PxU32 max(PxU32,PxU32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static PxU16 max(PxU16,PxU16);
		
		
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static PxF32 min(PxF32,PxF32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static PxF64 min(PxF64,PxF64);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static PxI32 min(PxI32,PxI32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static PxU32 min(PxU32,PxU32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static PxU16 min(PxU16,PxU16);
		
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		NX_INLINE static PxF32 mod(PxF32 x, PxF32 y);
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		NX_INLINE static PxF64 mod(PxF64 x, PxF64 y);

		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static PxF32 clamp(PxF32 v, PxF32 hi, PxF32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static PxF64 clamp(PxF64 v, PxF64 hi, PxF64 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static PxU32 clamp(PxU32 v, PxU32 hi, PxU32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static PxI32 clamp(PxI32 v, PxI32 hi, PxI32 low);

		//!powers
		/**
		\brief Square root.
		*/
		NX_INLINE static PxF32 sqrt(PxF32);
		/**
		\brief Square root.
		*/
		NX_INLINE static PxF64 sqrt(PxF64);
		
		/**
		\brief reciprocal square root.
		*/
		NX_INLINE static PxF32 recipSqrt(PxF32);
		/**
		\brief reciprocal square root.
		*/
		NX_INLINE static PxF64 recipSqrt(PxF64);
		
		/**
		\brief Calculates x raised to the power of y.
		*/
		NX_INLINE static PxF32 pow(PxF32 x, PxF32 y);
		/**
		\brief Calculates x raised to the power of y.
		*/
		NX_INLINE static PxF64 pow(PxF64 x, PxF64 y);
		
		
		/**
		\brief Calculates e^n
		*/
		NX_INLINE static PxF32 exp(PxF32);
		/**
		\brief Calculates e^n
		*/
		NX_INLINE static PxF64 exp(PxF64);
		
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static PxF32 logE(PxF32);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static PxF64 logE(PxF64);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static PxF32 log2(PxF32);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static PxF64 log2(PxF64);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static PxF32 log10(PxF32);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static PxF64 log10(PxF64);

		//!trigonometry -- all angles are in radians.
		
		/**
		\brief Converts degrees to radians.
		*/
		NX_INLINE static PxF32 degToRad(PxF32);
		/**
		\brief Converts degrees to radians.
		*/
		NX_INLINE static PxF64 degToRad(PxF64);

		/**
		\brief Converts radians to degrees.
		*/
		NX_INLINE static PxF32 radToDeg(PxF32);
		/**
		\brief Converts radians to degrees.
		*/
		NX_INLINE static PxF64 radToDeg(PxF64);

		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF32 sin(PxF32);
		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF64 sin(PxF64);
		
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF32 cos(PxF32);
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF64 cos(PxF64);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static void sinCos(PxF32, PxF32 & sin, PxF32 & cos);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static void sinCos(PxF64, PxF64 & sin, PxF64 & cos);


		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF32 tan(PxF32);
		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF64 tan(PxF64);
		
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF32 asin(PxF32);
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF64 asin(PxF64);
		
		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF32 acos(PxF32);
		
		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF64 acos(PxF64);
		
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF32 atan(PxF32);
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF64 atan(PxF64);

		/**
		\brief Arctangent of (x/y) with correct sign.
		
		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF32 atan2(PxF32 x, PxF32 y);
		/**
		\brief Arctangent of (x/y) with correct sign.
		
		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static PxF64 atan2(PxF64 x, PxF64 y);

		//random numbers
		
		/**
		\brief uniform random number in [a,b]
		*/
		NX_INLINE static PxF32 rand(PxF32 a,PxF32 b);
		
		/**
		\brief uniform random number in [a,b]
		*/
		NX_INLINE static PxI32 rand(PxI32 a,PxI32 b);

		/**
		\brief hashing: hashes an array of n 32 bit values	to a 32 bit value.
		
		Because the output bits are uniformly distributed, the caller may mask
		off some of the bits to index into a hash table	smaller than 2^32.
		*/
		NX_INLINE static PxU32 hash(const PxU32 * array, PxU32 n);

		/**
		\brief hash32
		*/
		NX_INLINE static PxI32 hash32(PxI32);

		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		NX_INLINE static bool isFinite(PxF32 x);
		
		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		NX_INLINE static bool isFinite(PxF64 x);
	};

/*
Many of these are just implemented as NX_INLINE calls to the C lib right now,
but later we could replace some of them with some approximations or more
clever stuff.
*/
NX_INLINE bool NxMath::equals(PxF32 a,PxF32 b,PxF32 eps)
	{
	const PxF32 diff = NxMath::abs(a - b);
	return (diff < eps);
	}

NX_INLINE bool NxMath::equals(PxF64 a,PxF64 b,PxF64 eps)
	{
	const PxF64 diff = NxMath::abs(a - b);
	return (diff < eps);
	}

NX_INLINE PxF32 NxMath::floor(PxF32 a)
	{
	return ::floorf(a);
	}

NX_INLINE PxF64 NxMath::floor(PxF64 a)
	{
	return ::floor(a);
	}

NX_INLINE PxF32 NxMath::ceil(PxF32 a)
	{
	return ::ceilf(a);
	}

NX_INLINE PxF64 NxMath::ceil(PxF64 a)
	{
	return ::ceil(a);
	}

NX_INLINE PxI32 NxMath::trunc(PxF32 a)
	{
	return (PxI32) a;	// ### PT: this actually depends on FPU settings
	}

NX_INLINE PxI32 NxMath::trunc(PxF64 a)
	{
	return (PxI32) a;	// ### PT: this actually depends on FPU settings
	}

NX_INLINE PxF32 NxMath::abs(PxF32 a)
	{
	return ::fabsf(a);
	}

NX_INLINE PxF64 NxMath::abs(PxF64 a)
	{
	return ::fabs(a);
	}

NX_INLINE PxI32 NxMath::abs(PxI32 a)
	{
	return ::abs(a);
	}

NX_INLINE PxF32 NxMath::sign(PxF32 a)
	{
	return (a >= 0.0f) ? 1.0f : -1.0f;
	}

NX_INLINE PxF64 NxMath::sign(PxF64 a)
	{
	return (a >= 0.0) ? 1.0 : -1.0;
	}

NX_INLINE PxI32 NxMath::sign(PxI32 a)
	{
	return (a >= 0) ? 1 : -1;
	}

NX_INLINE PxF32 NxMath::max(PxF32 a,PxF32 b)
	{
#ifdef _XBOX
	return (PxF32)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
	}

NX_INLINE PxF64 NxMath::max(PxF64 a,PxF64 b)
	{
#ifdef _XBOX
	return (PxF64)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
	}

NX_INLINE PxI32 NxMath::max(PxI32 a,PxI32 b)
	{
	return (a < b) ? b : a;
	}

NX_INLINE PxU32 NxMath::max(PxU32 a,PxU32 b)
	{
	return (a < b) ? b : a;
	}

NX_INLINE PxU16 NxMath::max(PxU16 a,PxU16 b)
	{
	return (a < b) ? b : a;
	}

NX_INLINE PxF32 NxMath::min(PxF32 a,PxF32 b)
	{
#ifdef _XBOX
	return (PxF32)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
	}

NX_INLINE PxF64 NxMath::min(PxF64 a,PxF64 b)
	{
#ifdef _XBOX
	return (PxF64)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
	}

NX_INLINE PxI32 NxMath::min(PxI32 a,PxI32 b)
	{
	return (a < b) ? a : b;
	}

NX_INLINE PxU32 NxMath::min(PxU32 a,PxU32 b)
	{
	return (a < b) ? a : b;
	}

NX_INLINE PxU16 NxMath::min(PxU16 a,PxU16 b)
	{
	return (a < b) ? a : b;
	}

NX_INLINE PxF32 NxMath::mod(PxF32 x, PxF32 y)
	{
	return (PxF32)::fmod(x,y);
	}

NX_INLINE PxF64 NxMath::mod(PxF64 x, PxF64 y)
	{
	return ::fmod(x,y);
	}

NX_INLINE PxF32 NxMath::clamp(PxF32 v, PxF32 hi, PxF32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

NX_INLINE PxF64 NxMath::clamp(PxF64 v, PxF64 hi, PxF64 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

NX_INLINE PxU32 NxMath::clamp(PxU32 v, PxU32 hi, PxU32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

NX_INLINE PxI32 NxMath::clamp(PxI32 v, PxI32 hi, PxI32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

#ifdef _XBOX
NX_INLINE PxF32 NxMath::sqrt(PxF32 a)
	{
	return __fsqrts(a);
	}

NX_INLINE PxF64 NxMath::sqrt(PxF64 a)
	{
	return __fsqrt(a);
	}
#else
NX_INLINE PxF32 NxMath::sqrt(PxF32 a)
	{
	return ::sqrtf(a);
	}

NX_INLINE PxF64 NxMath::sqrt(PxF64 a)
	{
	return ::sqrt(a);
	}
#endif

NX_INLINE PxF32 NxMath::recipSqrt(PxF32 a)
	{
	return 1.0f/::sqrtf(a);
	}

NX_INLINE PxF64 NxMath::recipSqrt(PxF64 a)
	{
	return 1.0/::sqrt(a);
	}


NX_INLINE PxF32 NxMath::pow(PxF32 x, PxF32 y)
	{
	return ::powf(x,y);
	}

NX_INLINE PxF64 NxMath::pow(PxF64 x, PxF64 y)
	{
	return ::pow(x,y);
	}

NX_INLINE PxF32 NxMath::exp(PxF32 a)
	{
	return ::expf(a);
	}

NX_INLINE PxF64 NxMath::exp(PxF64 a)
	{
	return ::exp(a);
	}

NX_INLINE PxF32 NxMath::logE(PxF32 a)
	{
	return ::logf(a);
	}

NX_INLINE PxF64 NxMath::logE(PxF64 a)
	{
	return ::log(a);
	}

NX_INLINE PxF32 NxMath::log2(PxF32 a)
	{
	const PxF32 ln2 = (PxF32)0.693147180559945309417;
    return ::logf(a) / ln2;
	}

NX_INLINE PxF64 NxMath::log2(PxF64 a)
	{
	const PxF64 ln2 = (PxF64)0.693147180559945309417;
    return ::log(a) / ln2;
	}

NX_INLINE PxF32 NxMath::log10(PxF32 a)
	{
	return (PxF32)::log10(a);
	}

NX_INLINE PxF64 NxMath::log10(PxF64 a)
	{
	return ::log10(a);
	}

NX_INLINE PxF32 NxMath::degToRad(PxF32 a)
	{
	return (PxF32)0.01745329251994329547 * a;
	}

NX_INLINE PxF64 NxMath::degToRad(PxF64 a)
	{
	return (PxF64)0.01745329251994329547 * a;
	}

NX_INLINE PxF32 NxMath::radToDeg(PxF32 a)
	{
	return (PxF32)57.29577951308232286465 * a;
	}

NX_INLINE PxF64 NxMath::radToDeg(PxF64 a)
	{
	return (PxF64)57.29577951308232286465 * a;
	}

NX_INLINE PxF32 NxMath::sin(PxF32 a)
	{
	return ::sinf(a);
	}

NX_INLINE PxF64 NxMath::sin(PxF64 a)
	{
	return ::sin(a);
	}

NX_INLINE PxF32 NxMath::cos(PxF32 a)
	{
	return ::cosf(a);
	}

NX_INLINE PxF64 NxMath::cos(PxF64 a)
	{
	return ::cos(a);
	}

// Calling fsincos instead of fsin+fcos
NX_INLINE void NxMath::sinCos(PxF32 f, PxF32& s, PxF32& c)
	{
#if defined(WIN32) && !defined(_WIN64)
		PxF32 localCos, localSin;
		PxF32 local = f;
		_asm	fld		local
		_asm	fsincos
		_asm	fstp	localCos
		_asm	fstp	localSin
		c = localCos;
		s = localSin;
#else
		c = cosf(f);
		s = sinf(f);
#endif
	}

NX_INLINE void NxMath::sinCos(PxF64 a, PxF64 & s, PxF64 & c)
	{
	s = ::sin(a);
	c = ::cos(a);
	}

NX_INLINE PxF32 NxMath::tan(PxF32 a)
	{
	return ::tanf(a);
	}

NX_INLINE PxF64 NxMath::tan(PxF64 a)
	{
	return ::tan(a);
	}

NX_INLINE PxF32 NxMath::asin(PxF32 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return (PxF32)NxHalfPiF32;
	if(f<=-1.0f)return -(PxF32)NxHalfPiF32;
				return ::asinf(f);
	}

NX_INLINE PxF64 NxMath::asin(PxF64 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return (PxF32)NxHalfPiF64;
	if(f<=-1.0)	return -(PxF32)NxHalfPiF64;
				return ::asin(f);
	}

NX_INLINE PxF32 NxMath::acos(PxF32 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return 0.0f;
	if(f<=-1.0f)return (PxF32)NxPiF32;
				return ::acosf(f);
	}

NX_INLINE PxF64 NxMath::acos(PxF64 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return 0.0;
	if(f<=-1.0)	return (PxF64)NxPiF64;
				return ::acos(f);
	}

NX_INLINE PxF32 NxMath::atan(PxF32 a)
	{
	return ::atanf(a);
	}

NX_INLINE PxF64 NxMath::atan(PxF64 a)
	{
	return ::atan(a);
	}

NX_INLINE PxF32 NxMath::atan2(PxF32 x, PxF32 y)
	{
	return ::atan2f(x,y);
	}

NX_INLINE PxF64 NxMath::atan2(PxF64 x, PxF64 y)
	{
	return ::atan2(x,y);
	}

NX_INLINE PxF32 NxMath::rand(PxF32 a,PxF32 b)
	{
	const PxF32 r = (PxF32)::rand()/((PxF32)RAND_MAX+1);
	return r*(b-a) + a;
	}

NX_INLINE PxI32 NxMath::rand(PxI32 a,PxI32 b)
	{
	return a + (PxI32)(::rand()%(b-a));
	}

/*
--------------------------------------------------------------------
lookup2.c, by Bob Jenkins, December 1996, Public Domain.
hash(), hash2(), hash3, and mix() are externally useful functions.
Routines to test the hash are included if SELF_TEST is defined.
You can use this free for any purpose.  It has no warranty.
--------------------------------------------------------------------
--------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.
For every delta with one or two bit set, and the deltas of all three
  high bits or all three low bits, whether the original value of a,b,c
  is almost all zero or is uniformly distributed,
* If mix() is run forward or backward, at least 32 bits in a,b,c
  have at least 1/4 probability of changing.
* If mix() is run forward, every bit of c will change between 1/3 and
  2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
mix() was built out of 36 single-cycle latency instructions in a 
  structure that could supported 2x parallelism, like so:
      a -= b; 
      a -= c; x = (c>>13);
      b -= c; a ^= x;
      b -= a; x = (a<<8);
      c -= a; b ^= x;
      c -= b; x = (b>>13);
      ...
  Unfortunately, superscalar Pentiums and Sparcs can't take advantage 
  of that parallelism.  They've also turned some of those single-cycle
  latency instructions into multi-cycle latency instructions.  Still,
  this is the fastest good hash I could find.  There were about 2^^68
  to choose from.  I only looked at a billion or so.
--------------------------------------------------------------------
*/
#define NX_HASH_MIX(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

/*
--------------------------------------------------------------------
 This works on all machines.  hash2() is identical to hash() on 
 little-endian machines, except that the length has to be measured
 in ub4s instead of bytes.  It is much faster than hash().  It 
 requires
 -- that the key be an array of ub4's, and
 -- that all your machines have the same endianness, and
 -- that the length be the number of ub4's in the key
--------------------------------------------------------------------
*/
NX_INLINE PxU32 NxMath::hash(const PxU32 *k, PxU32 length)
//register ub4 *k;        /* the key */
//register ub4  length;   /* the length of the key, in ub4s */
	{
	PxU32 a,b,c,len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = 0;           /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 3)
	{
	  a += k[0];
	  b += k[1];
	  c += k[2];
	  NX_HASH_MIX(a,b,c);
	  k += 3; len -= 3;
	}

	/*-------------------------------------- handle the last 2 ub4's */
	c += length;
	switch(len)              /* all the case statements fall through */
	{
	 /* c is reserved for the length */
	case 2 : b+=k[1];
	case 1 : a+=k[0];
	 /* case 0: nothing left to add */
	}
	NX_HASH_MIX(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
	}
#undef NX_HASH_MIX

NX_INLINE PxI32 NxMath::hash32(PxI32 key)
	{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key;
	}


NX_INLINE bool NxMath::isFinite(PxF32 f)
	{
	#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
	#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
	#elif defined(__MWERKS__)
	return isfinite(f);
	#else
	return true;
	#endif
	
	}

NX_INLINE bool NxMath::isFinite(PxF64 f)
	{
	#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
	#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
	#elif defined(__MWERKS__)
	return isfinite(f);
	#else
	return true;
	#endif
	}

 /** @} */
#endif
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
