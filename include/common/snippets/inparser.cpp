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
#include <assert.h>

#pragma warning(disable:4996)

#include "../../common/snippets/UserMemAlloc.h"
#include "inparser.h"
#include "filesystem.h"


/** @file inparser.cpp
 * @brief        Parse ASCII text, in place, very quickly.
 * @author       John W. Ratcliff
 * @date         December 3, 2003

 * This class provides for high speed in-place (destructive) parsing of an ASCII text file.
 * This class will either load an ASCII text file from disk, or can be constructed with a pointer to
 * a piece of ASCII text in memory.  It can only be called once, and the contents are destroyed.
 * To speed the process of parsing, it simply builds pointers to the original ascii data and replaces the
 * seperators with a zero byte to indicate end of string.  It performs callbacks to parse each line, in argc/argv format,
 * offering the option to cancel the parsing process at any time.
 *
 *
 * Written by John W. Ratcliff on December 3, 2003 : This is 100% open source MEMALLOC_FREE for use by anybody.  I wrote this
 * implementation as part of the TinyVRML project.  However, this parser can be used for any purposes.
 *
 * By default the only valid seperator is whitespace.  It will not treat commas or any other symbol as a separator.
 * However, you can specify up to 32 'hard' seperators, such as a comma, equal sign, etc. and these will act as valid
 * seperators and come back as part of the argc/argv data list.
 *
 * To use the parser simply inherit the pure virtual base class 'InPlaceParserInterface'.  Define the method 'ParseLine'.
 * When you invoke the Parse method on the InPlaceParser class, you will get an ARGC - ARGV style callback for each line
 * in the source file.  If you return 'false' at any time, it will abort parsing.  The entire thing is stack based, so you
 * can recursively call multiple parser instances.
 *
 * It is important to note.  Since this parser is 'in place' it writes 'zero bytes' (EOS marker) on top of the whitespace.
 * While it can handle text in quotes, it does not handle escape sequences.  This is a limitation which could be resolved.
 * There is a hard coded maximum limit of 512 arguments per line.
 *
 * Here is the full example usage:
 *
 *  InPlaceParser ipp("parse_me.txt");
 *
 *    ipp.Parse(this);
 *
 *  That's it, and you will receive an ARGC - ARGV callback for every line in the file.
 *
 *  If you want to parse some text in memory of your own. (It *MUST* be terminated by a zero byte, and lines seperated by carriage return
 *  or line-feed.  You will receive an assertion if it does not.  If you specify the source data than *you* are responsible for that memory
 *  and must de-allocate it yourself.  If the data was loaded from a file on disk, then it is automatically de-allocated by the InPlaceParser.
 *
 *  You can also construct the InPlaceParser without passing any data, so you can simply pass it a line of data at a time yourself.  The
 *  line of data should be zero-byte terminated.
*/

//==================================================================================
void InPlaceParser::SetFile(const char *fname,FileSystem *fsystem)
{
	if ( mMyAlloc )
	{
		MEMALLOC_FREE(mData);
	}
	mData = 0;
	mLen  = 0;
	mMyAlloc = false;

	if ( fsystem ) fname = fsystem->FileOpenString(fname,true);

	FILE *fph = fopen(fname,"rb");
	if ( fph )
	{
		fseek(fph,0L,SEEK_END);
		mLen = ftell(fph);
		fseek(fph,0L,SEEK_SET);

		if ( mLen )
		{
			mData = (char *) MEMALLOC_MALLOC(sizeof(char)*(mLen+1));
			HeI32 read = (HeI32)fread(mData,mLen,1,fph);
			if ( !read )
			{
				MEMALLOC_FREE(mData);
				mData = 0;
			}
			else
			{
				mData[mLen] = 0; // zero byte terminate end of file marker.
				mMyAlloc = true;
			}
		}
		fclose(fph);
	}
}

//==================================================================================
InPlaceParser::~InPlaceParser(void)
{
	if ( mMyAlloc )
	{
		MEMALLOC_FREE(mData);
	}
}

//==================================================================================
bool InPlaceParser::IsHard(char c)
{
	return mHard[c] == ST_HARD;
}

//==================================================================================
char * InPlaceParser::AddHard(HeI32 &argc,const char **argv,char *foo)
{
	while ( IsHard(*foo) )
	{
		const char *hard = &mHardString[*foo*2];
		if ( argc < MAXARGS )
		{
			argv[argc++] = hard;
		}
		++foo;
	}
	return foo;
}

