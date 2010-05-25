#ifndef MESH_IMPORT_BUILDER_H

#define MESH_IMPORT_BUILDER_H

#include "MeshImport.h"
#include "UserMemAlloc.h"

namespace NVSHARE
{

class KeyValueIni;

class MeshBuilder : public MeshSystem, public MeshImportInterface
{
public:
  virtual void gather(void) = 0;
  virtual void scale(PxF32 s) = 0;
  virtual void rotate(PxF32 rotX,PxF32 rotY,PxF32 rotZ) = 0;


};

MeshBuilder * createMeshBuilder(KeyValueIni *ini,
                                const char *meshName,
                                const void *data,
                                PxU32 dlen,
                                MeshImporter *mi,
                                const char *options,
                                MeshImportApplicationResource *appResource);

MeshBuilder * createMeshBuilder(MeshImportApplicationResource *appResource);
void          releaseMeshBuilder(MeshBuilder *m);

}; // end of namespace

#endif
