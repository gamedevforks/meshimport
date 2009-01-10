#include "binding.h"

#ifndef OPEN_SOURCE
#include "../../HeClientPlugins/HeClientPlugins.h"
#include "../../HeServerPlugins/HeServerPlugins.h"

HECLIENTPLUGINS::HeClientPlugins *gHeClientPlugins=0;
HESERVERPLUGINS::HeServerPlugins *gHeServerPlugins=0;
#endif

#ifdef _WIN32

#include <windows.h>

extern "C"
{

static bool gSuppressLoadError = true;

void setSuppressLoadError(bool state) // whether or not to suppress load error dialog boxes under windows.
{
  gSuppressLoadError = state;
}

void *getBindingInterfaceModule(const char *dll,int version_number,void *&rmodule) // loads the tetra maker DLL and returns the interface pointer.
{
  void *ret = 0;

  UINT errorMode = 0;
  if ( gSuppressLoadError ) errorMode = SEM_FAILCRITICALERRORS;
  UINT oldErrorMode = SetErrorMode(errorMode);
  HMODULE module = LoadLibraryA(dll);
  SetErrorMode(oldErrorMode);

  if ( module )
  {
    void *proc = GetProcAddress(module,"getInterface");
    if ( proc )
    {
      typedef void * (__cdecl * NX_GetToolkit)(int version);
  	  ret = ((NX_GetToolkit)proc)(version_number);
    }
  }

  if ( ret )
  {
    rmodule = module;
  }
  else
  {
    unloadBindingInterface(module);
    rmodule = 0;
  }

  return ret;
}


void *getBindingInterface(const char *dll,int version_number) // loads the tetra maker DLL and returns the interface pointer.
{
  void *ret = 0;

#ifndef OPEN_SOURCE
  if ( gHeClientPlugins )
  {
    HECLIENTPLUGINS::PluginType type = gHeClientPlugins->getPluginType(dll);
    if ( type != HECLIENTPLUGINS::PT_UNKNOWN )
    {
      ret = gHeClientPlugins->getPluginInterface(type,version_number);
    }
  }
  else if ( gHeServerPlugins )
  {
    HESERVERPLUGINS::PluginType type = gHeServerPlugins->getPluginType(dll);
    if ( type != HESERVERPLUGINS::PT_UNKNOWN )
    {
      ret = gHeServerPlugins->getPluginInterface(type,version_number);
    }
  }
#endif
  if ( ret == 0 )
  {
    UINT errorMode = 0;
    if ( gSuppressLoadError ) errorMode = SEM_FAILCRITICALERRORS;
    UINT oldErrorMode = SetErrorMode(errorMode);
    HMODULE module = LoadLibraryA(dll);
    SetErrorMode(oldErrorMode);
    if ( module )
    {
      void *proc = GetProcAddress(module,"getInterface");
      if ( proc )
      {
        typedef void * (__cdecl * NX_GetToolkit)(int version);
    	  ret = ((NX_GetToolkit)proc)(version_number);
      }
    }
  }
  return ret;
}


bool            unloadBindingInterface(void *module)
{
  bool ret = false;

  if ( module )
  {
    HMODULE hm = (HMODULE) module;
    BOOL b = FreeLibrary(hm);
    if ( b )
      ret = true;
  }


  return ret;
}

};

#endif

#ifdef LINUX
#include <dlfcn.h>
#include <stdio.h>
#include <string>

#define __cdecl
void * getBindingInterface(const char *dll_in,int version_number)
{
  void *ret = 0;

  if ( strcasecmp(dll_in,"HbPhysics.dll") == 0 )
  {
    dll_in = "libHbPhysics.so";
  }
  else if ( strcasecmp(dll_in,"PhysicsAsset.dll") == 0 )
  {
    dll_in = "libPhysicsAsset.so";
  }
  else if ( strcasecmp(dll_in,"RenderDebug.dll") == 0 )
  {
    dll_in = "libRenderDebug.so";
  }
  else if ( strcasecmp(dll_in,"HeroWorld.dll") == 0 )
  {
    dll_in = "libHeroWorld.so";
  }
  else if ( strcasecmp(dll_in,"CreateDynamics.dll") == 0 )
  {
    dll_in = "libCreateDynamics.so";
  }
  else if ( strcasecmp(dll_in,"SpeedTree.dll") == 0 )
  {
    dll_in = "libSpeedTree.so";
  }
  else if ( strcasecmp(dll_in,"PathMaker.dll") == 0 )
  {
    dll_in = "libPathMaker.so";
  }
  else if ( strcasecmp(dll_in,"HbPathSystem.dll") == 0 )
  {
    dll_in = "libHBPathSystem.so";
  }
  else if ( strcasecmp(dll_in,"HBPathSystem_AIWISDOM.dll") == 0 )
  {
    dll_in = "libHBPathSystem_AIWISDOM.so";
  }

  void* module = dlopen(dll_in, RTLD_LAZY);
  if ( module )
  {
    void* proc = dlsym(module, "getInterface");
    if ( proc )
    {
      typedef void * (__cdecl * NX_GetToolkit)(int version);
  	  ret = ((NX_GetToolkit)proc)(version_number);
    }
    else
    {
      printf("%s\n", dlerror());
    }
  }
  else
  {
    printf("%s\n%s\n", dlerror(), dll_in);
  }
  return ret;
}

bool            unloadBindingInterface(void *module)
{
	bool ret = false;
	if(module)
		if(!dlclose(module)) //0 == succeess
			ret = true;
	return ret;
}

#endif
