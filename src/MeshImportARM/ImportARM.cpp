#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "UserMemAlloc.h"
#include "FloatMath.h"
#include "MeshImport.h"
#include "UserMemAlloc.h"
#include "sutil.h"
#include "ImportARM.h"

namespace NVSHARE
{

class MeshImporterARM : public MeshImporter, public Memalloc
{
public:
  	virtual NxI32              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.
  	virtual const char *     getExtension(NxI32 index)  // report the default file name extension for this mesh type.
  	{
  		return ".xml";
	}

  	virtual const char *     getDescription(NxI32 index) // report the ascii description of the import type.
  	{
  		return "APEX Render Mesh";
	}

  	virtual bool             importMesh(const char *meshName,const void *data,NxU32 dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  	{
  		bool ret = false;
  		return ret;
	}

};


MeshImporter * createMeshImportARM(void)
{
    MeshImporterARM *m = MEMALLOC_NEW(MeshImporterARM);
    return static_cast< MeshImporter *>(m);
}

void           releaseMeshImportARM(MeshImporter *iface)
{
    MeshImporterARM *p = static_cast< MeshImporterARM *>(iface);
    delete p;
}


};  // end of namespace
