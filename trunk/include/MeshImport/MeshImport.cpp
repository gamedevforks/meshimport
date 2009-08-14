#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImport.h"

MESHIMPORT::MeshImport *gMeshImport=0; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.
MESHIMPORT::CommLayer *gCommLayer=0;

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>

void *getBindingInterface(const char *dll,NxI32 version_number,SYSTEM_SERVICES::SystemServices *services) // loads the tetra maker DLL and returns the interface pointer.
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
      typedef void * (__cdecl * NX_GetToolkit)(NxI32 version,SYSTEM_SERVICES::SystemServices *services);
      ret = ((NX_GetToolkit)proc)(version_number,services);
    }
  }
  return ret;
}



#endif

#ifdef LINUX_GENERIC
#include <sys/types.h>
#include <sys/dir.h>
#endif

#define MAXNAME 512

namespace MESHIMPORT
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
    bool ret;

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
  NxI32 bFound;
#endif
#ifdef LINUX_GENERIC
  DIR      *mDir;
#endif
};

}; // end of namespace

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

MESHIMPORT::MeshImport * loadMeshImporters(const char * directory,SYSTEM_SERVICES::SystemServices *services) // loads the mesh import library (dll) and all available importers from the same directory.
{
  MESHIMPORT::MeshImport *ret = 0;

  char scratch[512];
  if ( directory && strlen(directory) )
  {
    sprintf(scratch,"%s\\MeshImport.dll", directory);
  }
  else
  {
    strcpy(scratch,"MeshImport.dll");
  }

  ret = (MESHIMPORT::MeshImport *)getBindingInterface(scratch,MESHIMPORT_VERSION,services);

  if ( ret )
  {
    MESHIMPORT::FileFind ff(directory,"MeshImport*.dll");
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
          MESHIMPORT::MeshImporter *imp = (MESHIMPORT::MeshImporter *)getBindingInterface(name,MESHIMPORT_VERSION,services);
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
