#ifndef MESHIMPORTFBX_H
#define MESHIMPORTFBX_H

#include "../MeshImport/MeshImport.h"

namespace MESHIMPORTFBX
{
/*!
	* \brief
	* The MeshImportFbx plugin is used for...
	*
	* The services provided are:
	*
	* \remarks
	* The MeshImportFbx plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
  class MeshImportFbx : public MESHIMPORT::MeshImporter
{
public:
};

}; // End of namespace for MESHIMPORTFBX

#define MESHIMPORTFBX_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORTFBX::MeshImportFbx *gMeshImportFbx; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.



#endif
