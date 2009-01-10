#ifndef MESH_IMPORT_BUILDER_H

#define MESH_IMPORT_BUILDER_H

#include "MeshImport/MeshImport.h"

#pragma warning(disable:4100)

namespace MESHIMPORT
{

class MeshImportBuilder : public MeshSystem
{
public:
};


MeshImportBuilder * createMeshImportBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options);
void          releaseMeshImportBuilder(MeshImportBuilder *m);

};

#endif
