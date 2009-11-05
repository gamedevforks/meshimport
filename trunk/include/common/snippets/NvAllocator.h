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

#ifndef NV_FOUNDATION_ALLOCATOR
#define NV_FOUNDATION_ALLOCATOR

#include "Nx.h"
#include "NxUserAllocator.h"
#include "NxAssert.h"
#include "UserMemAlloc.h"

#include <stdlib.h>

#if (defined(NX_WINDOWS) | defined(NX_X360))
#include <typeinfo.h>
#endif

#ifndef WIN32
#define NX_GNUC 1
#endif

#pragma warning(push)
#pragma warning(disable:4100)

namespace NVSHARE
{


/**
\brief The return value is the greater of the two specified values.
*/
template<class N>
NX_INLINE N NvMax(N a, N b)							{	return a<b ? b : a;						}


/**
\brief The return value is the greater of the two specified values.
*/
template <>
NX_INLINE NxF32 NvMax(NxF32 a, NxF32 b)				{	return  a > b ? a : b;	}

/**
\brief The return value is the lesser of the two specified values.
*/
template<class N>
NX_INLINE N NvMin(N a, N b)							{	return a<b ? a : b;						}

/**
\brief The return value is the lesser of the two specified values.
*/
template <>
NX_INLINE NxF32 NvMin(NxF32 a, NxF32 b)				{	return a < b ? a : b;	}



	/**
	Allocator used to access the global NxUserAllocator instance without providing additional information.
	*/
	class Allocator
	{
	public:
		Allocator(const char* dummy = 0) 
		{
		}
		void* allocate(size_t size, const char* file, int line)
		{
      return MEMALLOC_MALLOC(size);
		}
		void deallocate(void* ptr)
		{
      MEMALLOC_FREE(ptr);
		}
	};

	/**
	Allocator used to access the global NxUserAllocator instance using a dynamic name.
	*/
	class NamedAllocator
	{
	public:
		NamedAllocator(const char* name = 0) 
			
		{

    }
		void* allocate(size_t size, const char* filename, int line)
		{
      return MEMALLOC_MALLOC(size);
		}
		void deallocate(void* ptr)
		{
      MEMALLOC_FREE(ptr);
		}
	private:
	};

	/**
	Allocator used to access the global NxUserAllocator instance using a static name derived from T.
	*/
	template <typename T>
	class ReflectionAllocator
	{
		static const char* getName()
		{
#if defined NX_GNUC
			return __PRETTY_FUNCTION__;
#else
			return typeid(T).name();
#endif
		}
	public:
		ReflectionAllocator(const char* dummy=0) 
		{
		}
		void* allocate(size_t size, const char* filename, int line)
		{
      return MEMALLOC_MALLOC(size);
		}
		void deallocate(void* ptr)
		{
      MEMALLOC_FREE(ptr);
		}
	};

	// if you get a build error here, you are trying to NX_NEW a class 
	// that is neither plain-old-type nor derived from NVSHARE::UserAllocated
	template <typename T, typename X>
	union EnableIfPod
	{
		int i; T t;
		typedef X Type;
	};

}

// Global placement new for ReflectionAllocator templated by plain-old-type. Allows using NX_NEW for pointers and built-in-types.
// ATTENTION: You need to use NX_DELETE_POD or NX_FREE to deallocate memory, not NX_DELETE. NX_DELETE_POD redirects to NX_FREE.
// Rationale: NX_DELETE uses global operator delete(void*), which we dont' want to overload. 
// Any other definition of NX_DELETE couldn't support array syntax 'NX_DELETE([]a);'. 
// NX_DELETE_POD was preferred over NX_DELETE_ARRAY because it is used less often and applies to both single instances and arrays.
template <typename T>
NX_INLINE void* operator new(size_t size, NVSHARE::ReflectionAllocator<T> alloc, const char* fileName, typename NVSHARE::EnableIfPod<T, int>::Type line)
{
	return alloc.allocate(size, fileName, line);
}

template <typename T>
NX_INLINE void* operator new[](size_t size, NVSHARE::ReflectionAllocator<T> alloc, const char* fileName, typename NVSHARE::EnableIfPod<T, int>::Type line)
{
	return alloc.allocate(size, fileName, line);
}

// If construction after placement new throws, this placement delete is being called.
template <typename T>
NX_INLINE void  operator delete(void* ptr, NVSHARE::ReflectionAllocator<T> alloc, const char* fileName, typename NVSHARE::EnableIfPod<T, int>::Type line)
{
	alloc.deallocate(ptr);
}

// If construction after placement new throws, this placement delete is being called.
template <typename T>
NX_INLINE void  operator delete[](void* ptr, NVSHARE::ReflectionAllocator<T> alloc, const char* fileName, typename NVSHARE::EnableIfPod<T, int>::Type line)
{
	alloc.deallocate(ptr);
}

#pragma warning(pop)

#endif
