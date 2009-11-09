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
#pragma warning(disable:4996)
#include "safestdio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>

#include "inparser.h"
#include "sutil.h"

#if defined(LINUX) || defined(PLAYSTATION3)
#include <math.h>
#include "linux_compat.h"
#endif

namespace NVSHARE
{

static char ToLower(char c)
{
	if ( c >= 'A' && c <= 'Z' ) c+=32;
	return c;
}

char *stristr(const char *str,const char *key)       // case insensitive str str
{
	assert( strlen(str) < 2048 );
	assert( strlen(key) < 2048 );
	char istr[2048];
	char ikey[2048];
	strncpy(istr,str,2048);
	strncpy(ikey,key,2048);
	strlwr(istr);
	strlwr(ikey);

	char *foo = strstr(istr,ikey);
	if ( foo )
	{
		NxU32 loc = (NxU32)(foo - istr);
		foo = (char *)str+loc;
	}

	return foo;
}

bool        isstristr(const char *str,const char *key)     // bool true/false based on case insenstive strstr
{
	bool ret = false;
	const char *found = strstr(str,key);
	if ( found ) ret = true;
	return ret;
}

NxU32 GetHex(NxU8 c)
{
	NxU32 v = 0;
	c = ToLower(c);
	if ( c >= '0' && c <= '9' )
		v = c-'0';
	else
	{
		if ( c >= 'a' && c <= 'f' )
		{
			v = 10 + c-'a';
		}
	}
	return v;
}

NxU8 GetHEX1(const char *foo,const char **endptr)
{
	NxU32 ret = 0;

	ret = (GetHex(foo[0])<<4) | GetHex(foo[1]);

	if ( endptr )
	{
		*endptr = foo+2;
	}

	return (NxU8) ret;
}


NxU16 GetHEX2(const char *foo,const char **endptr)
{
	NxU32 ret = 0;

	ret = (GetHex(foo[0])<<12) | (GetHex(foo[1])<<8) | (GetHex(foo[2])<<4) | GetHex(foo[3]);

	if ( endptr )
	{
		*endptr = foo+4;
	}

	return (NxU16) ret;
}

NxU32 GetHEX4(const char *foo,const char **endptr)
{
	NxU32 ret = 0;

	for (NxI32 i=0; i<8; i++)
	{
		ret = (ret<<4) | GetHex(foo[i]);
	}

	if ( endptr )
	{
		*endptr = foo+8;
	}

	return ret;
}

NxU32 GetHEX(const char *foo,const char **endptr)
{
	NxU32 ret = 0;

	while ( *foo )
	{
		NxU8 c = ToLower( *foo );
		NxU32 v = 0;
		if ( c >= '0' && c <= '9' )
			v = c-'0';
		else
		{
			if ( c >= 'a' && c <= 'f' )
			{
				v = 10 + c-'a';
			}
			else
				break;
		}
		ret = (ret<<4)|v;
		foo++;
	}

	if ( endptr ) *endptr = foo;

	return ret;
}


bool         IsWhitespace(char c)
{
	if ( c == ' ' || c == 9 || c == 13 || c == 10 || c == ',' ) return true;
	return false;
}


const char * SkipWhitespace(const char *str)
{
	while ( *str && IsWhitespace(*str) ) str++;
	return str;
}

#define MAXNUM 32

NxF32        GetFloatValue(const char *str,const char **next)
{
	NxF32 ret = 0;

	if ( next ) *next = 0;

	str = SkipWhitespace(str);

	char dest[MAXNUM];
	char *dst = dest;
	const char *hex = 0;

	for (NxI32 i=0; i<(MAXNUM-1); i++)
	{
		char c = *str;
		if ( c == 0 || IsWhitespace(c) )
		{
			if ( next ) *next = str;
			break;
		}
		else if ( c == '$' )
		{
			hex = str+1;
		}
		*dst++ = ToLower(c);
		str++;
	}

	*dst = 0;

	if ( hex )
	{
		NxU32 iv = GetHEX(hex,0);
		NxF32 *v = (NxF32 *)&iv;
		ret = *v;
	}
	else if ( dest[0] == 'f' )
	{
		if ( strcmp(dest,"fltmax") == 0 || strcmp(dest,"fmax") == 0 )
		{
			ret = FLT_MAX;
		}
		else if ( strcmp(dest,"fltmin") == 0 || strcmp(dest,"fmin") == 0 )
		{
			ret = FLT_MIN;
		}
	}
	else if ( dest[0] == 't' ) // t or 'true' is treated as the value '1'.
	{
		ret = 1;
	}
	else
	{
		ret = (NxF32)atof(dest);
	}
	return ret;
}

NxI32          GetIntValue(const char *str,const char **next)
{
	NxI32 ret = 0;

	if ( next ) *next = 0;

	str = SkipWhitespace(str);

	char dest[MAXNUM];
	char *dst = dest;

	for (NxI32 i=0; i<(MAXNUM-1); i++)
	{
		char c = *str;
		if ( c == 0 || IsWhitespace(c) )
		{
			if ( next ) *next = str;
			break;
		}
		*dst++ = c;
		str++;
	}

	*dst = 0;

	ret = atoi(dest);

	return ret;
}


bool CharToWide(const char *source,wchar_t *dest,NxI32 maxlen)
{
	bool ret = false;

	ret = true;
	mbstowcs(dest, source, maxlen );

	return ret;
}

bool WideToChar(const wchar_t *source,char *dest,NxI32 maxlen)
{
	bool ret = false;

	ret = true;
	wcstombs(dest, source, maxlen );

	return ret;
}



const char * GetTrueFalse(NxU32 state)
{
	if ( state ) return "true";
	return "false";
};


const char * FloatString(NxF32 v,bool binary)
{
	static char data[64*16];
	static NxI32  index=0;

	char *ret = &data[index*64];
	index++;
	if (index == 16 ) index = 0;

  if ( !_finite(v) )
  {
    assert(0);
    strcpy(ret,"0"); // not a valid number!
  }
/***
	else if ( v == FLT_MAX )
	{
		strcpy(ret,"fltmax");
	}
	else if ( v == FLT_MIN )
	{
		strcpy(ret,"fltmin");
	}
***/
	else if ( v == 1 )
	{
		strcpy(ret,"1");
	}
	else if ( v == 0 )
	{
		strcpy(ret,"0");
	}
	else if ( v == - 1 )
	{
		strcpy(ret,"-1");
	}
	else
	{
		if ( binary )
		{
			NxU32 *iv = (NxU32 *) &v;
			sprintf(ret,"%.4f$%x", v, *iv );
		}
		else
		{
			sprintf(ret,"%.9f", v );
			const char *dot = strstr(ret,".");
			if ( dot )
			{
				NxI32 len = (NxI32)strlen(ret);
				char *foo = &ret[len-1];
				while ( *foo == '0' ) foo--;
				if ( *foo == '.' )
					*foo = 0;
				else
					foo[1] = 0;
			}
		}
	}

	return ret;
}


char * NextSep(char *str,char &c)
{
	while ( *str && *str != ',' && *str != ')' )
	{
		str++;
	}
	c = *str;
	return str;
}

NxI32 GetUserArgs(const char *us,const char *key,const char **args)
{
	NxI32 ret = 0;
	static char arglist[2048];
	strcpy(arglist,us);

	char keyword[512];
	sprintf(keyword,"%s(", key );
	char *found = strstr(arglist,keyword);
	if ( found )
	{
		found = strstr(found,"(");
		found++;
		args[ret] = found;
		ret++;
    static bool bstate = true;
		while ( bstate )
		{
			char c;
			found = NextSep(found,c);
			if ( found )
			{
				*found = 0;
				if ( c == ',' )
				{
					found++;
					args[ret] = found;
					ret++;
				}
				else
				{
					break;
				}
			}
		}
	}
	return ret;
}

bool GetUserSetting(const char *us,const char *key,NxI32 &v)
{
	bool ret = false;

	const char *argv[256];

	NxI32 argc = GetUserArgs(us,key,argv);
	if ( argc )
	{
		v = atoi( argv[0] );
		ret = true;
	}
	return ret;
}

bool GetUserSetting(const char *us,const char *key,const char * &v)
{
	bool ret = false;

	const char *argv[256];
	NxI32 argc = GetUserArgs(us,key,argv);
	if ( argc )
	{
		v = argv[0];
		ret = true;
	}
	return ret;
}

const char **  GetArgs(char *str,NxI32 &count) // destructable parser, stomps EOS markers on the input string!
{
	InPlaceParser ipp;

	return ipp.GetArglist(str,count);
}

const char * GetRootName(const char *fname)
{
	static char scratch[512];

	const char *source = fname;

	const char *start  = fname;

  while ( *source )
  {
  	if ( *source == '/' || *source == '\\' )
  	{
  		start = source+1;
  	}
  	source++;
  }

	strcpy(scratch,start);

  char *dot = strrchr( scratch, '.' );
 
  if ( dot )
  {
  	*dot = 0;
  }

	return scratch;
}

bool IsTrueFalse(const char *c)
{
	bool ret = false;

  if ( stricmp(c,"true") == 0 || stricmp(c,"1") == 0 ) ret = true;

  return ret;
}


bool IsDirectory(const char *fname,char *path,char *basename,char *postfix)
{
	bool ret = false;

	strcpy(path,fname);
	strcpy(basename,fname);

	char *foo = path;
	char *last = 0;

	while ( *foo )
	{
		if ( *foo == '\\' || *foo == '/' ) last = foo;
		foo++;
	}

	if ( last )
	{
		strcpy(basename,last+1);
		*last = 0;
		ret = true;
	}

	const char *scan = fname;

  static bool bstate = true;
	while ( bstate )
	{
		const char *dot = strstr(scan,".");
		if ( dot == 0 )
				break;
		scan = dot+1;
	}

	strcpy(postfix,scan);
	strlwr(postfix);

	return ret;
}

bool hasSpace(const char *str) // true if the string contains a space
{
	bool ret = false;

  while ( *str )
  {
  	char c = *str++;
  	if ( c == 32 || c == 9 )
  	{
  		ret = true;
  		break;
  	}
  }
  return ret;
}


const char * lastDot(const char *src)
{
  const char *ret = 0;

  const char *dot = strchr(src,'.');
  while ( dot )
  {
    ret = dot;
    dot = strchr(dot+1,'.');
  }
  return ret;
}


const char *   lastChar(const char *src,char c)
{
  const char *ret = 0;

  const char *dot = (const char *)strchr(src,c);
  while ( dot )
  {
    ret = dot;
    dot = (const char *)strchr(dot+1,c);
  }
  return ret;
}


const char *         lastSlash(const char *src) // last forward or backward slash character, null if none found.
{
  const char *ret = 0;

  const char *dot = strchr(src,'\\');
  if  ( dot == 0 )
    dot = strchr(src,'/');
  while ( dot )
  {
    ret = dot;
    dot = strchr(ret+1,'\\');
    if ( dot == 0 )
      dot = strchr(ret+1,'/');
  }
  return ret;
}


const char	*fstring(NxF32 v)
{
	static char	data[64	*16];
	static NxI32 index = 0;

	char *ret	=	&data[index	*64];
	index++;
	if (index	== 16)
	{
		index	=	0;
	}

	if (v	== FLT_MIN)
	{
		return "-INF";
	}
	// collada notation	for	FLT_MIN	and	FLT_MAX
	if (v	== FLT_MAX)
	{
		return "INF";
	}

	if (v	== 1)
	{
		strcpy(ret,	"1");
	}
	else if	(v ==	0)
	{
		strcpy(ret,	"0");
	}
	else if	(v ==	 - 1)
	{
		strcpy(ret,	"-1");
	}
	else
	{
		sprintf(ret, "%.9f", v);
		const	char *dot	=	strstr(ret,	".");
		if (dot)
		{
			NxI32	len	=	(NxI32)strlen(ret);
			char *foo	=	&ret[len - 1];
			while	(*foo	== '0')
			{
				foo--;
			}
			if (*foo ==	'.')
			{
				*foo = 0;
			}
			else
			{
				foo[1] = 0;
			}
		}
	}

	return ret;
}


#define MAXNUMERIC 32  // JWR  support up to 16 32 character long numeric formated strings
#define MAXFNUM    16

static	char  gFormat[MAXNUMERIC*MAXFNUM];
static NxI32    gIndex=0;

const char * formatNumber(NxI32 number) // JWR  format this integer into a fancy comma delimited string
{
	char * dest = &gFormat[gIndex*MAXNUMERIC];
	gIndex++;
	if ( gIndex == MAXFNUM ) gIndex = 0;

	char scratch[512];

#if defined (LINUX_GENERIC) || defined(LINUX) || defined(__CELLOS_LV2__)
	snprintf(scratch, 10, "%d", number);
#else
	itoa(number,scratch,10);
#endif

	char *str = dest;
	NxU32 len = (NxU32)strlen(scratch);
	for (NxU32 i=0; i<len; i++)
	{
		NxI32 place = (len-1)-i;
		*str++ = scratch[i];
		if ( place && (place%3) == 0 ) *str++ = ',';
	}
	*str = 0;

	return dest;
}


bool fqnMatch(const char *n1,const char *n2) // returns true if two fully specified file names are 'the same' but ignores case sensitivty and treats either a forward or backslash as the same character.
{
  bool ret = true;

  while ( *n1 )
  {
    char c1 = *n1++;
    char c2 = *n2++;
    if ( c1 >= 'A' && c1 <= 'Z' ) c1+=32;
    if ( c2 >= 'A' && c2 <= 'Z' ) c2+=32;
    if ( c1 == '\\' ) c1 = '/';
    if ( c2 == '\\' ) c2 = '/';
    if ( c1 != c2 )
    {
      ret = false;
      break;
    }
  }
  if ( ret )
  {
    if ( *n2 ) ret = false;
  }

  return ret;

}


bool           getBool(const char *str)
{
  bool ret = false;

  if ( stricmp(str,"true") == 0 || strcmp(str,"1") == 0 || stricmp(str,"yes") == 0 ) ret = true;

  return ret;
}


bool needsQuote(const char *str) // if this string needs quotes around it (spaces, commas, #, etc)
{
  bool ret = false;

  if ( str )
  {
    while ( *str )
    {
      char c = *str++;
      if ( c == ',' || c == '#' || c == 32 || c == 9 )
      {
        ret = true;
        break;
      }
    }
  }
  return ret;
}

void  normalizeFQN(const wchar_t *source,wchar_t *dest)
{
  char scratch[512];
  WideToChar(source,scratch,512);
  char temp[512];
  normalizeFQN(scratch,temp);
  CharToWide(temp,dest,512);
}

void  normalizeFQN(const char *source,char *dest)
{
  if ( source && strlen(source ) )
  {
    while ( *source )
    {
      char c = *source++;
      if ( c == '\\' ) c = '/';
      if ( c >= 'A' && c <= 'Z' ) c+=32;
      *dest++ = c;
    }
    *dest = 0;
  }
  else
  {
    *dest = 0;
  }
}



bool           endsWith(const char *str,const char *ends,bool caseSensitive)
{
  bool ret = false;

  NxI32 l1 = strlen(str);
  NxI32 l2 = strlen(ends);
  if ( l1 >= l2 )
  {
    NxI32 diff = l1-l2;
    const char *echeck = &str[diff];
    if ( caseSensitive )
    {
      if ( strcmp(echeck,ends) == 0 )
      {
        ret = true;
      }
    }
    else
    {
      if ( stricmp(echeck,ends) == 0 )
      {
        ret = true;
      }
    }
  }
  return ret;
}

}; // end of namespace