#ifndef IMPORT_PSK_H

#define IMPORT_PSK_H


namespace MESHIMPORT
{

class MeshImporter;

MeshImporter * createMeshImportPSK(void);
void           releaseMeshImportPSK(MeshImporter *iface);

};

#endif
