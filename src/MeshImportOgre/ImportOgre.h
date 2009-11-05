#ifndef IMPORT_OGRE_H

#define IMPORT_OGRE_H

#include "UserMemAlloc.h"

namespace NVSHARE
{

class MeshImporter;

MeshImporter * createMeshImportOgre(void);
void           releaseMeshImportOgre(MeshImporter *iface);

};


#endif
