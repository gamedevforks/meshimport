#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <direct.h>

#include "MeshImport/MeshImport.h"

void main(int argc,const char **argv)
{
  if ( argc == 2 )
  {
    char dirname[256];
    strcpy(dirname,argv[0]);
    int len = strlen(dirname);
    char *scan = &dirname[len-1];
    while ( len )
    {
      if ( *scan == '\\' )
      {
        *scan = 0;
        break;
      }
      scan--;
      len--;
    }

    chdir( dirname );


    gMeshImport = loadMeshImporters(dirname,0);

    if ( gMeshImport )
    {
      unsigned char *data=0;
      unsigned int len = 0;
      const char *fname = argv[1];
      FILE *fph = fopen(fname,"rb");
      if ( fph )
      {
        fseek(fph,0L,SEEK_END);
        len = ftell(fph);
        fseek(fph,0L,SEEK_SET);
        if ( len > 0 )
        {
          data = new unsigned char[len];
          fread(data,len,1,fph);
        }
        fclose(fph);
      }
      if ( data )
      {
        MESHIMPORT::MeshSystemContainer *msc = gMeshImport->createMeshSystemContainer(fname,data,len,0);
        if ( msc )
        {
          printf("Success!\r\n");
          MESHIMPORT::MeshSystem *ms = gMeshImport->getMeshSystem(msc);
          MESHIMPORT::MeshSerialize s(MESHIMPORT::MSF_EZMESH);
          bool ok = gMeshImport->serializeMeshSystem(ms,s);
          if ( ok )
          {
            printf("Saving serialized data as temp.ezm\r\n");
            FILE *fph = fopen("temp.ezm", "wb");
            if ( fph )
            {
              fwrite(s.mBaseData,s.mBaseLen,1,fph);
              fclose(fph);
            }
            else
            {
              printf("Failed to open file for write access.\r\n");
            }
            gMeshImport->releaseSerializeMemory(s);
          }
          gMeshImport->releaseMeshSystemContainer(msc);
        }
        else
        {
          printf("Failed to create MeshSystem for '%s'\r\n", fname );
        }
        delete []data;
      }
      else
      {
        printf("Failed to load file '%s'\r\n", fname );
      }
    }

  }
  else
  {
    printf("Usage: MeshImport <fname>\r\n");
  }
}
