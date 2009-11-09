#ifndef USER_MEMALLOC_H

#define USER_MEMALLOC_H

#include "NxSimpleTypes.h"
#include <assert.h>

#include "NxSimpleTypes.h"

#include <new>

#ifndef NULL
#define NULL 0
#endif

#define USE_MEMALLOC 1

#define NVSHARE nvshare

#ifdef _DEBUG
#ifdef __CELLOS_LV2__
#define USE_MEMORY_TRACKER 0
#else
#define USE_MEMORY_TRACKER 1
#endif
#else
#define USE_MEMORY_TRACKER 0
#endif


namespace NVSHARE
{

	enum MemoryType
	{
		MT_NEW,
		MT_NEW_ARRAY,
		MT_MALLOC,
		MT_FREE,
		MT_DELETE,
		MT_DELETE_ARRAY,
		MT_GLOBAL_NEW,
		MT_GLOBAL_NEW_ARRAY,
		MT_GLOBAL_DELETE,
		MT_GLOBAL_DELETE_ARRAY,
	};



#if USE_MEMORY_TRACKER

class MemoryTracker
{
public:

  virtual void setLogLevel(bool logEveryAllocation,bool logEveyFrame) = 0;

  virtual void lock(void) = 0; // mutex lock.
  virtual void unlock(void) = 0; // mutex unlock

  // it will also fire asserts in a debug build.  The default is false for performance reasons.

  virtual void trackAlloc(void *mem,
                          NxU32 size,
                          MemoryType type,
                          const char *context,
                          const char *className,
                          const char *fileName,
                          NxI32 lineno) = 0;

  virtual void trackRealloc(void *oldMem,
	                        void *newMem,
							size_t newSize,
							MemoryType freeType,
							MemoryType allocType,
							const char *context,
							const char *className,
							const char *fileName,
							NxI32 lineno) = 0;

  virtual void trackFree(void *mem,
                         MemoryType type,
                         const char *context,
                         const char *fileName,
                         NxI32 lineno) = 0;

  virtual void trackFrame(void) = 0;


  virtual bool detectMemoryLeaks(const char *fname,bool reportAllLeaks=true) = 0; // detect memory leaks and, if any, write out a report to the filename specified.

};


}; // end of namespace

namespace NVSHARE
{
class SendTextMessage;

extern SendTextMessage *gSendTextMessage;


MemoryTracker * createMemoryTracker(void); // loads the DLL to track memory allocations.

#define MEMORY_TRACKER_VERSION 6

extern MemoryTracker *gMemoryTracker;

#define TRACK_LOCK() { if ( gMemoryTracker ) gMemoryTracker->lock(); };
#define TRACK_UNLOCK() { if ( gMemoryTracker ) gMemoryTracker->unlock(); };

#define TRACK_ALLOC(mem,size,type,context,className,fileName,lineno) { if ( gMemoryTracker ) gMemoryTracker->trackAlloc(mem,size,type,context,className,fileName,lineno); }
#define TRACK_FREE(mem,type,context,fileName,lineno)            { if ( gMemoryTracker ) gMemoryTracker->trackFree(mem,type,context,fileName,lineno); }
#define TRACK_REALLOC(oldMem,newMem,newSize,freeType,allocType,context,className,fileName,lineno) { if ( gMemoryTracker ) gMemoryTracker->trackRealloc(oldMem,newMem,newSize,freeType,allocType,context,className,fileName,lineno); }
#define TRACK_FRAME() { if ( gMemoryTracker ) gMemoryTracker->trackFrame(); }

#else

#define TRACK_LOCK()
#define TRACK_UNLOCK()

#define TRACK_ALLOC(mem,size,type,context,className,fileName,lineno)
#define TRACK_FREE(mem,type,context,fileName,lineno)
#define TRACK_REALLOC(oldMem,newMem,newSize,freeType,allocType,context,className,fileName,lineno)
#define TRACK_FRAME()

#endif

}; // end of namespace


//Header file defines memory allocation macros used by the ApexClothing integration
namespace NVSHARE
{

enum SendTextMessageType
{
	STM_CONSOLE,      // just echo the message to the console
	STM_WARNING,      // treat as a warning message
	STM_ERROR,        // treat as an error message
	STM_FILE,         // log to a file
};

class SendTextMessage
{
public:
	virtual void sendTextMessage(NxU32 type,const char *fmt,...) = 0;
};


extern SendTextMessage *gSendTextMessage;
#define SEND_TEXT_MESSAGE if ( NVSHARE::gSendTextMessage ) NVSHARE::gSendTextMessage->sendTextMessage

}; // end of namespace

namespace NVSHARE
{

class SystemServices
{
public:

