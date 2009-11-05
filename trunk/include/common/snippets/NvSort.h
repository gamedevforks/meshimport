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

#ifndef NV_SORT
#define NV_SORT

/** \addtogroup foundation
@{
*/

#include "NvSortInternals.h"
#include "NvFastMemory.h"
#include "NvUtilities.h"
#include "NvUserAllocated.h"

#ifdef _DEBUG
#define NX_SORT_PARANOIA
#endif


/**
\brief Sorts an array of objects in ascending order, assuming
that the predicate implements the < operator:

\see NVSHARE::Less, NVSHARE::Greater
*/

namespace NVSHARE
{
	template<class T, class Predicate>
	void sort(T *elements, NxU32 count, const Predicate &compare = Predicate())
	{
		static const int INITIAL_STACKSIZE = 32;
		static const NxU32 SMALL_SORT_CUTOFF = 5; // must be >= 3 since we need 3 for median

		NxI32 *stackMem;
		NX_ALLOCA(stackMem,NxI32,INITIAL_STACKSIZE);
		Internal::Stack stack(stackMem, INITIAL_STACKSIZE);

		NxI32 first = 0, last = count-1;
		if(last > first)
		{
			while(true)
			{
				while(last > first)
				{
					NX_ASSERT(first >= 0 && last < (NxI32)count);
					if((last-first) < SMALL_SORT_CUTOFF)
					{
						Internal::smallSort(elements, first, last, compare);
						break;
					}
					else
					{
						NxI32 partIndex = Internal::partition(elements, first, last, compare);

						// push smaller sublist to minimize stack usage
						if((partIndex - first) < (last - partIndex)) 
						{
							stack.push(first, partIndex-1);
							first = partIndex + 1;
						}
						else
						{
							stack.push(partIndex+1, last);
							last = partIndex - 1;
						}
					}
				}

				if(stack.empty())
					break;

				stack.pop(first, last);
			}
		}
#ifdef NX_SORT_PARANOIA
		for(NxU32 i=1; i<count; i++)
			NX_ASSERT(!compare(elements[i],elements[i-1]));
#endif

	}
}
#endif
