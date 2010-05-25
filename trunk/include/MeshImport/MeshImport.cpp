#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#endif

#include "MeshImport.h"

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace NVSHARE
{

#ifdef WIN32

static void *getMeshBindingInterface(const char *dll,PxI32 version_number) // loads the tetra maker DLL and returns the interface pointer.
{
  void *ret = 0;

  UINT errorMode = 0;
  errorMode = SEM_FAILCRITICALERRORS;
  UINT oldErrorMode = SetErrorMode(errorMode);
  HMODULE module = LoadLibraryA(dll);
  SetErrorMode(oldErrorMode);
  if ( module )
  {
    void *proc = GetProcAddress(module,"getInterface");
    if ( proc )
    {
      typedef void * (__cdecl * NX_GetToolkit)(PxI32 version);
      ret = ((NX_GetToolkit)proc)(version_number);
    }
  }
  return ret;
}



#endif

}; // end of namespace

#ifdef LINUX_GENERIC
#include <sys/types.h>
#include <sys/dir.h>
#endif

#define MAXNAME 512

#define MESHIMPORT_NVSHARE MESHIMPORT_##NVSHARE

namespace MESHIMPORT_NVSHARE
{

class FileFind
{
public:
  FileFind::FileFind(const char *dirname,const char *spec)
  {
    if ( dirname && strlen(dirname) )
      sprintf(mSearchName,"%s\\%s",dirname,spec);
    else
      sprintf(mSearchName,"%s",spec);
   }

  FileFind::~FileFind(void)
  {
  }


  bool FindFirst(char *name)
  {
    bool ret=false;

    #ifdef WIN32
    hFindNext = FindFirstFileA(mSearchName, &finddata);
    if ( hFindNext == INVALID_HANDLE_VALUE )
      ret =  false;
     else
     {
       bFound = 1; // have an initial file to check.
       ret =  FindNext(name);
     }
     #endif
     #ifdef LINUX_GENERIC
     mDir = opendir(".");
     ret = FindNext(name);
    #endif
    return ret;
  }

  bool FindNext(char *name)
  {
    bool ret = false;

    #ifdef WIN32
    while ( bFound )
    {
      bFound = FindNextFileA(hFindNext, &finddata);
      if ( bFound && (finddata.cFileName[0] != '.') && !(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
      {
        strncpy(name,finddata.cFileName,MAXNAME);
        ret = true;
        break;
      }
    }
    if ( !ret )
    {
      bFound = 0;
      FindClose(hFindNext);
    }
    #endif

    #ifdef LINUX_GENERIC

    if ( mDir )
    {
      while ( 1 )
      {

        struct direct *di = readdir( mDir );

        if ( !di )
        {
          closedir( mDir );
          mDir = 0;
          ret = false;
          break;
        }

        if ( strcmp(di->d_name,".") == 0 || strcmp(di->d_name,"..") == 0 )
        {
          // skip it!
        }
        else
        {
          strncpy(name,di->d_name,MAXNAME);
          ret = true;
          break;
        }
      }
    }
    #endif
    return ret;
  }

private:
  char mSearchName[MAXNAME];
#ifdef WIN32
  WIN32_FIND_DATAA finddata;
  HANDLE hFindNext;
  PxI32 bFound;
#endif
#ifdef LINUX_GENERIC
  DIR      *mDir;
#endif
};

}; // end of namespace

namespace NVSHARE
{

	using namespace MESHIMPORT_NVSHARE;

static const char *lastSlash(const char *foo)
{
  const char *ret = foo;
  const char *last_slash = 0;

  while ( *foo )
  {
    if ( *foo == '\\' ) last_slash = foo;
    if ( *foo == '/' ) last_slash = foo;
    if ( *foo == ':' ) last_slash = foo;
    foo++;
  }
  if ( last_slash ) ret = last_slash+1;
  return ret;
}

NVSHARE::MeshImport * loadMeshImporters(const char * directory) // loads the mesh import library (dll) and all available importers from the same directory.
{
  NVSHARE::MeshImport *ret = 0;

  char scratch[512];
  if ( directory && strlen(directory) )
  {
    sprintf(scratch,"%s\\MeshImport.dll", directory);
  }
  else
  {
    strcpy(scratch,"MeshImport.dll");
  }

#ifdef WIN32
  ret = (NVSHARE::MeshImport *)getMeshBindingInterface(scratch,MESHIMPORT_VERSION);
#else
  ret = 0;
#endif

  if ( ret )
  {
    NVSHARE::FileFind ff(directory,"MeshImport*.dll");
    char name[MAXNAME];
    if ( ff.FindFirst(name) )
    {
      do
      {
        const char *scan = lastSlash(name);
        if ( stricmp(scan,"MeshImport.dll") == 0 )
        {
          printf("Skipping 'MeshImport.dll'\r\n");
        }
        else
        {
          printf("Loading '%s'\r\n", scan );
		  const char *fname;

		  if ( directory && strlen(directory) )
		  {
			  sprintf(scratch,"%s%s", directory, scan);
			  fname = scratch;
		  }
		  else
		  {
			  fname = name;
		  }

#ifdef WIN32
          NVSHARE::MeshImporter *imp = (NVSHARE::MeshImporter *)getMeshBindingInterface(fname,MESHIMPORT_VERSION);
#else
		  NVSHARE::MeshImporter *imp = 0;
#endif
          if ( imp )
          {
            ret->addImporter(imp);
            printf("Added importer '%s'\r\n", name );
          }
        }
      } while ( ff.FindNext(name) );
    }
  }
  return ret;
}

}; // end of namespace