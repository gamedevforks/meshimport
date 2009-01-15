#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <direct.h>

#include "common/binding/binding.h"
#include "MeshImport/MeshImport.h"
#include "common/snippets/fmem.h"

#ifndef OPEN_SOURCE
#include "MeshImportGranny/MeshImportGranny.h"
#include "MeshImportSpeedTree/MeshImportSpeedTree.h"
#include "MeshImportHeTerrain/MeshImportHeTerrain.h"
#include "MeshImportHeWater/MeshImportHeWater.h"
#endif

#include "MeshImportEzm/MeshImportEzm.h"
#include "MeshImportFbx/MeshImportFbx.h"
#include "MeshImportLeveller/MeshImportLeveller.h"
#include "MeshImportObj/MeshImportObj.h"
#include "MeshImportPsk/MeshImportPsk.h"
#include "MeshImportAssimp/MeshImportAssimp.h"
#include "MeshImportOgre/MeshImportOgre.h"

MESHIMPORT::MeshImport                         *gMeshImport=0;

#ifndef OPEN_SOURCE
MESHIMPORTGRANNY::MeshImportGranny             *gMeshImportGranny=0;
MESHIMPORTSPEEDTREE::MeshImportSpeedTree       *gMeshImportSpeedTree=0;
MESHIMPORTHETERRAIN::MeshImportHeTerrain       *gMeshImportHeTerrain=0;
MESHIMPORTHEWATER::MeshImportHeWater           *gMeshImportHeWater=0;
#endif

