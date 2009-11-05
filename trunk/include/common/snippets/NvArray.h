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

#ifndef NV_FOUNDATION_ARRAY
#define NV_FOUNDATION_ARRAY

#include <new>
#include "UserMemAlloc.h"
#include "NxAssert.h"
#include "NxMath.h"

#include "NvAllocator.h"
#include "NvAlignedMalloc.h"
#include "NvInlineAllocator.h"
#include "NvUserAllocated.h"
#include "NvStrideIterator.h"

namespace NVSHARE
{
	namespace Internal
	{
		template <typename T>
		struct ArrayMetaData
		{
			T*					mData;
			NxU32				mCapacity;
			NxU32				mSize;
			ArrayMetaData(): mSize(0), mCapacity(0), mData(0) {}
		};

		template <typename T>
		struct AllocatorTraits
		{
#if defined _DEBUG
			typedef NamedAllocator Type;
#else
			typedef ReflectionAllocator<T> Type;
#endif
		};
	}

	/*!
	An array is a sequential container.

	Implementation note
	* entries between 0 and size are valid objects
	* we use inheritance to build this because the array is included inline in a lot
	  of objects and we want the allocator to take no space if it's not stateful, which
	  aggregation doesn't allow. Also, we want the metadata at the front for the inline
	  case where the allocator contains some inline storage space
	*/
	template<class T, class Alloc = typename Internal::AllocatorTraits<T>::Type >
	class Array : private Internal::ArrayMetaData<T>, private Alloc
	{
		typedef Internal::ArrayMetaData<T> MetaData;

		using MetaData::mCapacity;
		using MetaData::mData;
		using MetaData::mSize;

	public:

		typedef T*			Iterator;
		typedef const T*	ConstIterator;

		/*!
		Default array constructor. Initialize an empty array
		*/
		NX_INLINE Array(const Alloc& alloc = Alloc()) : Alloc(alloc) {}

		/*!
		Initialize array with given length
		*/
		NX_INLINE  explicit Array(NxU32 capacity, const Alloc& alloc = Alloc())
		: Alloc(alloc)
		{
			if(mCapacity>0)
				allocate(mCapacity);
		}

		/*!
		Copy-constructor. Copy all entries from other array
		*/
		template <class A> 
		NX_INLINE Array(const Array<T,A>& other, const Alloc& alloc = Alloc()) 
		{
			if(other.mSize > 0)
			{
				mData = allocate(mSize = mCapacity = other.mSize);
				copy(mData, other.mData, mSize);
			}
		}

		/*!
		Default destructor
		*/
		NX_INLINE ~Array()
		{
			destroy(0, mSize);
			if(mCapacity)
				deallocate(mData);
		}

		/*!
		Assignment operator. Copy content (deep-copy)
		*/
		template <class A> 
		NX_INLINE const Array& operator= (const Array<T,A>& t)
		{
			if(&t == this)
				return *this;

			if(mCapacity < t.mSize)
			{
				destroy(0,mSize);
				deallocate(mData);

				mData = allocate(t.mCapacity);
				mCapacity = t.mCapacity;

				copy(mData,t.mData,t.mSize);
				mSize = t.mSize;

				return;
			}
			else
			{
				NxU32 m = NvMin(t.mSize,mSize);
				copy(mData,t.mData,m);
				for(NxU32 i = m; i < mSize;i++)
					mData[i].~T();
				for(NxU32 i = m; i < t.mSize; i++)
					new(mData+i)T(t.mData[i]);
			}

			mSize = t.mSize;
			return *this;
		}

		/*!
		Array indexing operator.
		\param i
		The index of the element that will be returned.
		\return
		The element i in the array.
		*/
		NX_INLINE const T& operator[] (NxU32 i) const 
		{
			return mData[i];
		}

		/*!
		Array indexing operator.
		\param i
		The index of the element that will be returned.
		\return
		The element i in the array.
		*/
		NX_INLINE T& operator[] (NxU32 i) 
		{
			return mData[i];
		}

		/*!
		Returns a pointer to the initial element of the array.
		\return
		a pointer to the initial element of the array.
		*/
		NX_INLINE ConstIterator begin() const 
		{
			return mData;
		}

		NX_INLINE Iterator begin()
		{
			return mData;
		}

		/*!
		Returns an iterator beyond the last element of the array. Do not dereference.
		\return
		a pointer to the element beyond the last element of the array.
		*/

		NX_INLINE ConstIterator end() const 
		{
			return mData+mSize;
		}

		NX_INLINE Iterator end()
		{
			return mData+mSize;
		}

		/*!
		Returns a reference to the first element of the array. Undefined if the array is empty.
		\return a reference to the first element of the array
		*/

