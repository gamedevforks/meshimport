#ifndef IMPORT_ARM_H

#define IMPORT_ARM_H

#include "UserMemAlloc.h"

namespace NVSHARE
{

class MeshImporter;

MeshImporter * createMeshImportARM(void);
void           releaseMeshImportARM(MeshImporter *iface);

};

#endif
