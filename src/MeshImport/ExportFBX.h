#ifndef EXPORT_FBX_H

#define EXPORT_FBX_H

#include "FileInterface.h"

namespace NVSHARE
{

class MeshSystem;

void serializeFBX(FILE_INTERFACE *fph,MeshSystem *ms);

};// end of namespace

#endif