		NX_INLINE const T& front() const 
		{
			NX_ASSERT(mSize);
			return mData[0];
		}

		NX_INLINE T& front()
		{
			NX_ASSERT(mSize);
			return mData[0];
		}

		/*!
		Returns a reference to the last element of the array. Undefined if the array is empty
		\return a reference to the last element of the array
		*/

		NX_INLINE const T& back() const 
		{
			NX_ASSERT(mSize);
			return mData[mSize-1];
		}

		NX_INLINE T& back()
		{
			NX_ASSERT(mSize);
			return mData[mSize-1];
		}


		/*!
		Returns the number of entries in the array. This can, and probably will,
		differ from the array capacity.
		\return
		The number of of entries in the array.
		*/
		NX_INLINE NxU32 size() const 
		{
			return mSize;
		}

		/*!
		Clears the array.
		*/
		NX_INLINE void clear() 
		{
			destroy(0,mSize);
			mSize = 0;
		}

		/*!
		Returns whether the array is empty (i.e. whether its size is 0).
		\return
		true if the array is empty
		*/
		NX_INLINE bool empty() const
		{
			return mSize==0;
		}

		/*!
		Finds the first occurrence of an element in the array.
		\param a
		The element that will be removed. 
		*/


		NX_INLINE Iterator find(const T&a)
		{
			NxU32 index;
			for(index=0;index<mSize && mData[index]!=a;index++)
				;
			return mData+index;
		}

		NX_INLINE ConstIterator find(const T&a) const
		{
			NxU32 index;
			for(index=0;index<mSize && mData[index]!=a;index++)
				;
			return mData+index;
		}


		/////////////////////////////////////////////////////////////////////////
		/*!
		Adds one element to the end of the array. Operation is O(1).
		\param a
		The element that will be added to this array.
		*/
		/////////////////////////////////////////////////////////////////////////

		NX_INLINE T& pushBack(const T& a)
		{
			if(mCapacity<=mSize) 
				grow(capacityIncrement());

			new((void*)(mData + mSize)) T(a);

			return mData[mSize++];
		}

		/////////////////////////////////////////////////////////////////////////
		/*!
		Returns the element at the end of the array. Only legal if the array is non-empty.
		*/
		/////////////////////////////////////////////////////////////////////////
		NX_INLINE T popBack() 
		{
			NX_ASSERT(mSize);
			T t = mData[mSize-1];
			mData[--mSize].~T();
			return t;
		}


		/////////////////////////////////////////////////////////////////////////
		/*!
		Construct one element at the end of the array. Operation is O(1).
		*/
		/////////////////////////////////////////////////////////////////////////
		NX_INLINE T& insert()
		{
			if(mCapacity<=mSize) 
				grow(capacityIncrement());

			return *(new (mData+mSize++)T);
		}

		/////////////////////////////////////////////////////////////////////////
		/*!
		Subtracts the element on position i from the array and replace it with
		the last element.
		Operation is O(1)
		\param i
		The position of the element that will be subtracted from this array.
		\return
		The element that was removed.
		*/
		/////////////////////////////////////////////////////////////////////////
		NX_INLINE void replaceWithLast(NxU32 i)
		{
			NX_ASSERT(i<mSize);
			mData[i] = mData[--mSize];
			mData[mSize].~T();
		}

		NX_INLINE void replaceWithLast(Iterator i) 
		{
			replaceWithLast(static_cast<NxU32>(i-mData));
		}

		/////////////////////////////////////////////////////////////////////////
		/*!
		Replaces the first occurrence of the element a with the last element
		Operation is O(n)
		\param i
		The position of the element that will be subtracted from this array.
		\return Returns true if the element has been removed.
		*/
		/////////////////////////////////////////////////////////////////////////

		NX_INLINE bool findAndReplaceWithLast(const T& a)
		{
			NxU32 index;
			for(index=0;index<mSize && mData[index]!=a;index++)
				;
			if(index >= mSize)
				return false;
			replaceWithLast(index);
			return true;
		}

		/////////////////////////////////////////////////////////////////////////
		/*!
		Subtracts the element on position i from the array. Shift the entire
		array one step.
		Operation is O(n)
		\param i
		The position of the element that will be subtracted from this array.
		\return
		The element that was removed.
		*/
		/////////////////////////////////////////////////////////////////////////
		NX_INLINE void remove(NxU32 i) 
		{
			NX_ASSERT(i<mSize);
			while(i+1<mSize)
			{
				mData[i] = mData[i+1];
				i++;
			}

			mData[--mSize].~T();
		}


