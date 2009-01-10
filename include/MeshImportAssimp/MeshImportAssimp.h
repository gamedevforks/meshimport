#ifndef MESHIMPORTASSIMP_H
#define MESHIMPORTASSIMP_H

#include "../MeshImport/MeshImport.h"

namespace MESHIMPORTASSIMP
{
/*!
	* \brief
	* The MeshImportAssimp plugin is used for...
	*
	* The services provided are:
	*
	* \remarks
	* The MeshImportAssimp plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
  class MeshImportAssimp : public MESHIMPORT::MeshImporter
{
public:
};

}; // End of namespace for MESHIMPORTASSIMP

#define MESHIMPORTASSIMP_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORTASSIMP::MeshImportAssimp *gMeshImportAssimp; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.



#endif
