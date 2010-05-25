#ifndef INPARSER_H

#define INPARSER_H

#include <assert.h>

#include "UserMemAlloc.h"

#define MAXARGS 512

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



/** @file inparser.h
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
 * You can specify any character to be  a 'hard' seperator, such as an '=' for example and that will come back as a
 * distinct argument string.
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

namespace NVSHARE
{

class FileSystem;

class InPlaceParserInterface
{
public:
	virtual PxI32 ParseLine(PxI32 lineno,PxI32 argc,const char **argv) =0;  // return TRUE to continue parsing, return FALSE to abort parsing process
  virtual bool preParseLine(PxI32 /* lineno */,const char * /* line */)  { return false; }; // optional chance to pre-parse the line as raw data.  If you return 'true' the line will be skipped assuming you snarfed it.
};

enum SeparatorType
{
	ST_DATA,        // is data
	ST_HARD,        // is a hard separator
	ST_SOFT,        // is a soft separator
	ST_EOS,          // is a comment symbol, and everything past this character should be ignored
  ST_LINE_FEED
};

class InPlaceParser
{
public:
	InPlaceParser(void)
	{
		Init();
	}

	InPlaceParser(char *data,PxI32 len)
	{
		Init();
		SetSourceData(data,len);
	}

	InPlaceParser(const char *fname,FileSystem *fsystem)
	{
		Init();
		SetFile(fname,fsystem);
	}

	~InPlaceParser(void);

	void Init(void)
	{
		mQuoteChar = 34;
		mData = 0;
		mLen  = 0;
		mMyAlloc = false;
		for (PxI32 i=0; i<256; i++)
		{
			mHard[i] = ST_DATA;
			mHardString[i*2] = (char)i;
			mHardString[i*2+1] = 0;
		}
		mHard[0]  = ST_EOS;
		mHard[32] = ST_SOFT;
		mHard[9]  = ST_SOFT;
		mHard[13] = ST_LINE_FEED;
		mHard[10] = ST_LINE_FEED;
	}

	void SetFile(const char *fname,FileSystem *fsystem);

	void SetSourceData(char *data,PxI32 len)
	{
		mData = data;
		mLen  = len;
		mMyAlloc = false;
	};

	PxI32  Parse(const char *str,InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason
	PxI32  Parse(InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason

	PxI32 ProcessLine(PxI32 lineno,char *line,InPlaceParserInterface *callback);

	const char ** GetArglist(char *source,PxI32 &count); // convert source string into an arg list, this is a destructive parse.

	void SetHardSeparator(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}

	void SetHard(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}

	void SetSoft(char c) // add a hard separator
	{
		mHard[c] = ST_SOFT;
	}


	void SetCommentSymbol(char c) // comment character, treated as 'end of string'
	{
		mHard[c] = ST_EOS;
	}

	void ClearHardSeparator(char c)
	{
		mHard[c] = ST_DATA;
	}


	void DefaultSymbols(void); // set up default symbols for hard seperator and comment symbol of the '#' character.

	bool EOS(char c)
	{
		if ( mHard[c] == ST_EOS )
		{
			return true;
		}
		return false;
	}

	void SetQuoteChar(char c)
	{
		mQuoteChar = c;
	}

	bool HasData( void ) const
	{
		return ( mData != 0 );
	}

  void setLineFeed(char c)
  {
    mHard[c] = ST_LINE_FEED;
  }

  bool isLineFeed(char c)
  {
    if ( mHard[c] == ST_LINE_FEED ) return true;
    return false;
  }

private:

	inline char * AddHard(PxI32 &argc,const char **argv,char *foo);
	inline bool   IsHard(char c);
	inline char * SkipSpaces(char *foo);
	inline bool   IsWhiteSpace(char c);
	inline bool   IsNonSeparator(char c); // non seperator,neither hard nor soft

	bool   mMyAlloc; // whether or not *I* allocated the buffer and am responsible for deleting it.
	char  *mData;  // ascii data to parse.
	PxI32    mLen;   // length of data
	SeparatorType  mHard[256];
	char   mHardString[256*2];
	char           mQuoteChar;
	const char *argv[MAXARGS];
};

}; // end of namespace

#endif
