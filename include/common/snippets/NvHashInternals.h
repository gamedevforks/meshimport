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

#ifndef NV_FOUNDATION_HASHINTERNALS
#define NV_FOUNDATION_HASHINTERNALS

#include "NvBasicTemplates.h"
#include "NvArray.h"
#include "NvBitUtils.h"
#include "NvHash.h"

#pragma warning(push)
#pragma warning(disable:4127 4512) // disable the 'conditoinal expression is constant' warning message

namespace NVSHARE
{
	namespace Internal
	{
		template <class Entry,
				  class Key,
				  class HashFn,
				  class GetKey,
				  class Allocator,
				  bool compacting>
		class HashBase
		{
		public:
			typedef Entry EntryType;

			HashBase(NxU32 initialTableSize = 64, float loadFactor = 0.75f):
			mLoadFactor(loadFactor),
				mFreeList((NxU32)EOL),
				mTimestamp(0),
				mSize(0),
				mEntries(Allocator(NV_DEBUG_EXP("hashBaseEntries"))),
				mNext(Allocator(NV_DEBUG_EXP("hashBaseNext"))),
				mHash(Allocator(NV_DEBUG_EXP("hashBaseHash")))
			{
				if(initialTableSize)
					reserveInternal(initialTableSize);
			}

			~HashBase()
			{
				for(NxU32 i = 0;i<mHash.size();i++)
				{				
					for(NxU32 j = mHash[i]; j != EOL; j = mNext[j])
						mEntries[j].~Entry();
				}
			}

			static const int EOL = 0xffffffff;

			NX_INLINE Entry *create(const Key &k, bool &exists)
			{
				NxU32 h=0;
				if(mHash.size())
				{
					h = hash(k);
					NxU32 index = mHash[h];
					while(index!=EOL && !HashFn()(GetKey()(mEntries[index]), k))
						index = mNext[index];
					exists = index!=EOL;
					if(exists)
						return &mEntries[index];
				}

				if(freeListEmpty())
				{
					grow();
					h = hash(k);
				}

				NxU32 entryIndex = freeListGetNext();

				mNext[entryIndex] = mHash[h];
				mHash[h] = entryIndex;

				mSize++;
				mTimestamp++;

				return &mEntries[entryIndex];
			}

			NX_INLINE const Entry *find(const Key &k) const
			{
				if(!mHash.size())
					return false;

				NxU32 h = hash(k);
				NxU32 index = mHash[h];
				while(index!=EOL && !HashFn()(GetKey()(mEntries[index]), k))
					index = mNext[index];
				return index != EOL ? &mEntries[index]:0;
			}

			NX_INLINE bool erase(const Key &k)
			{
				if(!mHash.size())
					return false;

				NxU32 h = hash(k);
				NxU32 *ptr = &mHash[h];
				while(*ptr!=EOL && !HashFn()(GetKey()(mEntries[*ptr]), k))
					ptr = &mNext[*ptr];

				if(*ptr == EOL)
					return false;

				NxU32 index = *ptr;
				*ptr = mNext[index];

				mEntries[index].~Entry();

				mSize--;
				mTimestamp++;

				if(compacting && index!=mSize)
					replaceWithLast(index);

				freeListAdd(index);

				return true;
			}

			NX_INLINE NxU32 size() const
			{ 
				return mSize; 
			}

			void clear()
			{
				if(!mHash.size())
					return;

				for(NxU32 i = 0;i<mHash.size();i++)
					mHash[i] = (NxU32)EOL;
				for(NxU32 i = 0;i<mEntries.size()-1;i++)
					mNext[i] = i+1;
				mNext[mEntries.size()-1] = (NxU32)EOL;
				mFreeList = 0;
				mSize = 0;
			}

			void reserve(NxU32 size)
			{
				if(size>mHash.size())
					reserveInternal(size);
			}

			NX_INLINE const Entry *getEntries() const
			{
				return &mEntries[0];
			}

		private:

			// free list management - if we're coalescing, then we use mFreeList to hold
			// the top of the free list and it should always be equal to size(). Otherwise,
			// we build a free list in the next() pointers.

			NX_INLINE void freeListAdd(NxU32 index)
			{
				if(compacting)
				{
					mFreeList--;
					NX_ASSERT(mFreeList == mSize);
				}
				else
				{
					mNext[index] = mFreeList;
					mFreeList = index;
				}
			}

			NX_INLINE void freeListAdd(NxU32 start, NxU32 end)
			{
				if(!compacting)
				{
					for(NxU32 i = start; i<end-1; i++)	// add the new entries to the free list
						mNext[i] = i+1;
					mNext[end-1] = (NxU32)EOL;
				}
				mFreeList = start;
			}

			NX_INLINE NxU32 freeListGetNext()
			{
				NX_ASSERT(!freeListEmpty());
				if(compacting)
				{
					NX_ASSERT(mFreeList == mSize);
					return mFreeList++;
				}
				else
				{
					NxU32 entryIndex = mFreeList;
					mFreeList = mNext[mFreeList];
					return entryIndex;
				}
			}

			NX_INLINE bool freeListEmpty()
			{
				if(compacting)
					return mSize == mEntries.size();
				else
					return mFreeList == EOL;
			}

			NX_INLINE void replaceWithLast(NxU32 index)
			{
				new(&mEntries[index])Entry(mEntries[mSize]);
				mEntries[mSize].~Entry();
				mNext[index] = mNext[mSize];

				NxU32 h = hash(GetKey()(mEntries[index]));
				NxU32 *ptr;
				for(ptr = &mHash[h]; *ptr!=mSize; ptr = &mNext[*ptr])
					NX_ASSERT(*ptr!=EOL);
				*ptr = index;
			}


