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

#ifndef NV_FOUNDATION_HASHFUNCTION_H
#define NV_FOUNDATION_HASHFUNCTION_H

#include "UserMemAlloc.h"

/*!
Central definition of hash functions
*/

namespace NVSHARE
{
	// Hash functions
	template<class T>
	NxU32 hash(const T& key)
	{
		return (NxU32)key;
	}

	// Thomas Wang's 32 bit mix
	// http://www.cris.com/~Ttwang/tech/inthash.htm
	template<>
	NX_INLINE NxU32 hash<NxU32>(const NxU32& key)
	{
		NxU32 k = key;
		k += ~(k << 15);
		k ^= (k >> 10);
		k += (k << 3);
		k ^= (k >> 6);
		k += ~(k << 11);
		k ^= (k >> 16);
		return (NxU32)k;
	}

	template<>
	NX_INLINE NxU32 hash<NxI32>(const NxI32& key)
	{
		return hash((NxU32)key);
	}

	// Thomas Wang's 64 bit mix
	// http://www.cris.com/~Ttwang/tech/inthash.htm
	template<>
	NX_INLINE NxU32 hash<NxU64>(const NxU64& key)
	{
		NxU64 k = key;
		k += ~(k << 32);
		k ^= (k >> 22);
		k += ~(k << 13);
		k ^= (k >> 8);
		k += (k << 3);
		k ^= (k >> 15);
		k += ~(k << 27);
		k ^= (k >> 31);
		return (NxU32)k;
	}

	// Helper for pointer hashing
	template<int size>
	NxU32 PointerHash(const void* ptr);

	template<>
	NX_INLINE NxU32 PointerHash<4>(const void* ptr)
	{
		return hash<NxU32>(static_cast<NxU32>(reinterpret_cast<size_t>(ptr)));
	}


	template<>
	NX_INLINE NxU32 PointerHash<8>(const void* ptr)
	{
		return hash<NxU64>(reinterpret_cast<size_t>(ptr));
	}

	// Hash function for pointers
	template<class T>
	NX_INLINE NxU32 hash(T* key)
	{
		return PointerHash<sizeof(const void*)>(key);
	}

	// Hash function object for pointers
	template <class T>
	struct PointerHashFunctor
	{
		NxU32 operator()(const T* t) const
		{
			return PointerHash<sizeof(T*)>(t);
		}
		bool operator()(const T* t0, const T* t1) const
		{
			return t0 == t1;
		}
	};

	/*
	--------------------------------------------------------------------
	lookup2.c, by Bob Jenkins, December 1996, Public Domain.
	--------------------------------------------------------------------
	--------------------------------------------------------------------
	mix -- mix 3 32-bit values reversibly.
	For every delta with one or two bit set, and the deltas of all three
	high bits or all three low bits, whether the original value of a,b,c
	is almost all zero or is uniformly distributed,
	* If mix() is run forward or backward, at least 32 bits in a,b,c
	have at least 1/4 probability of changing.
	* If mix() is run forward, every bit of c will change between 1/3 and
	2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
	mix() was built out of 36 single-cycle latency instructions in a 
	structure that could supported 2x parallelism, like so:
	a -= b; 
	a -= c; x = (c>>13);
	b -= c; a ^= x;
	b -= a; x = (a<<8);
	c -= a; b ^= x;
	c -= b; x = (b>>13);
	...
	Unfortunately, superscalar Pentiums and Sparcs can't take advantage 
	of that parallelism.  They've also turned some of those single-cycle
	latency instructions into multi-cycle latency instructions.  Still,
	this is the fastest good hash I could find.  There were about 2^^68
	to choose from.  I only looked at a billion or so.
	--------------------------------------------------------------------
	*/
	NX_INLINE NxU32 hashMix(NxU32 &a, NxU32 &b, NxU32 &c)
	{
		a -= b; a -= c; a ^= (c>>13);
		b -= c; b -= a; b ^= (a<<8);
		c -= a; c -= b; c ^= (b>>13);
		a -= b; a -= c; a ^= (c>>12);
		b -= c; b -= a; b ^= (a<<16);
		c -= a; c -= b; c ^= (b>>5);
		a -= b; a -= c; a ^= (c>>3);
		b -= c; b -= a; b ^= (a<<10);
		c -= a; c -= b; c ^= (b>>15);
	}

	NX_INLINE NxU32 hash(const NxU32 *k, NxU32 length)
	{
		NxU32 a,b,c,len;

		/* Set up the internal state */
		len = length;
		a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
		c = 0;           /* the previous hash value */

		/*---------------------------------------- handle most of the key */
		while (len >= 3)
		{
			a += k[0];
			b += k[1];
			c += k[2];
			hashMix(a,b,c);
			k += 3; 
			len -= 3;
		}

		/*-------------------------------------- handle the last 2 ub4's */
		c += length;
		switch(len)              /* all the case statements fall through */
		{
			/* c is reserved for the length */
		case 2 : b+=k[1];
		case 1 : a+=k[0];
			/* case 0: nothing left to add */
		}
		hashMix(a,b,c);
		/*-------------------------------------------- report the result */
		return c;
	}

	template <class Key>
	class Hash
	{
	public:
		NxU32 operator()(const Key &k) const { return hash<Key>(k); }
		bool operator()(const Key &k0, const Key &k1) const { return k0 == k1; }
	};

	class NvStringHash
	{
	public:
		NxU32 operator()(const char *string) const
		{
			// "DJB" string hash 
			NxU32 h = 5381;
			for(const char *ptr = string; *ptr; ptr++)
				h = ((h<<5)+h)^*ptr;
			return h;
		}
		bool operator()(const char* string0, const char* string1) const
		{
			return !strcmp(string0, string1);
		}
	};
}

#endif
