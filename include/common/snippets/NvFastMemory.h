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

#ifndef NV_FOUNDATION_FAST_MEMORY
#define NV_FOUNDATION_FAST_MEMORY

// moved here from LL, with the intent to platform-specialize them
#include "UserMemAlloc.h"

namespace NVSHARE
{
	NX_INLINE void fastMemzero(void* addr, size_t size)		
	{ 
		memset(addr, 0, size);	
	}

	NX_INLINE void* fastMemset(void* dest, int c, size_t count)
	{
		return memset(dest,c,count);
	}

	NX_INLINE void* fastMemcpy(void* dest, const void* src, size_t count)
	{
		return memcpy(dest,src,count);
	}

	NX_INLINE void* fastMemmove(void* dest, const void* src, size_t count)
	{
		return memmove(dest,src,count);
	}
	
	NX_INLINE void gatherStrided(const void* src, void* dst, NxU32 nbElem, NxU32 elemSize, NxU32 stride)
	{
		const NxU8* s = (const NxU8*)src;
		NxU8* d = (NxU8*)dst;
		while(nbElem--)
		{
			memcpy(d, s, elemSize);
			d += elemSize;
			s += stride;
		}
	}
}

#endif

