#ifndef USER_MEMALLOC_H

#define USER_MEMALLOC_H

#include "NxSimpleTypes.h"
#include <assert.h>

#include "NxSimpleTypes.h"

#include <new>

#ifndef NULL
#define NULL 0
#endif

#define USE_MEMALLOC 0

#pragma warning(push)
#pragma warning(disable:4100)


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

	ret = ::malloc(x);

    return ret;
}

inline void inline_free(void *mem,const char *fileName,int lineno)
{
  ::free(mem);
}

inline void * inline_realloc(void *oldMem,size_t newSize,const char *typeName,const char *fileName,int lineno)
{
    void *ret;

	ret = ::realloc(oldMem,newSize);

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


#if USE_MEMALLOC
#define MEMALLOC_MALLOC(x) NVSHARE::inline_malloc(x,"malloc",__FILE__,__LINE__)
#define MEMALLOC_MALLOC_TYPE(x,t,f,l) NVSHARE::inline_malloc(x,t,f,l)
#define MEMALLOC_FREE(x) NVSHARE::inline_free(x,__FILE__,__LINE__)
#define MEMALLOC_REALLOC(x,y) NVSHARE::inline_realloc(x,y,"malloc",__FILE__,__LINE__)
#else
#define MEMALLOC_MALLOC(x) ::malloc(x)
#define MEMALLOC_MALLOC_TYPE(x,t,f,l) ::malloc(x)
#define MEMALLOC_FREE(x) ::free(x)
#define MEMALLOC_REALLOC(x,y) ::realloc(x,y)

#endif

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
   ret = ::malloc(size);
    return ret;
}

NX_INLINE void* Memalloc::operator new[](size_t size)
{
	void *ret;
  ret  = ::malloc(size);
    return ret;
}

NX_INLINE void Memalloc::operator delete(void* p)
{
  ::free(p);
}

NX_INLINE void Memalloc::operator delete(void* p,Memalloc *)
{
    assert(0); // should never be executed.
}

NX_INLINE void Memalloc::operator delete[](void* p)
{
  ::free(p);
}

NX_INLINE void Memalloc::operator delete(void* p,const char *className,const char *fileName,int lineno)
{
  ::free(p);
}

NX_INLINE void Memalloc::operator delete[](void* p,const char *className,const char *fileName,int lineno)
{
  ::free(p);
}

NX_INLINE void* Memalloc::operator new(size_t size,const char *className,const char* fileName, int lineno)
{
    void *ret;
  ret = ::malloc(size);
    return ret;
}

NX_INLINE void* Memalloc::operator new[](size_t size,const char *className,const char* fileName, int lineno)
{
    void *ret;
	ret = ::malloc(size);
	return ret;
}
#endif


}; // end of namespace

#pragma warning(pop)


#endif
