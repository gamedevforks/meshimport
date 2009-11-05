#ifndef COMM_LAYER_H

#define COMM_LAYER_H

#include "UserMemAlloc.h"
#include "MeshImport.h"

namespace NVSHARE
{

CommLayer *      CreateCommLayerTelent(const char *address="LOCALHOST",NxU32 port=23);
CommLayer *      CreateCommLayerWindowsMessage(const char *appName,const char *destApp);
void             ReleaseCommLayer(CommLayer *t);

};

#endif
