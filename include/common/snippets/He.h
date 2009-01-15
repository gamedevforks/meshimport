#ifndef HE_FOUNDATION_HE
#define HE_FOUNDATION_HE

/** \addtogroup foundation
  @{
*/

/**
DLL export macros
*/
#ifndef HE_C_EXPORT
	#define HE_C_EXPORT extern "C"
#endif

#ifndef HE_CALL_CONV
	#if defined WIN32
		#define HE_CALL_CONV __cdecl
	#elif defined LINUX
		#define HE_CALL_CONV
    #elif defined __APPLE__
        #define HE_CALL_CONV
	#elif defined __CELLOS_LV2__
		#define HE_CALL_CONV
	#elif defined _XBOX
        #define HE_CALL_CONV
	#else
		#error custom definition of HE_CALL_CONV for your OS needed!
	#endif
#endif

#if	  defined HE32
#elif defined HE64
#elif defined WIN32
	#ifdef HE64
		#error HeroEngine: Platforms pointer size ambiguous!  The defines WIN32 and HE64 are in conflict.
	#endif
	#define HE32
#elif defined WIN64
	#ifdef HE32
		#error HeroEngine: Platforms pointer size ambiguous!  The defines WIN64 and HE32 are in conflict.
	#endif
	#define HE64
#elif defined __CELLOS_LV2__
	#ifdef __LP32__
            #define HE32
    #else
            #define HE64
    #endif
#elif defined _XBOX
	#define HE32
#elif defined LINUX
        #define HE32
#else
	#error PhysX SDK: Platforms pointer size ambiguous.  Please define HE32 or He64 in the compiler settings!
#endif


#if !defined __CELLOS_LV2__
	#define HE_COMPILE_TIME_ASSERT(exp)	extern char HE_CompileTimeAssert[ size_t((exp) ? 1 : -1) ]
#else
    // GCC4 don't like the line above
	#define _CELL_HE_COMPILE_TIME_NAME_(x) HE_CompileTimeAssert ## x
	#define _CELL_HE_COMPILE_TIME_NAME(x) _CELL_HE_COMPILE_TIME_NAME_(x)
	#define HE_COMPILE_TIME_ASSERT(exp) extern char _CELL_HE_COMPILE_TIME_NAME(__LINE__)[ (exp) ? 1 : -1]
#endif


#if _MSC_VER
	#define HE_MSVC		// Compiling with VC++
	#if _MSC_VER >= 1400
		#define HE_VC8
	#elif _MSC_VER >= 1300
		#define HE_VC7		// Compiling with VC7
	#else
		#define HE_VC6		// Compiling with VC6
		#define __FUNCTION__	"Undefined"
	#endif
#endif

/**
 He SDK misc defines.
*/

//HE_INLINE
#if (_MSC_VER>=1000)
	#define HE_INLINE __forceinline	//alternative is simple inline
	#pragma inline_depth( 255 )
	#include <string.h>
	#include <stdlib.h>
#elif defined(__MWERKS__)
	//optional: #pragma always_inline on
	#define HE_INLINE inline
#else
	#define HE_INLINE inline
#endif

template<class Type> HE_INLINE void HE_Swap(Type& a, Type& b)
{
  const Type c = a; a = b; b = c;
}

#ifndef HE_SUV_WARNING
#define HE_SUV_WARNING
  template <typename T>
  void suppress_unused_variable_warning(const T&)
  {}
#endif

//files to always include:
#ifdef LINUX
#include <time.h>
#include <string.h>
#include <stdlib.h>
#elif __APPLE__
#include <time.h>
#elif __CELLOS_LV2__
	#include <string.h>
#endif
#include "HeSimpleTypes.h"

#include <assert.h>
#ifndef HE_ASSERT
  #ifdef _DEBUG
    #define HE_ASSERT(x) assert(x)
//      #define HE_ASSERT(x) {}
  #else
    #define HE_ASSERT(x) {}
  #endif
#endif


// Don't use inline for alloca !!!
#ifdef WIN32
	#include <malloc.h>
	#define HeAlloca(x)	_alloca(x)
#elif LINUX
	#include <malloc.h>
	#define HeAlloca(x)	alloca(x)
#elif __APPLE__
	#include <alloca.h>
	#include <stdlib.h>
	#define HeAlloca(x)	alloca(x)
#elif __CELLOS_LV2__
	#include <alloca.h>
	#include <stdlib.h>
	#define HeAlloca(x)	alloca(x)
#elif _XBOX
	#include <malloc.h>
	#define HeAlloca(x)	_alloca(x)
#endif

#ifdef WIN32

#define FORCE_NOINLINE __declspec(noinline)

#else

#define FORCE_NOINLINE

#endif

#endif
