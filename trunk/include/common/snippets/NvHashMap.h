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
// Copyright � 2009 NVIDIA Corporation. All rights reserved.
// Copyright � 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright � 2001-2006 NovodeX. All rights reserved.

#ifndef NV_FOUNDATION_HASHMAP
#define NV_FOUNDATION_HASHMAP

#include "NvHashInternals.h"

// TODO: make this doxy-format
//
// This header defines two hash maps. Hash maps
// * support custom initial table sizes (rounded up internally to power-of-2)
// * support custom static allocator objects
// * auto-resize, based on a load factor (i.e. a 64-entry .75 load factor hash will resize 
//                                        when the 49th element is inserted)
// * are based on open hashing
// * have O(1) contains, erase
//
// Maps have STL-like copying semantics, and properly initialize and destruct copies of objects
// 
// There are two forms of map: coalesced and uncoalesced. Coalesced maps keep the entries in the
// initial segment of an array, so are fast to iterate over; however deletion is approximately
// twice as expensive.
//
// HashMap<T>:
//		bool			insert(const Key &k, const Value &v)	O(1) amortized (exponential resize policy)
//		Value &			operator[](const Key &k)				O(1) for existing objects, else O(1) amortized
//		const Entry *	find(const Key &k);						O(1)
//		bool			erase(const T &k);						O(1)
//		NxU32			size();									constant
//		void			reserve(NxU32 size);					O(MAX(currentOccupancy,size))
//		void			clear();								O(currentOccupancy) (with zero constant for objects without destructors) 
//      Iterator		getIterator();
//
// operator[] creates an entry if one does not exist, initializing with the default constructor.
// CoalescedHashMap<T> does not support getInterator, but instead supports
// 		const Key *getEntries();
//
// Use of iterators:
// 
// for(HashMap::Iterator iter = test.getIterator(); !iter.done(); ++iter)
//			myFunction(iter->first, iter->second);

namespace NVSHARE
{
	template <class Key,
			  class Value,
			  class HashFn = Hash<Key>,
			  class Allocator = Allocator >
	class HashMap: public Internal::HashMapBase<Key, Value, HashFn, Allocator>
	{
	public:

		typedef Internal::HashMapBase<Key, Value, HashFn, Allocator> HashMapBase;
		typedef typename HashMapBase::Iterator Iterator;

		HashMap(NxU32 initialTableSize = 64, float loadFactor = 0.75f):	HashMapBase(initialTableSize,loadFactor) {}
		Iterator getIterator() { return Iterator(HashMapBase::mBase); }
	};

	template <class Key, 
			  class Value,
			  class HashFn = Hash<Key>, 
			  class Allocator = Allocator >
	class CoalescedHashMap: public Internal::HashMapBase<Key, Value, HashFn, Allocator>
	{
		typedef Internal::HashMapBase<Key, Value, HashFn, Allocator> HashMapBase;

		CoalescedHashMap(NxU32 initialTableSize = 64, float loadFactor = 0.75f): HashMapBase(initialTableSize,loadFactor){}
		const Key *getEntries() const { return HashMapBase::mBase.getEntries(); }
	};

}



#endif