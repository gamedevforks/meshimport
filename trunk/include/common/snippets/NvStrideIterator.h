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

#ifndef NV_FOUNDATION_NXSTRIDEDDATA
#define NV_FOUNDATION_NXSTRIDEDDATA
/** \addtogroup foundation
  @{
*/

template<typename T>
class NvStrideIterator
{
	template <typename X>
	struct StripConst
	{
		typedef X Type;
	};

	template <typename X>
	struct StripConst<const X>
	{
		typedef X Type;
	};

public:
	explicit NX_INLINE NvStrideIterator(T* ptr = NULL, NxU32 stride = sizeof(T)) :
		mPtr(ptr), mStride(stride)
	{
		NX_ASSERT(mStride == 0 || sizeof(T) <= mStride);
	}

	NX_INLINE NvStrideIterator(const NvStrideIterator<typename StripConst<T>::Type>& strideIterator) :
		mPtr(strideIterator.ptr()), mStride(strideIterator.stride())
	{
		NX_ASSERT(mStride == 0 || sizeof(T) <= mStride);
	}

	NX_INLINE T* ptr() const
	{
		return mPtr;
	}

	NX_INLINE NxU32 stride() const
	{
		return mStride;
	}

	NX_INLINE T& operator*() const
	{
		return *mPtr;
	}

	NX_INLINE T* operator->() const
	{
		return mPtr;
	}

	NX_INLINE T& operator[](int i) const
	{
		return *byteAdd(mPtr, i * stride());
	}

	// preincrement
	NX_INLINE NvStrideIterator& operator++()
	{
		mPtr = byteAdd(mPtr, stride());
		return *this;
	}

	// postincrement
	NX_INLINE NvStrideIterator operator++(int)
	{
		NvStrideIterator tmp = *this;
		mPtr = byteAdd(mPtr, stride());
		return tmp;
	}

	// predecrement
	NX_INLINE NvStrideIterator& operator--()
	{
		mPtr = byteSub(mPtr, stride());
		return *this;
	}

	// postdecrement
	NX_INLINE NvStrideIterator operator--(int)
	{
		NvStrideIterator tmp = *this;
		mPtr = byteSub(mPtr, stride());
		return tmp;
	}

	NX_INLINE NvStrideIterator& operator+=(int i)
	{
		mPtr = byteAdd(mPtr, i * stride());
		return *this;
	}

	NX_INLINE NvStrideIterator operator+(int i) const
	{	
		return NvStrideIterator(byteAdd(mPtr, i * stride()), stride());
	}

	NX_INLINE NvStrideIterator& operator-=(int i)
	{
		mPtr = byteSub(mPtr, i * stride());
		return *this;
	}

	NX_INLINE NvStrideIterator operator-(int i) const
	{
		return NvStrideIterator(byteSub(mPtr, i * stride()), stride());
	}

	// iterator difference
	NX_INLINE int operator-(const NvStrideIterator& other) const
	{
		NX_ASSERT(isCompatible(other));
		int byteDiff = static_cast<int>(reinterpret_cast<const NxU8*>(mPtr) - reinterpret_cast<const NxU8*>(other.mPtr));
		return byteDiff / static_cast<int>(stride());
	}

	NX_INLINE bool operator==(const NvStrideIterator& other) const
	{
		NX_ASSERT(isCompatible(other));
		return mPtr == other.mPtr;
	}

	NX_INLINE bool operator!=(const NvStrideIterator& other) const
	{
		NX_ASSERT(isCompatible(other));
		return mPtr != other.mPtr;
	}

	NX_INLINE bool operator<(const NvStrideIterator& other) const
	{
		NX_ASSERT(isCompatible(other));
		return mPtr < other.mPtr;
	}

	NX_INLINE bool operator>(const NvStrideIterator& other) const
	{
		NX_ASSERT(isCompatible(other));
		return mPtr > other.mPtr;
	}

	NX_INLINE bool operator<=(const NvStrideIterator& other) const
	{
		NX_ASSERT(isCompatible(other));
		return mPtr <= other.mPtr;
	}

	NX_INLINE bool operator>=(const NvStrideIterator& other) const
	{
		NX_ASSERT(isCompatible(other));
		return mPtr >= other.mPtr;
	}

private:
	NX_INLINE static T* byteAdd(T* ptr, NxU32 bytes) 
	{ 
		return const_cast<T*>(reinterpret_cast<const T*>(reinterpret_cast<const NxU8*>(ptr) + bytes));
	}

	NX_INLINE static T* byteSub(T* ptr, NxU32 bytes) 
	{ 
		return const_cast<T*>(reinterpret_cast<const T*>(reinterpret_cast<const NxU8*>(ptr) - bytes));
	}

	NX_INLINE bool isCompatible(const NvStrideIterator& other) const
	{
		int byteDiff = static_cast<int>(reinterpret_cast<const NxU8*>(mPtr) - reinterpret_cast<const NxU8*>(other.mPtr));
		return (stride() == other.stride()) && (abs(byteDiff) % stride() == 0);
	}

	T* mPtr;
	NxU32 mStride;
};


template<typename T>
NX_INLINE NvStrideIterator<T> operator+(int i, NvStrideIterator<T> it)
{
	it += i;
	return it;
}

 /** @} */
#endif
