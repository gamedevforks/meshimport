#ifndef MESHIMPORTOBJ_H
#define MESHIMPORTOBJ_H

#include "MeshImport.h"

namespace MESHIMPORTOBJ
{
/*!
	* \brief
	* The MeshImportObj plugin is used for...
	*
	* The services provided are:
	*
	* \remarks
	* The MeshImportObj plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
  class MeshImportObj : public MESHIMPORT::MeshImporter
{
public:
};

}; // End of namespace for MESHIMPORTOBJ

#define MESHIMPORTOBJ_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORTOBJ::MeshImportObj *gMeshImportObj; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.



#endif
