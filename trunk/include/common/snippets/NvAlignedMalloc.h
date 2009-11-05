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

#ifndef NV_FOUNDATION_ALIGNEDMALLOC_H
#define NV_FOUNDATION_ALIGNEDMALLOC_H

#include "NvUserAllocated.h"

/*!
Allocate aligned memory.
Alignment must be a power of 2!
-- should be templated by a base allocator
*/

namespace NVSHARE
{
	/**
	Allocator, which is used to access the global NxUserAllocator instance
	(used for dynamic data types template instantiation), which can align memory
	*/

	// SCS: AlignedMalloc with 3 params not found, seems not used on PC either
	// disabled for now to avoid GCC error

	template<NxU32 N, typename BaseAllocator = Allocator >
	class AlignedAllocator : public BaseAllocator
	{
	public:
		AlignedAllocator(const BaseAllocator& base = BaseAllocator()) 
		: BaseAllocator(base) {}

		void* allocate(size_t size, const char* file, int line)
		{
			size_t pad = N - 1 + sizeof(size_t); // store offset for delete.
			NxU8* base = (NxU8*)BaseAllocator::allocate(size+pad, file, line);

			NxU8* ptr = (NxU8*)(size_t(base + pad) & ~(N - 1)); // aligned pointer
			((size_t*)ptr)[-1] = ptr - base; // store offset

			return ptr;
		}
		void deallocate(void* ptr)
		{
			if(ptr == NULL)
				return;

			NxU8* base = ((NxU8*)ptr) - ((size_t*)ptr)[-1];
			BaseAllocator::deallocate(base);
		}
	};
}

#endif
