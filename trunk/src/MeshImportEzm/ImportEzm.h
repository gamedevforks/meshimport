#ifndef IMPORT_EZM_H

#define IMPORT_EZM_H

namespace MESHIMPORT
{

class MeshImporter;

MeshImporter * createMeshImportEZM(void);
void         releaseMeshImportEZM(MeshImporter *iface);

}; // end of namespace

#endif
