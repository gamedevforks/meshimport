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

#ifndef NV_SLIST_H
#define NV_SLIST_H

#include "UserMemAlloc.h"
#include "Nx.h"
#include "NvAtomic.h"

#ifdef _XBOX
#include <xtl.h>
namespace NVSHARE
{
	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.
	typedef SLIST_ENTRY SListEntry;
	typedef SLIST_HEADER SListHeader;

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		InitializeSListHead(&header);
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		InterlockedPushEntrySList(&header, entry);
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		return InterlockedPopEntrySList(&header);
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		return InterlockedFlushSList(&header);
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return item->Next;
	}
}
#elif defined(_WIN32) | defined(_WIN64)
// For WinXp
#define NOMINMAX
#define _WIN32_WINNT 0x0500
#include <windows.h>
namespace NVSHARE
{
	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.
	typedef SLIST_ENTRY SListEntry;
	typedef SLIST_HEADER SListHeader;

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		InitializeSListHead(&header);
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		InterlockedPushEntrySList(&header, entry);
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		return InterlockedPopEntrySList(&header);
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		return InterlockedFlushSList(&header);
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return item->Next;
	}
}

#elif defined(__CELLOS_LV2__)
namespace NVSHARE
{
	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.

	struct SListEntry
	{
		volatile SListEntry *Next;
	};

	struct SListHeader
	{
		volatile SListEntry *Next;
		volatile NxI32 lock;		
	};

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		header.Next = NULL;
		header.lock = -1;// -1 == unlocked !=-1 means unlocked
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		entry->Next = header.Next;
		header.Next = entry;

		atomicExchange(&header.lock,-1);//unlock
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		if(header.Next!=NULL)
			header.Next = header.Next->Next;

		atomicExchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		header.Next = NULL;

		atomicExchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return (SListEntry *)item->Next;
	}

	/*
	private:

	static NX_INLINE void NxMemoryBarrier()
	{
	__lwsync();
	}*/
}

#elif defined(LINUX)
namespace NVSHARE
{
	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.

	struct SListEntry
	{
		volatile SListEntry *Next;
	};

	struct SListHeader
	{
		volatile SListEntry *Next;
		volatile NxI32 lock;
	};

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		header.Next = NULL;
		header.lock = -1;// -1 == unlocked, !=-1 means locked
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		entry->Next = header.Next;
		header.Next = entry;

		atomicExchange(&header.lock,-1);//unlock
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		if(header.Next!=NULL)
			header.Next = header.Next->Next;

		atomicExchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		header.Next = NULL;

		atomicExchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return (SListEntry *)item->Next;
	}
}

#elif defined(__PPCGEKKO__)

//! Class to provide simple interlocked operations (not threadsafe).
namespace NVSHARE
{
	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.

	struct SListEntry
	{
		volatile SListEntry *Next;
	};

	struct SListHeader
	{
		volatile SListEntry *Next;
		volatile NxI32 lock;
	};

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		header.Next = NULL;
		header.lock = -1;// -1 == unlocked, !=-1 means locked
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		entry->Next = header.Next;
		header.Next = entry;

		atomicExchange(&header.lock,-1);//unlock
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		if(header.Next!=NULL)
			header.Next = header.Next->Next;

		atomicExchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		while(atomicCompareExchange(&header.lock, 0, -1)!=-1);//lock

		volatile SListEntry *rv = header.Next;
		header.Next = NULL;

		atomicExchange(&header.lock,-1);//unlock

		return (SListEntry *)rv;
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return (SListEntry *)item->Next;
	}
}
#else


namespace NVSHARE
{
	//Note: this is a non thread safe version, used for platforms without threading support(PS3)
	static NX_INLINE void *atomicCompareExchangePointer(volatile void **dest,void *exch,void *comp)
	{
		volatile void *oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;

		return (void *)oldDest;
	}


	static NX_INLINE NxI32 atomicCompareExchange(volatile NxI32 *dest,NxI32 exch,NxI32 comp)
	{
		NxI32 oldDest=(*dest);
		if((*dest)==comp)
			(*dest)=exch;

		return oldDest;
	}

	static NX_INLINE NxI32 atomicIncrement(volatile NxI32 *val)
	{
		return ++(*val);
	}

	static NX_INLINE NxI32 atomicDecrement(volatile NxI32 *val)
	{
		//TODO: Need thread safe version for other platforms.
		return --(*val);
	}

	static NX_INLINE NxI32 atomicAdd(volatile NxI32 *val,NxI32 delta)
	{
		//TODO: Need thread safe version for other platforms.
		(*val)+=delta;
		return *val;
	}

	static NX_INLINE NxI32 atomicMax(volatile NxI32 *val,NxI32 val2)
	{
		//TODO: Need thread safe version for other platforms.
		if(val2>(*val))
			(*val)=val2;

		return *val;
	}

	static NX_INLINE NxI32 atomicExchange(volatile NxI32 *val,NxI32 val2)
	{
		NxI32 rv=*val;
		*val=val2;
		val2=rv;

		return rv;
	}


	// Warning SListEntry and SListHeader should be 16byte aligned for 64bit platforms.

	struct SListEntry
	{
		SListEntry *Next;
	};

	struct SListHeader
	{
		SListEntry *Next;
	};

	static NX_INLINE void InitializeSList(SListHeader &header)
	{
		header.Next = NULL;
	}

	static NX_INLINE void PushSList(SListHeader &header, SListEntry *entry)
	{
		entry->Next = header.Next;
		header.Next = entry;
	}
	static NX_INLINE SListEntry *PopSList(SListHeader &header)
	{
		SListEntry *rv = header.Next;
		if(header.Next!=NULL)
			header.Next = header.Next->Next;
		return rv;
	}

	// Flush atomically extracts all entries from the slist returning a pointer
	// to the head element.
	static NX_INLINE SListEntry *FlushSList(SListHeader &header)
	{
		SListEntry *rv = header.Next;
		header.Next = NULL;
		return rv;
	}

	//In general this should not be used unless the list has been flushed, since the
	// operation is not atomic.
	static NX_INLINE SListEntry *NextSList(SListEntry *item)
	{
		return item->Next;
	}
}
#endif


#endif
