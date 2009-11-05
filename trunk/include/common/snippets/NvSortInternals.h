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

#ifndef NV_SORTINTERNALS_H
#define NV_SORTINTERNALS_H

/** \addtogroup foundation
@{
*/

#include "UserMemalloc.h"
#include "NvFastMemory.h"
#include "NvUtilities.h"
#include "NvUserAllocated.h"

namespace NVSHARE
{
	namespace Internal
	{
		template<class T, class Predicate>
		NX_INLINE void median3(T *elements, NxI32 first, NxI32 last, Predicate &compare)
		{
			/*
			This creates sentinels because we know there is an element at the start lower(or equal) 
			than the pivot and an element at the end greater(or equal) than the pivot. Plus the 
			median of 3 reduces the chance of degenerate behavour.
			*/

			NxI32 mid = (first + last)/2;

			if(compare(elements[mid], elements[first]))
				swap(elements[first], elements[mid]);

			if(compare(elements[last], elements[first]))
				swap(elements[first], elements[last]);

			if(compare(elements[last], elements[mid]))
				swap(elements[mid], elements[last]);

			//keep the pivot at last-1
			swap(elements[mid], elements[last-1]);
		}

		template<class T, class Predicate>
		NX_INLINE NxI32 partition(T *elements, NxI32 first, NxI32 last, Predicate &compare)
		{
			median3(elements, first, last, compare);

			/*
			WARNING: using the line:

			T partValue = elements[last-1];

			and changing the scan loops to:

			while(comparator.greater(partValue, elements[++i]));
			while(comparator.greater(elements[--j], partValue);

			triggers a compiler optimizer bug on xenon where it stores a double to the stack for partValue
			then loads it as a single...:-(
			*/

			NxI32 i = first;		//we know first is less than pivot(but i gets pre incremented) 
			NxI32 j = last - 1;		//pivot is in last-1 (but j gets pre decremented)

			while(true)
			{
				while(compare(elements[++i], elements[last-1]));
				while(compare(elements[last-1], elements[--j]));

				if(i>=j) break;

				NX_ASSERT(i<=last && j>=first);
				swap(elements[i], elements[j]);
			}
			//put the pivot in place

			NX_ASSERT(i<=last && first<=(last-1));
			swap(elements[i], elements[last-1]);

			return i;
		}

		template<class T, class Predicate>
		NX_INLINE void smallSort(T *elements, NxI32 first, NxI32 last, Predicate &compare)
		{
			//selection sort - could reduce to fsel on 360 with floats. 

			for(NxI32 i=first; i<last; i++)
			{
				NxI32 m = i;
				for(NxI32 j=i+1; j<=last; j++)
					if(compare(elements[j], elements[m])) m = j;

				swap(elements[m], elements[i]);
			}
		}


		class Stack
		{
			NxU32 mSize, mCapacity;
			NxI32 *mMemory;
			bool mRealloc;
		public:
			Stack(NxI32 *memory, NxU32 capacity): mMemory(memory), mSize(0), mCapacity(capacity), mRealloc(false) {}
			~Stack()
			{
				if(mRealloc) 
					MEMALLOC_FREE(mMemory);
			}

			void grow()
			{
				mCapacity *=2;
				NxI32 *newMem = (NxI32*) MEMALLOC_MALLOC(sizeof(NxI32)*mCapacity);
				fastMemcpy(newMem,mMemory,mSize*sizeof(NxI32));
				if(mRealloc) 
					MEMALLOC_FREE(mMemory);
				mRealloc = true;
				mMemory = newMem;
			}

			NX_INLINE void push(NxI32 start, NxI32 end) 
			{ 
				if(mSize >= mCapacity-1)
					grow();
				mMemory[mSize++] = start;
				mMemory[mSize++] = end;
			}

			NX_INLINE void pop(NxI32 &start, NxI32 &end)
			{
				NX_ASSERT(!empty());
				end = mMemory[--mSize];
				start = mMemory[--mSize];
			}

			NX_INLINE bool empty()
			{
				return mSize == 0;
			}
		};
	}
}
#endif
