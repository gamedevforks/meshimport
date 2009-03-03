#ifndef USER_MEMALLOC_H

#define USER_MEMALLOC_H

#include <new>
#define HE_USE_MEMORY_TRACKING 0

#ifndef NULL
#define NULL 0
#endif

#include "HeSimpleTypes.h"

#include "SystemServices.h"

#if HE_USE_MEMORY_TRACKING



#define HE_USE_FIXED_POOL_ALLOCATOR 1
#define HE_REPLACE_GLOBAL_NEW_DELETE 1

#define USER_STL MemoryContainer
#define USER_STL_EXT MemoryContainer
#define HE_USE_SIMUTRONICS_MEMORY_POOLING

#include "../MemoryServices/MemoryContainer.h"

#define MEMALLOC_NEW(x) new ( SYSTEM_SERVICES::init()->malloc(sizeof(x),0,#x,__FILE__,__LINE__,SYSTEM_SERVICES::MAT_NEW) )x
#define MEMALLOC_NEW_ARRAY(x,y) new ( SYSTEM_SERVICES::init()->malloc(sizeof(x)*(y)+sizeof(size_t),0,#x,__FILE__,__LINE__,SYSTEM_SERVICES::MAT_NEW_ARRAY) )x

#define MEMALLOC_DELETE(x,y) delete y;
#define MEMALLOC_DELETE_ARRAY(x,y) delete []y;

#define MEMALLOC_MALLOC_TYPE(x,t,f,l) SYSTEM_SERVICES::init()->malloc(x,0,t,f,l,SYSTEM_SERVICES::MAT_MALLOC)
#define MEMALLOC_MALLOC_TAGGED(x,t) SYSTEM_SERVICES::init()->malloc(x,0,t,__FILE__,__LINE__,SYSTEM_SERVICES::MAT_MALLOC)
#define MEMALLOC_MALLOC(x) SYSTEM_SERVICES::init()->malloc(x,0,__FILE__,__FILE__,__LINE__,SYSTEM_SERVICES::MAT_MALLOC)
#define MEMALLOC_FREE(x) SYSTEM_SERVICES::init()->free(x,SYSTEM_SERVICES::MAT_MALLOC)
#define MEMALLOC_REALLOC(x,y) SYSTEM_SERVICES::init()->realloc(x,y,__FILE__,__LINE__)
#define MEMALLOC_MALLOC_ALIGN(x,y,t,f,l) SYSTEM_SERVICES::init()->malloc(x,y,t,f,l,SYSTEM_SERVICES::MAT_MALLOC)


#else

#if defined(WIN32)
#define USER_STL std
#define USER_STL_EXT stdext
#else
/*
namespace MemoryContainer
{
  class GlobalMemoryPool
  {
  };
}
#include "../MemoryServices/simuhash_linux.h"
*/
#define USER_STL MemoryContainer
#endif

#define HE_REPLACE_GLOBAL_NEW_DELETE 0

void * mallocAlign(size_t size,size_t alignment);

#define MEMALLOC_NEW(x) new x
#define MEMALLOC_NEW_ARRAY(x,y) new x

#define MEMALLOC_DELETE(x,y) delete y;
#define MEMALLOC_DELETE_ARRAY(x,y) delete []y;

#define MEMALLOC_MALLOC_TYPE(x,t,f,l) ::malloc(x)
#define MEMALLOC_MALLOC_TAGGED(x,t) ::malloc(x)
#define MEMALLOC_MALLOC(x) ::malloc(x)
#define MEMALLOC_FREE(x) ::free(x)
#define MEMALLOC_REALLOC(x,y) ::realloc(x,y)
#define MEMALLOC_MALLOC_ALIGN(x,y,t,f,l) mallocAlign(x,y)

#define DEFINE_MEMORYPOOL_IN_CLASS(x)
#define IMPLEMENT_MEMORYPOOL_IN_CLASS(x)


#endif

#endif
