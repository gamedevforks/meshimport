#ifndef SYSTEM_SERVICES_H

#define SYSTEM_SERVICES_H

#include <stdlib.h>

#pragma warning(push)
#pragma warning(disable:4100)

class SendTextMessage;

namespace RESOURCE_INTERFACE
{
  class  ResourceInterface;
};

namespace JOB_SWARM
{
  class JobSwarmContext;
};

namespace SYSTEM_SERVICES
{

enum MemoryAllocationType
{
  MAT_MALLOC,
  MAT_NEW,
  MAT_NEW_ARRAY,
};



class SystemServices
{
public:

  virtual SendTextMessage *                       getSendTextMessage(void)   { return 0; };  // application provides an optional console logging interface
  virtual RESOURCE_INTERFACE::ResourceInterface * getResourceInterface(void) { return 0; };  // application provides an optional resource acquistion interface
  virtual JOB_SWARM::JobSwarmContext *            getJobSwarmContext(void)   { return 0; };  // application provides an optional multi-threaded job interface

  virtual void *                                  malloc(size_t size,size_t align,const char *type,const char *file,int lineno,MemoryAllocationType mtype)
  {
    return ::malloc(size);
  }

  virtual void          free(void *mem,MemoryAllocationType mtype)
  {
    ::free(mem);
  }

  virtual void *        realloc(void *mem,size_t size,const char *file,int lineno)
  {
    return ::realloc(mem,size);
  }



};


SystemServices * init(void);

extern SystemServices *gSystemServices;

#pragma warning(pop)

};

#endif
