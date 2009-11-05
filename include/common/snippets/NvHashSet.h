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

#ifndef NV_FOUNDATION_HASHSET
#define NV_FOUNDATION_HASHSET

#include "NvHashInternals.h"

// TODO: make this doxy-format

// This header defines two hash sets. Hash sets
// * support custom initial table sizes (rounded up internally to power-of-2)
// * support custom static allocator objects
// * auto-resize, based on a load factor (i.e. a 64-entry .75 load factor hash will resize 
//                                        when the 49th element is inserted)
// * are based on open hashing
//
// Sets have STL-like copying semantics, and properly initialize and destruct copies of objects
// 
// There are two forms of set: coalesced and uncoalesced. Coalesced sets keep the entries in the
// initial segment of an array, so are fast to iterate over; however deletion is approximately
// twice as expensive.
//
// HashSet<T>:
//		bool		insert(const T &k)						amortized O(1) (exponential resize policy)
// 		bool		contains(const T &k)	const;			O(1)
//		bool		erase(const T &k);						O(1)
//		NxU32		size()					const;			constant
//		void		reserve(NxU32 size);					O(MAX(size, currentOccupancy))
//		void		clear();								O(currentOccupancy) (with zero constant for objects without destructors) 
//      Iterator    getIterator();
//
// Use of iterators:
// 
// for(HashSet::Iterator iter = test.getIterator(); !iter.done(); ++iter)
//			myFunction(*iter);
//
// CoalescedHashSet<T> does not support getInterator, but instead supports
// 		const Key *getEntries();
//
// insertion into a set already containing the element fails returning false, as does
// erasure of an element not in the set
//



namespace NVSHARE
{
	template <class Key,
			  class HashFn = Hash<Key>,
			  class Allocator = Allocator >
	class HashSet: public Internal::HashSetBase<Key, HashFn, Allocator, false>
	{
	public:

		typedef Internal::HashSetBase<Key, HashFn, Allocator, false> HashSetBase;
		typedef typename HashSetBase::Iterator Iterator;

		HashSet(NxU32 initialTableSize = 64, float loadFactor = 0.75f):	HashSetBase(initialTableSize,loadFactor){}
		Iterator getIterator() { return Iterator(HashSetBase::mBase); }
	};

	template <class Key, 
			  class HashFn = Hash<Key>, 
			  class Allocator = Allocator >
	class CoalescedHashSet: public Internal::HashSetBase<Key, HashFn, Allocator, true>
	{
	public:
		typedef typename Internal::HashSetBase<Key, HashFn, Allocator, true> HashSetBase;

		CoalescedHashSet(NxU32 initialTableSize = 64, float loadFactor = 0.75f): HashSetBase(initialTableSize,loadFactor){}
		const Key *getEntries() { return HashSetBase::mBase.getEntries(); }
	};

}



#endif