MESHIMPORTEZM::MeshImportEzm                   *gMeshImportEzm=0;
MESHIMPORTFBX::MeshImportFbx                   *gMeshImportFbx=0;
MESHIMPORTLEVELLER::MeshImportLeveller         *gMeshImportLeveller=0;
MESHIMPORTOBJ::MeshImportObj                   *gMeshImportObj=0;
MESHIMPORTPSK::MeshImportPsk                   *gMeshImportPsk=0;
MESHIMPORTASSIMP::MeshImportAssimp             *gMeshImportAssimp=0;
MESHIMPORTOGRE::MeshImportOgre                 *gMeshImportOgre=0;

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


    printf("Loading 'MeshImport.dll' : The MeshImporter manager.\r\n");
    gMeshImport            = (MESHIMPORT::MeshImport *)                  getBindingInterface("MeshImport.dll",         MESHIMPORT_VERSION);

  #ifndef OPEN_SOURCE
    printf("Loading MeshImportGranny.dll : Support for Rad Game Tools Granny files. Must be a Granny licensee to use. (.GR2)\r\n");
    gMeshImportGranny      = (MESHIMPORTGRANNY::MeshImportGranny *)      getBindingInterface("MeshImportGranny.dll",   MESHIMPORTGRANNY_VERSION);

    printf("Loading MeshImportSpeedTree.dll : Support for SpeedTree files Must be a SpeedTree licensee to use. (.SPT)\r\n");
    gMeshImportSpeedTree   = (MESHIMPORTSPEEDTREE::MeshImportSpeedTree *)getBindingInterface("MeshImportSpeedTree.dll",MESHIMPORTSPEEDTREE_VERSION);

    printf("Loading MeshImportHeTerrain.dll : Support for HeroEngine terrain (Must be a HeroEngine licensee) (.HMS)\r\n");
    gMeshImportHeTerrain   = (MESHIMPORTHETERRAIN::MeshImportHeTerrain *)getBindingInterface("MeshImportHeTerrain.dll", MESHIMPORTHETERRAIN_VERSION);

    printf("Loading MeshImportWater.dll : Support for HeroEngine water (Must be a HeroEngine licensee) (.MIR)\r\n");
    gMeshImportHeWater     = (MESHIMPORTHEWATER::MeshImportHeWater *)    getBindingInterface("MeshImportHeWater.dll",  MESHIMPORTHEWATER_VERSION);

  #endif
    printf("Loading MeshImportEzm.dll : Support for the Ageia Rocket Ez-Mesh format (.EZM)\r\n");
    gMeshImportEzm         = (MESHIMPORTEZM::MeshImportEzm *)            getBindingInterface("MeshImportEzm.dll",      MESHIMPORTEZM_VERSION);

    printf("Loading MeshImportFbx.dll : Support for the AutoDesk FBX file format. (.FBX)\r\n");
    gMeshImportFbx         = (MESHIMPORTFBX::MeshImportFbx *)            getBindingInterface("MeshImportFbx.dll",      MESHIMPORTFBX_VERSION);

    printf("Loading MeshImportLeveller.dll : Support for floating point heightfields in the Leveller file format. (.TER) \r\n");
    gMeshImportLeveller    = (MESHIMPORTLEVELLER::MeshImportLeveller *)       getBindingInterface("MeshImportLeveller.dll", MESHIMPORTLEVELLER_VERSION);

    printf("Loading MeshImportObj.dll : Support for Alias Wavefront OBJ files. (.OBJ)\r\n");
    gMeshImportObj         = (MESHIMPORTOBJ::MeshImportObj *)            getBindingInterface("MeshImportObj.dll",      MESHIMPORTOBJ_VERSION);

    printf("Loading MeshImportPsk.dll : Support for the legacy Unreal character model format. (.PSK)\r\n");
    gMeshImportPsk         = (MESHIMPORTPSK::MeshImportPsk *)            getBindingInterface("MeshImportPsk.dll",      MESHIMPORTPSK_VERSION);

    printf("Loading MeshImportOgre.dll : Support for the Ogre3d XML format. (.XML)\r\n");
    gMeshImportOgre        = (MESHIMPORTOGRE::MeshImportOgre *)            getBindingInterface("MeshImportOgre.dll",      MESHIMPORTOGRE_VERSION);

    printf("Loading MeshImportAssimp.dll : Support for the OpenAssetImporter library.  Supports many file formats.\r\n");
    gMeshImportAssimp      = (MESHIMPORTASSIMP::MeshImportAssimp *)      getBindingInterface("MeshImportAssimp.dll",   MESHIMPORTASSIMP_VERSION);

    if ( gMeshImport )
    {
  #ifndef OPEN_SOURCE
      gMeshImport->addImporter(gMeshImportGranny);
      gMeshImport->addImporter(gMeshImportSpeedTree);
      gMeshImport->addImporter(gMeshImportHeTerrain);
      gMeshImport->addImporter(gMeshImportHeWater);
  #endif
      gMeshImport->addImporter(gMeshImportEzm);
      gMeshImport->addImporter(gMeshImportFbx);
      gMeshImport->addImporter(gMeshImportLeveller);
      gMeshImport->addImporter(gMeshImportObj);
      gMeshImport->addImporter(gMeshImportPsk);
			gMeshImport->addImporter(gMeshImportOgre);
      gMeshImport->addImporter(gMeshImportAssimp);
    }

    if ( !gMeshImport ) printf("Failed to load 'MeshImport.dll'\r\n");
  #ifndef OPEN_SOURCE
    if ( !gMeshImportGranny ) printf("Failed to load 'MeshImportGranny.dll\r\n");
    if ( !gMeshImportSpeedTree ) printf("Failed to load 'MeshImportSpeedTree.dll\r\n");
    if ( !gMeshImportHeTerrain ) printf("Failed to load 'MeshImportHeTerrain.dll\r\n");
    if ( !gMeshImportHeWater ) printf("Failed to load 'MeshImportHeWater.dll\r\n");
  #endif
    if ( !gMeshImportEzm ) printf("Failed to load 'MeshImportEzm.dll\r\n");
    if ( !gMeshImportFbx ) printf("Failed to load 'MeshImportFbx.dll\r\n");
    if ( !gMeshImportLeveller ) printf("Failed to load 'MeshImportLeveller.dll\r\n");
    if ( !gMeshImportObj ) printf("Failed to load 'MeshImportObj.dll\r\n");
    if ( !gMeshImportPsk ) printf("Failed to load 'MeshImportPsk.dll\r\n");
    if ( !gMeshImportOgre ) printf("Failed to load 'MeshImportOgre.dll\r\n");
    if ( !gMeshImportAssimp ) printf("Failed to load 'MeshImportAssimp.dll\r\n");

    if ( gMeshImport )
    {
      const char *fname = argv[1];
      unsigned int len;
      unsigned char *data = getLocalFile(fname,len);
      if ( data )
      {
        MESHIMPORT::MeshSystem *ms = gMeshImport->createMeshSystem(fname,data,len,0);
        if ( ms )
        {
          printf("Success!\r\n");

          unsigned int olen;
          void *data = gMeshImport->serializeMeshSystem(ms,olen,MESHIMPORT::MSF_EZMESH);
          if ( data )
          {
            printf("Saving serialized data as temp.ezm\r\n");
            FILE *fph = fopen("temp.ezm", "wb");
            if ( fph )
            {
              fwrite(data,olen,1,fph);
              fclose(fph);
            }
            else
            {
              printf("Failed to open file for write access.\r\n");
            }
            gMeshImport->releaseSerializeMemory(data);
          }
          gMeshImport->releaseMeshSystem(ms);
        }
        else
        {
          printf("Failed to create MeshSystem for '%s'\r\n", fname );
        }
        releaseFmem(data);
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
