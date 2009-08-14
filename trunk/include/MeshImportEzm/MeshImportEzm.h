#ifndef MESHIMPORTEZM_H
#define MESHIMPORTEZM_H

#include "MeshImport.h"

namespace MESHIMPORTEZM
{
/*!
	* \brief
	* The MeshImportEzm plugin is used for...
	*
	* The services provided are:
	*
	* \remarks
	* The MeshImportEzm plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
  class MeshImportEzm : public MESHIMPORT::MeshImporter
{
public:
};

}; // End of namespace for MESHIMPORTEZM

#define MESHIMPORTEZM_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORTEZM::MeshImportEzm *gMeshImportEzm; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.



#endif