//==================================================================================
bool   InPlaceParser::IsWhiteSpace(char c)
{
	return mHard[c] == ST_SOFT;
}

//==================================================================================
char * InPlaceParser::SkipSpaces(char *foo)
{
	while ( !EOS(*foo) && IsWhiteSpace(*foo) ) 
		++foo;
	return foo;
}

//==================================================================================
bool InPlaceParser::IsNonSeparator(char c)
{
	return ( !IsHard(c) && !IsWhiteSpace(c) && c != 0 );
}

//==================================================================================
HeI32 InPlaceParser::ProcessLine(HeI32 lineno,char *line,InPlaceParserInterface *callback)
{
	HeI32 ret = 0;

	const char *argv[MAXARGS];
	HeI32 argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{
		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) ) 
			break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			++foo;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) 
				++foo;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				++foo;
			}
		}
		else
		{
			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

			if ( IsNonSeparator(*foo) )  // add non-hard argument.
			{
				bool quote  = false;
				if ( *foo == mQuoteChar )
				{
					++foo;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) 
						++foo;
					if ( *foo ) 
						*foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						++foo;
						break;
					}
					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
					{
						const char *hard = &mHardString[*foo*2];
						*foo = 0;
						if ( argc < MAXARGS )
						{
							argv[argc++] = hard;
						}
						++foo;
						break;
					}
					++foo;
				} // end of while loop...
			}
		}
	}

	if ( argc )
	{
		ret = callback->ParseLine(lineno, argc, argv );
	}

	return ret;
}


HeI32  InPlaceParser::Parse(const char *str,InPlaceParserInterface *callback) // returns true if entire file was parsed, false if it aborted for some reason
{
  HeI32 ret = 0;

  mLen = (HeI32)strlen(str);
  if ( mLen )
  {
    mData = (char *)MEMALLOC_MALLOC(mLen+1);
    strcpy(mData,str);
    mMyAlloc = true;
    ret = Parse(callback);
  }
  return ret;
}

//==================================================================================
// returns true if entire file was parsed, false if it aborted for some reason
//==================================================================================
HeI32  InPlaceParser::Parse(InPlaceParserInterface *callback)
{
	HeI32 ret = 0;
	HE_ASSERT( callback );
	if ( mData )
	{
		HeI32 lineno = 0;

		char *foo   = mData;
		char *begin = foo;

		while ( *foo )
		{
			if ( isLineFeed(*foo) )
			{
				++lineno;
				*foo = 0;
				if ( *begin ) // if there is any data to parse at all...
				{
          bool snarfed = callback->preParseLine(lineno,begin);
          if ( !snarfed )
          {
  					HeI32 v = ProcessLine(lineno,begin,callback);
  					if ( v )
  						ret = v;
          }
				}

				++foo;
				if ( *foo == 10 )
					++foo; // skip line feed, if it is in the carraige-return line-feed format...
				begin = foo;
			}
			else
			{
				++foo;
			}
		}

		lineno++; // lasst line.

		HeI32 v = ProcessLine(lineno,begin,callback);
		if ( v )
			ret = v;
	}
	return ret;
}

//==================================================================================
void InPlaceParser::DefaultSymbols(void)
{
	SetHardSeparator(',');
	SetHardSeparator('(');
	SetHardSeparator(')');
	SetHardSeparator('=');
	SetHardSeparator('[');
	SetHardSeparator(']');
	SetHardSeparator('{');
	SetHardSeparator('}');
	SetCommentSymbol('#');
}

//==================================================================================
// convert source string into an arg list, this is a destructive parse.
//==================================================================================
const char ** InPlaceParser::GetArglist(char *line,HeI32 &count)
{
	const char **ret = 0;

	HeI32 argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{
		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) )
			break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			++foo;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) 
				++foo;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				++foo;
			}
		}
		else
		{
			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

			if ( IsNonSeparator(*foo) )  // add non-hard argument.
			{
				bool quote  = false;
				if ( *foo == mQuoteChar )
				{
					++foo;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) 
						++foo;
					if ( *foo ) 
						*foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						++foo;
						break;
					}
					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
					{
						const char *hard = &mHardString[*foo*2];
						*foo = 0;
						if ( argc < MAXARGS )
						{
							argv[argc++] = hard;
						}
						++foo;
						break;
					}
					++foo;
				} // end of while loop...
			}
		}
	}

	count = argc;
	if ( argc )
	{
		ret = argv;
	}

	return ret;
}

