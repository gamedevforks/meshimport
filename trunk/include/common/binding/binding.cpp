#if defined(LINUX)
#include "linux_compat.h"
#endif

#include "binding.h"
#include "../snippets/UserMemAlloc.h"
#include <hash_map>
#include <string>

#ifndef OPEN_SOURCE
#include "../../HeClientPlugins/HeClientPlugins.h"
#include "../../HeServerPlugins/HeServerPlugins.h"
#endif

#define DLL_IN_EXT       ".dll"

typedef USER_STL_EXT::hash_map<USER_STL::string, PLUGIN_INTERFACE_FUNC> PLUGIN_INTERFACE_HASH;
PLUGIN_INTERFACE_HASH *gPluginInterfaceHash=0;

PLUGIN_INTERFACE_HASH & getHash(void)
{
  if ( gPluginInterfaceHash == 0 )
  {
    gPluginInterfaceHash = MEMALLOC_NEW(PLUGIN_INTERFACE_HASH);
  }
  return *gPluginInterfaceHash;
}


#ifdef _WIN32

#include <windows.h>
#endif

extern "C"
{

static bool gSuppressLoadError = true;

void setSuppressLoadError(bool state) // whether or not to suppress load error dialog boxes under windows.
{
  gSuppressLoadError = state;
}

void loadModuleInterfaces(const char *dll, void **rmodule) // loads the interface from a dll and populates it's internal list
{
#if defined(WIN32)
  UINT errorMode = 0;
  if ( gSuppressLoadError ) errorMode = SEM_FAILCRITICALERRORS;
  UINT oldErrorMode = SetErrorMode(errorMode);
  HMODULE module = LoadLibraryA(dll);
  SetErrorMode(oldErrorMode);
#else
  USER_STL::string mname(dll);
  mname.assign(mname.substr(0, mname.rfind(DLL_IN_EXT)));
  mname.insert(0,"lib");
  mname.append(".so");
  HMODULE module = (HMODULE)dlopen(mname.c_str(), RTLD_LAZY);
#endif

  bool found_exports = (module != 0);

  if ( module )
  {    
    #if defined(WIN32)
    void *proc = GetProcAddress(module,"getInterfaceList");
    #else
    void *proc = dlsym(module, "getInterfaceList");
    #endif
    if ( proc )
    {
      const INTERFACE_EXPORT *interfaces;
      int num = ((PLUGIN_INTERFACE_LIST_FUNC)proc)(&interfaces);
      for (int i = 0; i < num; i++)
      {
        getHash()[interfaces[i].name] = interfaces[i].func;
        found_exports = true;
      }
    }
    //LEGACY SUPPORT
    else // DO it the old way
    {
      #if defined(WIN32)
      void *proc = GetProcAddress(module,"getInterface");
      #else
      void *proc = dlsym(module, "getInterface");
      #endif
      if ( proc )
      {
        // store the name of the dll sans extension as the classname lookup
        USER_STL::string cname(dll);
        getHash()[cname.substr(0, cname.rfind(DLL_IN_EXT))] = (PLUGIN_INTERFACE_FUNC)proc;
        found_exports = true;
      }
    }
    //END LEGACY
  }

  if (found_exports)
  {
    if (rmodule)
      *rmodule = module;
  }
  else
  {
    unloadModule(module);
    if (rmodule)
      *rmodule = 0;
  }
}

void *getBindingInterface(const char *dll, const char *name, int version_number, SYSTEM_SERVICES::SystemServices *services, void **rmodule) // loads the dll and grabs the interface
{
  if (dll)
    loadModuleInterfaces(dll, rmodule);
  
  if (getHash()[name])
  {
    return getHash()[name](version_number, services);
  }
  return 0;
}


bool unloadModule(void *module)
{
  bool ret = false;

  if ( module )
  {
    HMODULE hm = (HMODULE) module;
    #if defined(WIN32)
    BOOL b = FreeLibrary(hm);
    #else
    BOOL b = dlclose(hm);
    #endif
    if ( b )
      ret = true;
  }


  return ret;
}

};