		//////////////////////////////////////////////////////////////////////////
		/*!
		Resize array
		\param compaction
		If set to true and the specified size is smaller than the capacity, a new
		memory block which fits the size is allocated and the old one gets freed.
		*/
		//////////////////////////////////////////////////////////////////////////
		NX_INLINE void resize(const NxU32 size, const bool compaction = false, const T& a = T())
		{
			if(size > mCapacity)
			{
				grow(size);
			}
			else if (compaction && (size != mCapacity))
			{
				recreate(size, NvMin(mSize, size));
			}

			for(NxU32 i = mSize; i < size; i++)
				::new(mData+i)T(a);

			if (!compaction)  // With compaction, these elements have been deleted already
			{
				for(NxU32 i = size; i < mSize; i++)
					mData[i].~T();
			}

			mSize = size;
		}


		//////////////////////////////////////////////////////////////////////////
		/*!
		Resize array such that only as much memory is allocated to hold the 
		existing elements
		*/
		//////////////////////////////////////////////////////////////////////////
		NX_INLINE void shrink()
		{
			resize(mSize, true);
		}


		//////////////////////////////////////////////////////////////////////////
		/*!
		Deletes all array elements and frees memory.
		*/
		//////////////////////////////////////////////////////////////////////////
		NX_INLINE void reset()
		{
			resize(0, true);
		}


		//////////////////////////////////////////////////////////////////////////
		/*!
		Ensure that the array has at least size capacity.
		*/
		//////////////////////////////////////////////////////////////////////////
		NX_INLINE void reserve(const NxU32 size)
		{
			if(size > mCapacity)
				grow(size);
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Query the capacity(allocated mem) for the array.
		*/
		//////////////////////////////////////////////////////////////////////////
		NX_INLINE NxU32 capacity()	const
		{
			return mCapacity;
		}


	private:

		NX_INLINE T* allocate(size_t capacity)
		{
			return (T*)Alloc::allocate(sizeof(T) * capacity, __FILE__, __LINE__);
		}

		NX_INLINE void deallocate(void *mem)
		{
			Alloc::deallocate(mem);
		}

		NX_INLINE void copy(T* dst, const T* src, size_t count)
		{
			for(size_t i=0;i<count;i++)
				::new (dst+i)T(src[i]);
		}

		NX_INLINE void destroy(size_t start, size_t end)
		{
			for(size_t i = start; i<end; i++)
				mData[i].~T();
		}

		// The idea here is to prevent accidental brain-damage with pushBack or insert. Unfortunately
		// it interacts badly with InlineArrays with smaller inline allocations.
		// TODO(dsequeira): policy template arg, this is exactly what they're for.
		NX_INLINE NxU32 capacityIncrement()	const
		{
			return mCapacity == 0 ? 1 : mCapacity * 2;
		}

		/*!
		Creates a new memory block, copies all entries to the new block and destroys old entries.

		\param capacity
		The number of entries that the set should be able to hold.
		\param copyCount
		The number of entries to copy.
		*/
		NX_INLINE void recreate(NxU32 capacity, NxU32 copyCount)
		{
			NX_ASSERT(capacity >= copyCount);
			NX_ASSERT(mSize >= copyCount);
			T* newData = allocate(capacity);
			NX_ASSERT(	((newData != NULL) && (capacity > 0)) ||
						((newData == NULL) && (capacity == 0)) );

			if(mCapacity)
			{
				copy(newData,mData,copyCount);
				destroy(0,mSize);
				deallocate(mData);
			}

			mData = newData;
			mCapacity = capacity;
		}

		/*!
		Resizes the available memory for the array.

		\param capacity
		The number of entries that the set should be able to hold.
		*/	
		NX_INLINE void grow(NxU32 capacity) 
		{
			NX_ASSERT(mCapacity < capacity);
			recreate(capacity, mSize);
		}
	};

	// array that pre-allocates for N elements
	template <typename T, NxU32 N, typename Alloc = typename Internal::AllocatorTraits<T>::Type>
	class InlineArray : public Array<T, InlineAllocator<N * sizeof(T), Alloc> >
	{
		typedef InlineAllocator<N * sizeof(T), Alloc> Allocator;
	public:
		NX_INLINE InlineArray(const Alloc& alloc = Alloc()) 
			: Array<T, Allocator>(alloc) 
		{}
	};
}

template <typename T>
NX_INLINE NvStrideIterator<T> getStrideIterator(NVSHARE::Array<T>& array)
{
	return NvStrideIterator<T>(array.begin(), sizeof(T));
}

template <typename T>
NX_INLINE NvStrideIterator<const T> getConstStrideIterator(NVSHARE::Array<T>& array)
{
	return NvStrideIterator<const T>(array.begin(), sizeof(T));
}


#endif
