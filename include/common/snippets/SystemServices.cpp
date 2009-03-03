#include "SystemServices.h"
#include "UserMemAlloc.h"
#include <new>

namespace SYSTEM_SERVICES
{

SystemServices *gSystemServices=0;

#if HE_USE_MEMORY_TRACKING
class DefaultSystemServices : public SystemServices
{
public:
  DefaultSystemServices(void)
  {
    gSystemServices = static_cast< SystemServices *>(this);
  }
};


SystemServices * init(void)
{
  if ( gSystemServices == 0 )
  {
    void *foo = ::malloc(sizeof(DefaultSystemServices));
    gSystemServices = new ( foo ) DefaultSystemServices;
  }
  return gSystemServices;
}
#endif

}; // end of namespace
