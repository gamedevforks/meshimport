#ifndef MESHIMPORTLEVELLER_H
#define MESHIMPORTLEVELLER_H

#include "../MeshImport/MeshImport.h"

namespace MESHIMPORTLEVELLER
{
/*!
	* \brief
	* The MeshImportLeveller plugin is used for...
	*
	* The services provided are:
	*
	* \remarks
	* The MeshImportLeveller plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
  class MeshImportLeveller : public MESHIMPORT::MeshImporter
{
public:
};

}; // End of namespace for MESHIMPORTLEVELLER

#define MESHIMPORTLEVELLER_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORTLEVELLER::MeshImportLeveller *gMeshImportLeveller; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.



#endif
