#ifndef IMPORT_PSK_H

#define IMPORT_PSK_H

#include "UserMemAlloc.h"

namespace NVSHARE
{

class MeshImporter;

MeshImporter * createMeshImportPSK(void);
void           releaseMeshImportPSK(MeshImporter *iface);

};

#endif
