#ifndef SUTIL_H

#define SUTIL_H

#include "UserMemAlloc.h"

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
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
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


namespace NVSHARE
{

char *         stristr(const char *str,const char *key);       // case insensitive str str
bool           isstristr(const char *str,const char *key);     // bool true/false based on case insenstive strstr
NxU32   GetHEX(const char *foo,const char **next=0);
NxU8  GetHEX1(const char *foo,const char **next=0);
NxU16 GetHEX2(const char *foo,const char **next=0);
NxU32   GetHEX4(const char *foo,const char **next=0);
NxF32          GetFloatValue(const char *str,const char **next=0);
NxI32            GetIntValue(const char *str,const char **next=0);
const char *   SkipWhitespace(const char *str);
bool           IsWhitespace(char c);
const char *   FloatString(NxF32 v,bool binary=false);
const char *   GetTrueFalse(NxU32 state);
bool           CharToWide(const char *source,wchar_t *dest,NxI32 maxlen);
bool           WideToChar(const wchar_t *source,char *dest,NxI32 maxlen);
const char **  GetArgs(char *str,NxI32 &count); // destructable parser, stomps EOS markers on the input string!
NxI32            GetUserArgs(const char *us,const char *key,const char **args);
bool           GetUserSetting(const char *us,const char *key,NxI32 &v);
bool           GetUserSetting(const char *us,const char *key,const char * &v);
const char *   GetRootName(const char *fname); // strip off everything but the 'root' file name.
bool           IsTrueFalse(const char *c);
bool           IsDirectory(const char *fname,char *path,char *basename,char *postfix);
bool           hasSpace(const char *str); // true if the string contains a space
const char *   lastDot(const char *src);
const char *   lastSlash(const char *src); // last forward or backward slash character, null if none found.
const char *   lastChar(const char *src,char c);
const char  	*fstring(NxF32 v);
const char *   formatNumber(NxI32 number); // JWR  format this integer into a fancy comma delimited string
bool           fqnMatch(const char *n1,const char *n2); // returns true if two fully specified file names are 'the same' but ignores case sensitivty and treats either a forward or backslash as the same character.
bool           getBool(const char *str);
bool           needsQuote(const char *str); // if this string needs quotes around it (spaces, commas, #, etc)

#define MAX_FQN 512 // maximum size of an FQN string
void           normalizeFQN(const wchar_t *source,wchar_t *dest);
void           normalizeFQN(const char *source,char *dest);
bool           endsWith(const char *str,const char *ends,bool caseSensitive);

}; // end of namespace

#endif
