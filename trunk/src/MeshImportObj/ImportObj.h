#ifndef IMPORT_OBJ_H

#define IMPORT_OBJ_H


namespace MESHIMPORT
{

class MeshImporter;

MeshImporter * createMeshImportObj(void);
void         releaseMeshImportObj(MeshImporter *iface);

}; // end of namespace



#endif