  virtual SendTextMessage * getSendTextMessage(void) { return 0; }; // optional message logging interface.

  virtual void *            malloc(size_t size,
                                   NVSHARE::MemoryType type,
                                   const char *className,
                                   const char *fileName,
                                   int lineno) = 0;

  virtual void              free(void *mem,NVSHARE::MemoryType type,const char *fileName,int lineno) = 0;

  virtual void *            realloc(void *oldMem,
                                    size_t newSize,
                                    NVSHARE::MemoryType typeFree,   // type of memory being freed
                                    NVSHARE::MemoryType typeAlloc,  // type of memory being allocated.
                                    const char *className,
                                    const char *fileName,
                                    int lineno) = 0;

  virtual NxU32 getAllocaThreshold(void) const { return 8192; };

  virtual void makeAllocaAutoThresholdAtMost(NxU32 /* val */ ) {};

};

extern SystemServices *gSystemServices;

}; // end of namespace


namespace NVSHARE
{

#if USE_MEMALLOC
#define MEMALLOC_NEW(x) new(#x,__FILE__,__LINE__) x
#else
#define MEMALLOC_NEW(x) new x
#endif

inline void * inline_malloc(size_t x,const char *typeName,const char *fileName,int lineno)
{
    void *ret;

	if ( NVSHARE::gSystemServices )
    {
        ret = NVSHARE::gSystemServices->malloc(x,NVSHARE::MT_MALLOC,typeName,fileName,lineno);
    }
    else
    {
  	  ret = ::malloc(x);
	  TRACK_ALLOC(ret,(NxU32)x,NVSHARE::MT_MALLOC,"malloc",typeName,fileName,lineno);
    }
    return ret;
}

inline void inline_free(void *mem,const char *fileName,int lineno)
{
    if ( NVSHARE::gSystemServices )
    {
        NVSHARE::gSystemServices->free(mem,NVSHARE::MT_FREE,fileName,lineno);
    }
    else
    {
  	  TRACK_FREE(mem,NVSHARE::MT_FREE,"malloc",fileName,lineno);
	  ::free(mem);
    }
}

inline void * inline_realloc(void *oldMem,size_t newSize,const char *typeName,const char *fileName,int lineno)
{
    void *ret;

    if ( NVSHARE::gSystemServices )
    {
        ret = NVSHARE::gSystemServices->realloc(oldMem,newSize,NVSHARE::MT_FREE,NVSHARE::MT_MALLOC,typeName,fileName,lineno);
    }
    else
    {
	  ret = ::realloc(oldMem,newSize);
	  TRACK_REALLOC(oldMem,ret,newSize,NVSHARE::MT_FREE,NVSHARE::MT_MALLOC,"malloc",typeName,fileName,lineno);
    }
	return ret;
}


#ifdef NX_WINDOWS
#include <malloc.h>
#define MEMALLOC_ALLOCA(x)	_alloca(x)
#elif defined(NX_LINUX)
#include <malloc.h>
#define MEMALLOC_ALLOCA(x)	alloca(x)
#elif defined(NX_APPLE)
#include <alloca.h>
#include <stdlib.h>
#define MEMALLOC_ALLOCA(x)	alloca(x)
#elif defined(NX_CELL)
#include <alloca.h>
#include <stdlib.h>
#define MEMALLOC_ALLOCA(x)	alloca(x)
#elif defined(NX_X360)
#include <malloc.h>
#define MEMALLOC_ALLOCA(x)	_alloca(x)
#elif defined(NX_WII)
#include <alloca.h>
#define MEMALLOC_ALLOCA(x)	alloca(x)
#endif



#define MEMALLOC_MALLOC(x) NVSHARE::inline_malloc(x,"malloc",__FILE__,__LINE__)
#define MEMALLOC_MALLOC_TYPE(x,t,f,l) NVSHARE::inline_malloc(x,t,f,l)
#define MEMALLOC_FREE(x) NVSHARE::inline_free(x,__FILE__,__LINE__)
#define MEMALLOC_REALLOC(x,y) NVSHARE::inline_realloc(x,y,"malloc",__FILE__,__LINE__)

#pragma warning(push)
#pragma warning(disable:4100)


/**
Subclasses of this base class automatically take part in user memory management
Should be called Allocateable but then we collide with Ice::Allocateable.
*/
class Memalloc
{
public:
#if USE_MEMALLOC
	NX_INLINE void* operator new(size_t size,Memalloc *t);
	NX_INLINE void* operator new(size_t size);
	NX_INLINE void* operator new[](size_t size);
	NX_INLINE void* operator new(size_t size,const char *className,const char* fileName, int line);
	NX_INLINE void* operator new[](size_t size,const char *className,const char* fileName, int line);

