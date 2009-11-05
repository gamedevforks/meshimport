// This code contains NVIDIA Confidential Information and is disclosed
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.

#ifndef NV_FOUNDATION_USERALLOCATED
#define NV_FOUNDATION_USERALLOCATED

#include "NvAllocator.h"
#include "UserMemAlloc.h"
#include <new>

// an expression that should expand to nothing in _DEBUG builds.  We currently
// use this only for tagging the purpose of containers for memory use tracking.
#if defined(_DEBUG)
#define NV_DEBUG_EXP(x) (x)
#define NV_DEBUG_EXP_C(x) x,
#else
#define NV_DEBUG_EXP(x)
#define NV_DEBUG_EXP_C(x)
#endif

#if defined (NX_X360) | defined (NX_WINDOWS) | defined (NX_CELL) | defined (NXLINUX) | defined(NX_WII) 
// Stack allocation with alloc fallback for large allocations (>50% of default stack size for platform)
#	define NX_ALLOCA(var, type, number)											\
		bool alloced_##var = false;												\
    if (sizeof(type)*number*2 > (NVSHARE::gSystemServices ? gSystemServices->getAllocaThreshold() : 8192)  )	\
		{																		\
			var = (type *)MEMALLOC_MALLOC(sizeof(type)*number);					\
			alloced_##var = true;												\
		} else {																\
			var = (type *)MEMALLOC_ALLOCA(sizeof(type)*number);						\
		}
#	define NX_FREEA(var) if (alloced_##var) MEMALLOC_FREE(var);
#else
#	define NX_ALLOCA(var, type, number)		var = (type *)NxAlloca(sizeof(type)*number);
#	define NX_FREEA(var)					0;
#endif

namespace NVSHARE
{
	/**
	Provides new and delete using a UserAllocator.
	Guarantees that 'delete x;' uses the UserAllocator too.
	*/
	class UserAllocated
	{
	public:

		template <typename Alloc>
		NX_INLINE void* operator new(size_t size, Alloc alloc, const char* fileName, int line)
		{
      return MEMALLOC_MALLOC(size);
		}
		template <typename Alloc>
		NX_INLINE void* operator new[](size_t size, Alloc alloc, const char* fileName, int line)
		{
      return MEMALLOC_MALLOC(size);
		}

		NX_INLINE void  operator delete(void* ptr)
		{
      MEMALLOC_FREE(ptr);
		}
		NX_INLINE void  operator delete[](void* ptr)
		{
      MEMALLOC_FREE(ptr);
		}
	};
};

#endif
