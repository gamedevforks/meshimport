#ifndef IMPORT_OBJ_H

#define IMPORT_OBJ_H

#include "UserMemAlloc.h"

namespace NVSHARE
{

class MeshImporter;

MeshImporter * createMeshImportObj(void);
void         releaseMeshImportObj(MeshImporter *iface);

}; // end of namespace



#endif