			NX_INLINE NxU32 hash(const Key &k) const
			{
				return HashFn()(k)&(mHash.size()-1);
			}

			void reserveInternal(NxU32 size)
			{
				size = nextPowerOfTwo(size);
				// resize the hash and reset
				mHash.resize(size);
				for(NxU32 i=0;i<mHash.size();i++)
					mHash[i] = (NxU32)EOL;

				NX_ASSERT(!(mHash.size()&(mHash.size()-1)));

				NxU32 oldSize = mEntries.size();
				NxU32 newSize = NxU32(float(mHash.size())*mLoadFactor);

				mEntries.resize(newSize);
				mNext.resize(newSize);

				freeListAdd(oldSize,newSize);

				// rehash all the existing entries
				for(NxU32 i=0;i<oldSize;i++)
				{
					NxU32 h = hash(GetKey()(mEntries[i]));
					mNext[i] = mHash[h];
					mHash[h] = i;
				}
			}

			void grow()
			{
				NX_ASSERT(mFreeList == EOL || compacting && mSize == mEntries.size());

				NxU32 size = mHash.size()==0 ? 16 : mHash.size()*2;
				reserve(size);
			}


			Array<Entry, Allocator>	mEntries;
			Array<NxU32, Allocator>	mNext;
			Array<NxU32, Allocator>	mHash;
			float					mLoadFactor;
			NxU32					mFreeList;
			NxU32					mTimestamp;
			NxU32					mSize;

			friend class Iter;

		public:
			class Iter
			{
			public:
				NX_INLINE Iter(HashBase &b): mBase(b), mTimestamp(b.mTimestamp), mBucket(0), mEntry((NxU32)b.EOL)
				{
					if(mBase.mEntries.size()>0)
					{
						mEntry = mBase.mHash[0];
						skip();
					}
				}

				NX_INLINE void check()				{ NX_ASSERT(mTimestamp == mBase.mTimestamp);	}
				NX_INLINE Entry operator*()			{ check(); return mBase.mEntries[mEntry];		}
				NX_INLINE Entry *operator->()		{ check(); return &mBase.mEntries[mEntry];		}
				NX_INLINE Iter operator++()			{ check(); advance(); return *this;				}
				NX_INLINE Iter operator++(int)		{ check(); Iter i = *this; advance(); return i;	}
				NX_INLINE bool done()				{ check(); return mEntry == mBase.EOL;			}

			private:
				NX_INLINE void advance()			{	mEntry = mBase.mNext[mEntry]; skip();		}
				NX_INLINE void skip()
				{
					while(mEntry==mBase.EOL) 
					{ 
						if(++mBucket == mBase.mHash.size())
							break;
						mEntry = mBase.mHash[mBucket];
					}
				}

				NxU32 mBucket;
				NxU32 mEntry;
				NxU32 mTimestamp;
				HashBase &mBase;
			};
		};

		template <class Key, 
				  class HashFn, 
				  class Allocator = Allocator,
				  bool Coalesced = false>
		class HashSetBase
		{ 
		public:
			struct GetKey { NX_INLINE const Key &operator()(const Key &e) {	return e; }	};

			typedef HashBase<Key, Key, HashFn, GetKey, Allocator, Coalesced> BaseMap;
			typedef typename BaseMap::Iter Iterator;

			HashSetBase(NxU32 initialTableSize = 64, 
						float loadFactor = 0.75f):	mBase(initialTableSize,loadFactor)	{}

			bool insert(const Key &k)
			{
				bool exists;
				Key *e = mBase.create(k,exists);
				if(!exists)
					new(e)Key(k);
				return !exists;
			}

			NX_INLINE bool		contains(const Key &k)	const	{	return mBase.find(k)!=0;		}
			NX_INLINE bool		erase(const Key &k)				{	return mBase.erase(k);			}
			NX_INLINE NxU32		size()					const	{	return mBase.size();			}
			NX_INLINE void		reserve(NxU32 size)				{	mBase.reserve(size);			}
			NX_INLINE void		clear()							{	mBase.clear();					}
		protected:
			BaseMap mBase;

		};

		template <class Key, 
			  class Value,
			  class HashFn, 
			  class Allocator = Allocator >

		class HashMapBase
		{ 
		public:
			typedef Pair<const Key,Value> Entry;
			struct GetKey { NX_INLINE const Key &operator()(const Entry &e) {	return e.first; }	};
			typedef HashBase<Pair<const Key,Value>, Key, HashFn, GetKey, Allocator, true> BaseMap;
			typedef typename BaseMap::Iter Iterator;

			HashMapBase(NxU32 initialTableSize = 64, float loadFactor = 0.75f):	mBase(initialTableSize,loadFactor)	{}

			bool insert(const Key &k, const Value &v)
			{
				bool exists;
				Entry *e = mBase.create(k,exists);
				if(!exists)
					new(e)Entry(k,v);
				return !exists;
			}

			Value &operator [](const Key &k)
			{
				bool exists;
				Entry *e = mBase.create(k, exists);
				if(!exists)
					new(e)Entry(k,Value());
		
				return e->second;
			}

			NX_INLINE const Entry *	find(const Key &k)		const	{	return mBase.find(k);			}
			NX_INLINE bool			erase(const Key &k)				{	return mBase.erase(k);			}
			NX_INLINE NxU32			size()					const	{	return mBase.size();			}
			NX_INLINE Iterator		getIterator()					{	return Iterator(mBase);			}
			NX_INLINE void			reserve(NxU32 size)				{	mBase.reserve(size);			}
			NX_INLINE void			clear()							{	mBase.clear();					}

		protected:
			BaseMap mBase;
		};

	}
}

#pragma warning(pop)

#endif