	NX_INLINE void  operator delete(void* p,Memalloc *t);
	NX_INLINE void  operator delete(void* p);
	NX_INLINE void  operator delete[](void* p);
	NX_INLINE void  operator delete(void *p,const char *className,const char* fileName, int line);
	NX_INLINE void  operator delete[](void *p,const char *className,const char* fileName, int line);
#endif
};

#if USE_MEMALLOC
NX_INLINE void* Memalloc::operator new(size_t size,Memalloc *t)
{
	return t;
}


NX_INLINE void* Memalloc::operator new(size_t size)
{
	void *ret;
    if ( NVSHARE::gSystemServices )
    {
        ret = NVSHARE::gSystemServices->malloc(size,NVSHARE::MT_NEW,"Memalloc",__FILE__,__LINE__);
    }
    else
    {
	   ret = ::malloc(size);
      TRACK_ALLOC(ret,(NxU32)size,NVSHARE::MT_NEW,"UserMemAlloc","Memalloc",__FILE__,__LINE__);
    }
    return ret;
}

NX_INLINE void* Memalloc::operator new[](size_t size)
{
	void *ret;
    if ( NVSHARE::gSystemServices )
    {
        ret = NVSHARE::gSystemServices->malloc(size,NVSHARE::MT_NEW_ARRAY,"Memalloc",__FILE__,__LINE__);
    }
    else
    {
	  ret  = ::malloc(size);
      TRACK_ALLOC(ret,(NxU32)size,NVSHARE::MT_NEW_ARRAY,"UserMemAlloc","Memalloc",__FILE__,__LINE__);
    }
    return ret;
}

NX_INLINE void Memalloc::operator delete(void* p)
{
    if ( NVSHARE::gSystemServices )
    {
        NVSHARE::gSystemServices->free(p,NVSHARE::MT_DELETE,__FILE__,__LINE__);
    }
    else
    {
      TRACK_FREE(p,NVSHARE::MT_DELETE,"UserMemAlloc",__FILE__,__LINE__);
  	  ::free(p);
    }
}

NX_INLINE void Memalloc::operator delete(void* p,Memalloc *)
{
    assert(0); // should never be executed.
}

NX_INLINE void Memalloc::operator delete[](void* p)
{
    if ( NVSHARE::gSystemServices )
    {
        NVSHARE::gSystemServices->free(p,NVSHARE::MT_DELETE_ARRAY,__FILE__,__LINE__);
    }
    else
    {
      TRACK_FREE(p,NVSHARE::MT_DELETE_ARRAY,"UserMemAlloc",__FILE__,__LINE__);
	  ::free(p);
    }
}

NX_INLINE void Memalloc::operator delete(void* p,const char *className,const char *fileName,int lineno)
{
    if ( NVSHARE::gSystemServices )
    {
        NVSHARE::gSystemServices->free(p,NVSHARE::MT_DELETE,fileName,lineno);
    }
    else
    {
  	  TRACK_FREE(p,NVSHARE::MT_DELETE,"UserMemAlloc",fileName,lineno);
	  ::free(p);
    }
}

NX_INLINE void Memalloc::operator delete[](void* p,const char *className,const char *fileName,int lineno)
{
    if ( NVSHARE::gSystemServices )
    {
        NVSHARE::gSystemServices->free(p,NVSHARE::MT_DELETE_ARRAY,fileName,lineno);
    }
    else
    {
	  TRACK_FREE(p,NVSHARE::MT_DELETE_ARRAY,"UserMemAlloc",fileName,lineno);
	  ::free(p);
    }
}

NX_INLINE void* Memalloc::operator new(size_t size,const char *className,const char* fileName, int lineno)
{
    void *ret;
    if ( NVSHARE::gSystemServices )
    {
        ret = NVSHARE::gSystemServices->malloc(size,NVSHARE::MT_NEW,className,fileName,lineno);
    }
    else
    {
	  ret = ::malloc(size);
      TRACK_ALLOC(ret,(NxU32)size,NVSHARE::MT_NEW,"UserMemAlloc",className,fileName,lineno);
    }
    return ret;
}

NX_INLINE void* Memalloc::operator new[](size_t size,const char *className,const char* fileName, int lineno)
{
    void *ret;
    if ( NVSHARE::gSystemServices )
    {
        ret = NVSHARE::gSystemServices->malloc(size,NVSHARE::MT_NEW_ARRAY,className,fileName,lineno);
    }
    else
    {
	  ret = ::malloc(size);
      TRACK_ALLOC(ret,(NxU32)size,NVSHARE::MT_NEW_ARRAY,"UserMemAlloc",className,fileName,lineno);
    }
	return ret;
}
#endif


}; // end of namespace

#pragma warning(pop)


#endif
