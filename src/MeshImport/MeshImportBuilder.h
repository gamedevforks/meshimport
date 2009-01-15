#ifndef MESH_IMPORT_BUILDER_H

#define MESH_IMPORT_BUILDER_H

#include "MeshImport/MeshImport.h"

#pragma warning(disable:4100)

namespace MESHIMPORT
{

class MeshBuilder : public MeshSystem
{
public:
};


MeshBuilder * createMeshBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options);
void          releaseMeshBuilder(MeshBuilder *m);

};

#endif
