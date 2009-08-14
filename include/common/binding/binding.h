/* HeroEngine, Copyright 2002-2008 Simutronics, Corp. All rights reserved. */


/*!
 * \file binding.h
 *
 * \brief
 * Write brief comment for binding.h here.
 *
 * Write detailed description for binding.h here.
 */

#if defined(LINUX)
#include "linux_compat.h"
#endif

#ifndef BINDING_H

#define BINDING_H

#include "UserMemAlloc.h"


namespace SYSTEM_SERVICES
{
class SystemServices;
};

/*!
* \brief
* function type used by interface registration
*/
typedef void * (__cdecl * PLUGIN_INTERFACE_FUNC)     (NxI32 version,SYSTEM_SERVICES::SystemServices *services);

/*!
* \brief
* Structure used pass the names of plug-in interfaces exported by a dll or library
*/
struct INTERFACE_EXPORT
{
  const char *name;
  PLUGIN_INTERFACE_FUNC func;
};

typedef NxI32    (__cdecl * PLUGIN_INTERFACE_LIST_FUNC)(const INTERFACE_EXPORT **);



extern "C"
{


/*!
 * \brief
 * Write brief comment for getBindingInterface here.
 *
 * \param dll
 * Description of parameter dll.
 *
 * \param version_number
 * Description of parameter version_number.
 *
 * \returns
 * Write description of return value here.
 *
 * \throws <exception class>
 * Description of criteria for throwing this exception.
 *
 * Write detailed description for getBindingInterface here.
 *
 * \remarks
 * Write remarks for getBindingInterface here.
 *
 * \see
 * Separate items with the '|' character.
 */
void       loadModuleInterfaces(const char *dll, void **rmodule = 0);
void *     getBindingInterface(const char *dll, const char *name, NxI32 version_number, SYSTEM_SERVICES::SystemServices *services, void **rmodule = 0); 
bool       unloadModule(void *module);


void       setSuppressLoadError(bool state); // whether or not to suppress load error dialog boxes under windows.

void * aaGetBindingInterface(const char *dll,NxI32 version_number,void *(&module));
void *  reGetBindingInterface(const char *dll,NxI32 version_number,void *(&module));
};


#endif
