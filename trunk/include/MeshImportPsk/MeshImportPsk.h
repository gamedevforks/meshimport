#ifndef MESHIMPORTPSK_H
#define MESHIMPORTPSK_H

#include "../MeshImport/MeshImport.h"

namespace MESHIMPORTPSK
{
/*!
	* \brief
	* The MeshImportPsk plugin is used for...
	*
	* The services provided are:
	*
	* \remarks
	* The MeshImportPsk plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
  class MeshImportPsk : public MESHIMPORT::MeshImporter
{
public:
};

}; // End of namespace for MESHIMPORTPSK

#define MESHIMPORTPSK_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORTPSK::MeshImportPsk *gMeshImportPsk; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.



#endif
