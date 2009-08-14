#ifndef MESHIMPORTOGRE_H
#define MESHIMPORTOGRE_H

#include "MeshImport.h"

namespace MESHIMPORTOGRE
{
/*!
	* \brief
	* The MeshImportOgre plugin is used for...
	*
	* The services provided are:
	*
	* \remarks
	* The MeshImportOgre plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
  class MeshImportOgre : public MESHIMPORT::MeshImporter
{
public:
};

}; // End of namespace for MESHIMPORTOGRE

#define MESHIMPORTOGRE_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORTOGRE::MeshImportOgre *gMeshImportOgre; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.



#endif
