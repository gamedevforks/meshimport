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
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>

#include "UserMemAlloc.h"
#include "asc2bin.h"

namespace NVSHARE
{

static inline bool         IsWhitespace(char c)
{
	if ( c == ' ' || c == 9 || c == 13 || c == 10 || c == ',' ) return true;
	return false;
}



static inline const char * SkipWhitespace(const char *str)
{
  if ( str )
  {
	  while ( *str && IsWhitespace(*str) ) str++;
  }
	return str;
}

static char ToLower(char c)
{
	if ( c >= 'A' && c <= 'Z' ) c+=32;
	return c;
}

static inline NxU32 GetHex(NxU8 c)
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

static inline NxU8 GetHEX1(const char *foo,const char **endptr)
{
	NxU32 ret = 0;

	ret = (GetHex(foo[0])<<4) | GetHex(foo[1]);

	if ( endptr )
	{
		*endptr = foo+2;
	}

	return (NxU8) ret;
}


static inline unsigned short GetHEX2(const char *foo,const char **endptr)
{
	NxU32 ret = 0;

	ret = (GetHex(foo[0])<<12) | (GetHex(foo[1])<<8) | (GetHex(foo[2])<<4) | GetHex(foo[3]);

	if ( endptr )
	{
		*endptr = foo+4;
	}

	return (unsigned short) ret;
}

static inline NxU32 GetHEX4(const char *foo,const char **endptr)
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

static inline NxU32 GetHEX(const char *foo,const char **endptr)
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




#define MAXNUM 32

static inline NxF32        GetFloatValue(const char *str,const char **next)
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

static inline NxI32          GetIntValue(const char *str,const char **next)
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



#ifdef PLAYSTATION3
#include <ctype.h> // for tolower()
#endif

enum Atype {  AT_FLOAT,  AT_INT,  AT_CHAR,  AT_BYTE,  AT_SHORT,  AT_STR,  AT_HEX1,  AT_HEX2,  AT_HEX4,  AT_LAST };

#define MAXARG 64

#if 0
void TestAsc2bin(void)
{
	Asc2Bin("1 2 A 3 4 Foo AB ABCD FFEDFDED",1,"f d c b h p x1 x2 x4", 0);
}
#endif

void * Asc2Bin(const char *source,const NxI32 count,const char *spec,void *dest)
{

	NxI32   cnt = 0;
	NxI32   size  = 0;

	Atype types[MAXARG];

	const char *ctype = spec;

	while ( *ctype )
	{
		switch ( ToLower(*ctype) )
		{
			case 'f': size+= sizeof(NxF32); types[cnt] = AT_FLOAT; cnt++;  break;
			case 'd': size+= sizeof(NxI32);   types[cnt] = AT_INT;   cnt++;  break;
			case 'c': size+=sizeof(char);   types[cnt] = AT_CHAR;  cnt++;  break;
			case 'b': size+=sizeof(char);   types[cnt] = AT_BYTE;  cnt++;  break;
			case 'h': size+=sizeof(short);  types[cnt] = AT_SHORT; cnt++;  break;
			case 'p': size+=sizeof(const char *);  types[cnt] = AT_STR; cnt++;  break;
			case 'x':
				{
					Atype type = AT_HEX4;
					NxI32 sz = 4;
					switch ( ctype[1] )
					{
						case '1':  type = AT_HEX1; sz   = 1; ctype++;  break;
						case '2':  type = AT_HEX2; sz   = 2; ctype++;  break;
						case '4':  type = AT_HEX4; sz   = 4; ctype++;  break;
					}
					types[cnt] = type;
					size+=sz;
					cnt++;
				}
				break;
		}
		if ( cnt == MAXARG ) return 0; // over flowed the maximum specification!
		ctype++;
	}

	bool myalloc = false;

	if ( dest == 0 )
	{
		myalloc = true;
		dest = (char *) MEMALLOC_MALLOC(sizeof(char)*count*size);
	}

	// ok...ready to parse lovely data....
	memset(dest,0,count*size); // zero out memory

	char *dst = (char *) dest; // where we are storing the results
	for (NxI32 i=0; i<count; i++)
	{
		for (NxI32 j=0; j<cnt; j++)
		{
			source = SkipWhitespace(source); // skip white spaces.

			if ( *source == 0 ) // we hit the end of the input data before we successfully parsed all input!
			{
				if ( myalloc )
				{
					MEMALLOC_FREE(dest);
				}
				return 0;
			}

			switch ( types[j] )
			{
				case AT_FLOAT:
					{
						NxF32 *v = (NxF32 *) dst;
						*v = GetFloatValue(source,&source);
						dst+=sizeof(NxF32);
					}
					break;
				case AT_INT:
					{
						NxI32 *v = (NxI32 *) dst;
						*v = GetIntValue( source, &source );
						dst+=sizeof(NxI32);
					}
					break;
				case AT_CHAR:
					{
						*dst++ = *source++;
					}
					break;
				case AT_BYTE:
					{
						char *v = (char *) dst;
						*v = (char)GetIntValue(source,&source);
						dst+=sizeof(char);
					}
					break;
				case AT_SHORT:
					{
						short *v = (short *) dst;
						*v = (unsigned short)GetIntValue( source,&source );
						dst+=sizeof(short);
					}
					break;
				case AT_STR:
					{
						const char **ptr = (const char **) dst;
						*ptr = source;
						dst+=sizeof(const char *);
						while ( *source && !IsWhitespace(*source) ) source++;
					}
					break;
				case AT_HEX1:
					{
						NxU32 hex = GetHEX1(source,&source);
						NxU8 *v = (NxU8 *) dst;
						*v = (NxU8)hex;
						dst+=sizeof(NxU8);
					}
					break;
				case AT_HEX2:
					{
						NxU32 hex = GetHEX2(source,&source);
						unsigned short *v = (unsigned short *) dst;
						*v = (unsigned short)hex;
						dst+=sizeof(unsigned short);
					}
					break;
				case AT_HEX4:
					{
						NxU32 hex = GetHEX4(source,&source);
						NxU32 *v = (NxU32 *) dst;
						*v = hex;
						dst+=sizeof(NxU32);
					}
					break;
			}
		}
	}

	return dest;
}

}; // end of namespace
