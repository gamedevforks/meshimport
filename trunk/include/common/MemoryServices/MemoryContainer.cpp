/*  HeroEngine, Copyright 2002-2006 MemoryContainer, Corp.  All rights reserved.

*/

//-ignore_file      Do not allow SCANCPP to process this file!

#include "../snippets/UserMemAlloc.h"

#if HE_USE_MEMORY_TRACKING

#if defined(WIN32)
#include <Windows.h>
#include <tchar.h>
#else
typedef void* HANDLE;
#endif

#include "MemoryContainer.h"
#include <assert.h>
#include <malloc.h>

#pragma warning(disable:4100)


namespace MemoryContainer
{

IMPLEMENT_MEMORYPOOL_IN_CLASS(GlobalMemoryPool);

void* malloc(size_t s,const MemoryPoolClassDefinition& pool,const char *_file,int lineno)
{
#if HE_USE_MEMORY_TRACKING
  return MEMALLOC::malloc(MEMALLOC::gMemAlloc,(unsigned int)s,0,pool.name(), _file,lineno, MEMALLOC::MAT_MALLOC );
#else
  return MEMALLOC_MALLOC(s);
#endif
}

void  free(void * p, const MemoryPoolClassDefinition& pool)
{
  MEMALLOC_FREE(p);
}

void* realloc(void* p, size_t s, const MemoryPoolClassDefinition& pool)
{
  return MEMALLOC_REALLOC(p,s);
}

  void* malloc_aligned(size_t alignment, size_t s, const MemoryPoolClassDefinition& pool,const char *file,int lineno)
  {
#if HE_USE_MEMORY_TRACKING
    void * p = MEMALLOC::malloc( 0,(unsigned int)s, (unsigned int)alignment, pool.name(),file,lineno,MEMALLOC::MAT_MALLOC);
#else
    assert(0);    // not implemented!
    void *p = 0;
#endif

    return p;
  }

void* mallocSTL(size_t s, const MemoryPoolClassDefinition& pool)
{
#if HE_USE_MEMORY_TRACKING
  return MEMALLOC::malloc(MEMALLOC::gMemAlloc,(unsigned int)s,0,"STL",__FILE__,__LINE__, MEMALLOC::MAT_MALLOC );
#else
  return MEMALLOC_MALLOC(s);
#endif
}
void  freeSTL(void * p, const MemoryPoolClassDefinition& pool)
{
  MEMALLOC_FREE(p);
}

void* reallocSTL(void* p, size_t s, const MemoryPoolClassDefinition& pool)
{
  return MEMALLOC_REALLOC(p,s);
}

void* MemoryPool_Private::OperatorNew(POOL_NUMBER pool, size_t s,const char *className,const char *file,int lineno)
{
#if HE_USE_MEMORY_TRACKING
  return MEMALLOC::malloc(MEMALLOC::gMemAlloc,(unsigned int)s,0,className,file,lineno, MEMALLOC::MAT_NEW );
#else
  return MEMALLOC_MALLOC(s);
#endif

}

void  MemoryPool_Private::OperatorDelete(POOL_NUMBER pool, void* ptr)
{
#if HE_USE_MEMORY_TRACKING
  if ( ptr ) MEMALLOC::free(MEMALLOC::gMemAlloc,ptr,MEMALLOC::MAT_NEW);
#else
  MEMALLOC_FREE(ptr);
#endif
}

void* MemoryPool_Private::OperatorNewArray(POOL_NUMBER pool, size_t s,const char *className,const char *file,int lineno)
{
  return MEMALLOC_MALLOC(s);
}

void  MemoryPool_Private::OperatorDeleteArray(POOL_NUMBER pool, void* ptr)
{
  MEMALLOC_FREE(ptr);
}


}; // end of namespace

#endif
