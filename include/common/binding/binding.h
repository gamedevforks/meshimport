/* HeroEngine, Copyright 2002-2008 Simutronics, Corp. All rights reserved. */


/*!
 * \file binding.h
 *
 * \brief
 * Write brief comment for binding.h here.
 *
 * Write detailed description for binding.h here.
 */

#ifndef BINDING_H

#define BINDING_H

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
void *     getBindingInterfaceModule(const char *dll,int version_number,void *&module);
void *     getBindingInterface(const char *dll,int version_number);
bool       unloadBindingInterface(void *module);

void       setSuppressLoadError(bool state); // whether or not to suppress load error dialog boxes under windows.

void * aaGetBindingInterface(const char *dll,int version_number,void *(&module));
void *  reGetBindingInterface(const char *dll,int version_number,void *(&module));
};


#endif
