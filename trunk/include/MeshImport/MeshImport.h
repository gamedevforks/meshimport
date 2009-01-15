#ifndef MESHIMPORT_H
#define MESHIMPORT_H

#include "MeshImport/MeshSystem.h"
#include "MeshImport/MeshImport.h"

#pragma warning(push)
#pragma warning(disable:4996)


// MeshImporters to write:  Wavefront OBJ
//                          EZ-Mesh
//                          Ogre3d
//                          Unreal PSK
//                          Granny
//                          SpeedTree
//                          HeroEngine terrain
//                          HeroEngine water
//                          Leveller heightfields using RTIN
//

namespace MESHIMPORT
{

class MeshImportInterface
{
public:
  virtual void        importMaterial(const char *matName,const char *metaData) = 0;        // one material
  virtual void        importUserData(const char *userKey,const char *userValue) = 0;       // carry along raw user data as ASCII strings only..
  virtual void        importUserBinaryData(const char *name,unsigned int len,const unsigned char *data) = 0;
  virtual void        importTetraMesh(const char *tetraName,const char *meshName,unsigned int tcount,const float *tetraData) = 0;

  virtual void        importAssetName(const char *assetName,const char *info) = 0;         // name of the overall asset.
  virtual void        importMesh(const char *meshName,const char *skeletonName) = 0;       // name of a mesh and the skeleton it refers to.

  virtual void        importTriangle(const char *meshName,
                                     const char *materialName,
                                     unsigned int vertexFlags,
                                     const MeshVertex verts[3]) = 0;

  virtual void        importIndexedTriangleList(const char *meshName,
                                                const char *materialName,
                                                unsigned int vertexFlags,
                                                unsigned int vcount,
                                                const MeshVertex *vertices,
                                                unsigned int tcount,
                                                const unsigned int *indices) = 0;

  virtual void        importAnimation(const MeshAnimation &animation) = 0;
  virtual void        importSkeleton(const MeshSkeleton &skeleton) = 0;
  virtual void        importRawTexture(const char *textureName,const unsigned char *pixels,unsigned int wid,unsigned int hit) = 0;
  virtual void        importMeshInstance(const char *meshName,const float pos[3],const float rotation[4],const float scale[3])= 0;

};



class MeshImporter
{
public:
  virtual int              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.
  virtual const char *     getExtension(int index=0) = 0; // report the default file name extension for this mesh type.
  virtual const char *     getDescription(int index=0) = 0; // report the ascii description of the import type.

  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options) = 0;
};

enum MeshSerializeFormat
{
  MSF_EZMESH, // save it back out into ez-mesh, lossless XML format.
};

class MeshImport
{
public:
  virtual void             addImporter(MeshImporter *importer) = 0; // add an additional importer

  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options) = 0;
  virtual MeshSystem *     createMeshSystem(const char *meshName,const void *data,unsigned int dlen,const char *options) = 0; // imports and converts to a single MeshSystem data structure
  virtual void            *serializeMeshSystem(MeshSystem *mesh,unsigned int &len,MeshSerializeFormat format) = 0;
  virtual void             releaseSerializeMemory(void *data) = 0;
  virtual void             releaseMeshSystem(MeshSystem *mesh) = 0;

  virtual int              getImporterCount(void) = 0;
  virtual MeshImporter    *getImporter(int index) = 0;

  virtual MeshImporter *   locateMeshImporter(const char *fname) = 0; // based on this file name, find a matching mesh importer.
};

}; // End of namespace for MESHIMPORT

#define MESHIMPORT_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORT::MeshImport *gMeshImport; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.

#pragma warning(pop)

#endif
