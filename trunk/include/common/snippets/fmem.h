#ifndef FMEM_H

#define FMEM_H

/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <stdio.h>
#include <string.h>

#include "common/snippets/UserMemAlloc.h"

#pragma warning(push)
#pragma warning(disable:4996)

inline unsigned char *getLocalFile(const char *filename,unsigned int &len)
{
  unsigned char * ret = 0;
  FILE *fph = fopen(filename,"rb");
  if ( fph )
  {
    fseek(fph,0L,SEEK_END);
    len = ftell(fph);
    fseek(fph,0L,SEEK_SET);
    unsigned char *mem = MEMALLOC_NEW_ARRAY(unsigned char,len)[len];
    unsigned int l = fread(mem,len,1,fph);
    if ( l == 0 )
    {
      delete mem;
    }
    else
    {
      ret = mem;
    }
    fclose(fph);
  }
  return ret;
}

inline void releaseFmem(unsigned char *mem)
{
  delete []mem;
}

inline bool putLocalFile(const char *filename,const void *mem,unsigned int len)
{
  bool ret = false;

  char scratch[512];
  strncpy(scratch,filename,512);
  char *scan = scratch;
  while ( *scan )
  {
    if ( *scan == ':' ) *scan = '@';
    if ( *scan == '/' ) *scan = '_';
    if ( *scan == '\\' ) *scan = '_';
    scan++;
  }
  FILE *fph = fopen(scratch,"wb");
  if ( fph )
  {
    fwrite(mem,len,1,fph);
    fclose(fph);
    ret = true;
  }
  return ret;
}

#pragma warning(pop)

#endif
