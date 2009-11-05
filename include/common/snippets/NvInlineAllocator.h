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

#ifndef NV_FOUNDATION_INLINE_ALLOCATOR_H
#define NV_FOUNDATION_INLINE_ALLOCATOR_H

#include "NvUserAllocated.h"

namespace NVSHARE
{
	// this is used by the array class to allocate some space for a small number
	// of objects along with the metadata
	template<NxU32 N, typename BaseAllocator>
	class InlineAllocator : private BaseAllocator
	{
	public:

		InlineAllocator(const BaseAllocator& alloc = BaseAllocator())
			: BaseAllocator(alloc)
		{}

		void* allocate(size_t size, const char* filename, int line)
		{
			return size <= N ? mBuffer : BaseAllocator::allocate(size, filename, line);
		}

		void deallocate(void* ptr)
		{
			if(ptr != mBuffer)
				BaseAllocator::deallocate(ptr);
		}

	private:
		NxU8 mBuffer[N];
	};
}

#endif
