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

#ifndef NV_POOL_H
#define NV_POOL_H

#include "UserMemAlloc.h"
#include "NvArray.h"
#include "NvSort.h"
#include "NvBasicTemplates.h"
#include "NvNoCopy.h"

namespace NVSHARE
{
	/*!
	Simple allocation pool
	*/
	template<class T>
	class Pool : public NoCopy, public Memalloc
	{
	public:
#if defined(_DEBUG)
		Pool (const char * DEBUGpurpose, NxU32 elementsPerSlab = 32) :
		  mElementsPerSlab (elementsPerSlab), 
			  mSlabSize (sizeof(T) * elementsPerSlab),
			  mFreeElement (0),
			  mUsed(0),
			  mDEBUGpurpose(DEBUGpurpose),
			  mSlabs(NV_DEBUG_EXP("poolSlabs"))
		  {
		  }
		  const char * mDEBUGpurpose;
#else
		Pool (NxU32 elementsPerSlab = 32) :
		  mElementsPerSlab (elementsPerSlab), 
			  mSlabSize (sizeof(T) * elementsPerSlab),
			  mFreeElement (0),
			  mUsed(0)
		  {
		  }
#endif

		  ~Pool ()
		  {
			  disposeElements();
		  }

		  NX_INLINE T* construct()
		  {
			  T* t = reinterpret_cast<T*>(allocateElement());
			  return t == 0 ? 0 : new (t) T;
		  }

		  template<class A1>
		  NX_INLINE T* construct(A1& a)
		  {
			  T* t = reinterpret_cast<T*>(allocateElement());
			  return t == 0 ? 0 : new (t) T (a);
		  }

		  template<class A1, class A2>
		  NX_INLINE T* construct(A1& a, A2& b)
		  {
			  T* t = reinterpret_cast<T*>(allocateElement());
			  return t == 0 ? 0 : new (t) T (a, b);
		  }

		  template<class A1, class A2, class A3>
		  NX_INLINE T* construct(A1& a, A2& b, A3& c)
		  {
			  T* t = reinterpret_cast<T*>(allocateElement());
			  return t == 0 ? 0 : new (t) T (a, b, c);
		  }


		  NX_INLINE void destroy(T* const p)
		  {
			  if(p)
			  {
				  p->~T();
				  freeElement(p);
			  }
		  }

	protected:
		struct FreeList
		{
			FreeList* mNext;
		};

	

		// All the allocated slabs, sorted by pointer
		NVSHARE::Array<void*> mSlabs;

		NxU32 mElementsPerSlab;
		NxU32 mSlabSize;
		NxU32 mUsed;

		FreeList* mFreeElement; // Head of free-list


		// Helper function to get bitmap of allocated elements

		// Allocate a slab and segregate it into the freelist
		void allocateSlab ()
		{
#if defined(_DEBUG)
			NVSHARE::NamedAllocator alloc(mDEBUGpurpose);			
#else
			NVSHARE::Allocator alloc;
#endif
			NxU8* slab = reinterpret_cast<NxU8*>(alloc.allocate(mSlabSize, __FILE__, __LINE__));

			//Save the slab ptr
			mSlabs.pushBack(slab);

			// Build a chain of nodes for the freelist
			FreeList* nextFree = mFreeElement;
			NxU8* node = slab + (mElementsPerSlab - 1) * sizeof(T);
			for (; node >= slab; node -= sizeof(T))
			{
				FreeList* element = reinterpret_cast<FreeList*> (node);
				element->mNext = nextFree;
				nextFree = element;
			}
			mFreeElement = nextFree;
		}

		// Allocate space for single object
		NX_INLINE void* allocateElement ()
		{
			if(mFreeElement == 0)
				allocateSlab();

			mUsed++;

			void* element = mFreeElement;
			mFreeElement = mFreeElement->mNext;
			return element;
		}

		// Put space for a single element back in the lists
		NX_INLINE void freeElement (void* p)
		{
			FreeList* element = reinterpret_cast<FreeList*>(p);
			element->mNext = mFreeElement;
			mFreeElement = element;

			mUsed--;
		}

		/*
		Cleanup method. Go through all active slabs and call destructor for live objects,
		then free their memory
		*/
		void disposeElements ()
		{
			if(mUsed)
			{
				NVSHARE::Array <void*> freeNodes NV_DEBUG_EXP("disposeFreeNodes");
				while(mFreeElement)
				{
					freeNodes.pushBack(mFreeElement);
					mFreeElement = mFreeElement->mNext;
				}
				NVSHARE::sort(freeNodes.begin(), freeNodes.size(), NVSHARE::Less<void *>());
				NVSHARE::sort(mSlabs.begin(), mSlabs.size(), NVSHARE::Less<void *>());

				NxU32 freeIndex = 0;
				for(NxU32 i = 0; i < mSlabs.size(); ++i)
				{
					NxU8* slab = reinterpret_cast<NxU8*>(mSlabs[i]);
					for(NxU32 elId = 0; elId < mElementsPerSlab; ++elId)
					{
						void* element = slab + elId*sizeof(T);
						NX_ASSERT((freeNodes.size() == 0) || (element<=freeNodes[freeIndex]));
						if((freeNodes.size() > 0) && (freeNodes[freeIndex] == element))
							freeIndex++;
						else
							reinterpret_cast<T*>(element)->~T();
					}
				}
				NX_ASSERT(freeIndex == freeNodes.size());
			}

			for(NxU32 i = 0; i < mSlabs.size(); ++i)
			{
				MEMALLOC_FREE(mSlabs[i]);
			}
			mFreeElement = 0;
		}
	};
}
#endif
