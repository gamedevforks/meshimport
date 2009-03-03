#ifndef MESH_IMPORT_BUILDER_H

#define MESH_IMPORT_BUILDER_H

#include "MeshImport/MeshImport.h"

#pragma warning(disable:4100)

namespace MESHIMPORT
{

class MeshBuilder : public MeshSystem, public MeshImportInterface
{
public:
  virtual void gather(void) = 0;
  virtual void scale(float s) = 0;


};


MeshBuilder * createMeshBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options,MeshImportApplicationResource *appResource);
MeshBuilder * createMeshBuilder(MeshImportApplicationResource *appResource);
void          releaseMeshBuilder(MeshBuilder *m);

};

#endif
