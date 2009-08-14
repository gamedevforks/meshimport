#ifndef IMPORT_OGRE_H

#define IMPORT_OGRE_H

namespace MESHIMPORT
{

class MeshImporter;

MeshImporter * createMeshImportOgre(void);
void           releaseMeshImportOgre(MeshImporter *iface);

};


#endif
