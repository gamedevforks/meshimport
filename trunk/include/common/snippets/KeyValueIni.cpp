#include "KeyValueIni.h"

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

#include <vector>
#include <string.h>
#include <assert.h>

#include "KeyValueIni.h"

#pragma warning(disable:4100 4267)

namespace KEYVALUEINI
{


#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.

#define DEFAULT_BUFFER_SIZE 1000000
#define DEFAULT_GROW_SIZE   2000000

#if defined(LINUX)
#   define _stricmp(a,b) strcasecmp((a),(b))
#endif

class FILE_INTERFACE
{
public:
	FILE_INTERFACE(const char *fname,const char *spec,void *mem,size_t len)
	{
		mMyAlloc = false;
		mRead = true; // default is read access.
		mFph = 0;
		mData = (char *) mem;
		mLen  = len;
		mLoc  = 0;

		if ( spec && _stricmp(spec,"wmem") == 0 )
		{
			mRead = false;
			if ( mem == 0 || len == 0 )
			{
				mData = (char *)MEMALLOC_MALLOC(DEFAULT_BUFFER_SIZE);
				mLen  = DEFAULT_BUFFER_SIZE;
				mMyAlloc = true;
			}
		}

		if ( mData == 0 )
		{
			mFph = fopen(fname,spec);
		}

  	strncpy(mName,fname,512);
	}

  ~FILE_INTERFACE(void)
  {
  	if ( mMyAlloc )
  	{
  		MEMALLOC_FREE(mData);
  	}
  	if ( mFph )
  	{
  		fclose(mFph);
  	}
  }

  size_t read(char *data,size_t size)
  {
  	size_t ret = 0;
  	if ( (mLoc+size) <= mLen )
  	{
  		memcpy(data, &mData[mLoc], size );
  		mLoc+=size;
  		ret = 1;
  	}
    return ret;
  }

  size_t write(const char *data,size_t size)
  {
  	size_t ret = 0;

		if ( (mLoc+size) >= mLen && mMyAlloc ) // grow it
		{
			size_t newLen = mLen+DEFAULT_GROW_SIZE;
			if ( size > newLen ) newLen = size+DEFAULT_GROW_SIZE;

			char *data = (char *)MEMALLOC_MALLOC(newLen);
			memcpy(data,mData,mLoc);
      MEMALLOC_FREE(mData);
			mData = data;
			mLen  = newLen;
		}

  	if ( (mLoc+size) <= mLen )
  	{
  		memcpy(&mData[mLoc],data,size);
  		mLoc+=size;
  		ret = 1;
  	}
  	return ret;
  }

	size_t read(void *buffer,size_t size,size_t count)
	{
		size_t ret = 0;
		if ( mFph )
		{
			ret = fread(buffer,size,count,mFph);
		}
		else
		{
			char *data = (char *)buffer;
			for (size_t i=0; i<count; i++)
			{
				if ( (mLoc+size) <= mLen )
				{
					read(data,size);
					data+=size;
					ret++;
				}
				else
				{
					break;
				}
			}
		}
		return ret;
	}

  size_t write(const void *buffer,size_t size,size_t count)
  {
  	size_t ret = 0;

  	if ( mFph )
  	{
  		ret = fwrite(buffer,size,count,mFph);
  	}
  	else
  	{
  		const char *data = (const char *)buffer;

  		for (size_t i=0; i<count; i++)
  		{
    		if ( write(data,size) )
				{
    			data+=size;
    			ret++;
    		}
    		else
    		{
    			break;
    		}
  		}
  	}
  	return ret;
  }

  size_t writeString(const char *str)
  {
  	size_t ret = 0;
  	if ( str )
  	{
  		size_t len = strlen(str);
  		ret = write(str,len, 1 );
  	}
  	return ret;
  }


  size_t  flush(void)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = fflush(mFph);
  	}
  	return ret;
  }


  size_t seek(size_t loc,size_t mode)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = fseek(mFph,loc,mode);
  	}
  	else
  	{
  		if ( mode == SEEK_SET )
  		{
  			if ( loc <= mLen )
  			{
  				mLoc = loc;
  				ret = 1;
  			}
  		}
  		else if ( mode == SEEK_END )
  		{
  			mLoc = mLen;
  		}
  		else
  		{
  			assert(0);
  		}
  	}
  	return ret;
  }

  size_t tell(void)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = ftell(mFph);
  	}
  	else
  	{
  		ret = mLoc;
  	}
  	return ret;
  }

  size_t myputc(char c)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = fputc(c,mFph);
  	}
  	else
  	{
  		ret = write(&c,1);
  	}
  	return ret;
  }

  size_t eof(void)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = feof(mFph);
  	}
  	else
  	{
  		if ( mLoc >= mLen )
  			ret = 1;
  	}
  	return ret;
  }

  size_t  error(void)
  {
  	size_t ret = 0;
  	if ( mFph )
  	{
  		ret = ferror(mFph);
  	}
  	return ret;
  }


  FILE 	*mFph;
  char  *mData;
  size_t    mLen;
  size_t    mLoc;
  bool   mRead;
	char   mName[512];
	bool   mMyAlloc;

};


FILE_INTERFACE * fi_fopen(const char *fname,const char *spec,void *mem=0,size_t len=0)
{
	FILE_INTERFACE *ret = 0;

	ret = MEMALLOC_NEW(FILE_INTERFACE)(fname,spec,mem,len);

	if ( mem == 0 && ret->mData == 0)
  {
  	if ( ret->mFph == 0 )
  	{
      delete ret;
  		ret = 0;
  	}
  }

	return ret;
}

void       fi_fclose(FILE_INTERFACE *file)
{
  delete file;
}

size_t        fi_fread(void *buffer,size_t size,size_t count,FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->read(buffer,size,count);
	}
	return ret;
}

size_t        fi_fwrite(const void *buffer,size_t size,size_t count,FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->write(buffer,size,count);
	}
	return ret;
}

size_t        fi_fprintf(FILE_INTERFACE *fph,const char *fmt,...)
{
	size_t ret = 0;

	char buffer[2048];
  buffer[2047] = 0;
	_vsnprintf(buffer,2047, fmt, (char *)(&fmt+1));

	if ( fph )
	{
		ret = fph->writeString(buffer);
	}

	return ret;
}


size_t        fi_fflush(FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->flush();
	}
	return ret;
}


size_t        fi_fseek(FILE_INTERFACE *fph,size_t loc,size_t mode)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->seek(loc,mode);
	}
	return ret;
}

size_t        fi_ftell(FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->tell();
	}
	return ret;
}

size_t        fi_fputc(char c,FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->myputc(c);
	}
	return ret;
}

size_t        fi_fputs(const char *str,FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->writeString(str);
	}
	return ret;
}

size_t        fi_feof(FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->eof();
	}
	return ret;
}

size_t        fi_ferror(FILE_INTERFACE *fph)
{
	size_t ret = 0;
	if ( fph )
	{
		ret = fph->error();
	}
	return ret;
}

void *     fi_getMemBuffer(FILE_INTERFACE *fph,size_t &outputLength)
{
	outputLength = 0;
	void * ret = 0;
	if ( fph )
	{
		ret = fph->mData;
		outputLength = fph->mLoc;
	}
	return ret;
}


#if defined(__APPLE__) || defined(__CELLOS_LV2__) || defined(LINUX)
#define stricmp(a, b) strcasecmp((a), (b))
#endif

/*******************************************************************/
/******************** InParser.h  ********************************/
/*******************************************************************/
class InPlaceParserInterface
{
public:
	virtual NxI32 ParseLine(NxI32 lineno,NxI32 argc,const char **argv) =0;  // return TRUE to continue parsing, return FALSE to abort parsing process
};

enum SeparatorType
{
	ST_DATA,        // is data
	ST_HARD,        // is a hard separator
	ST_SOFT,        // is a soft separator
	ST_EOS,          // is a comment symbol, and everything past this character should be ignored
  ST_COMMENT
};

class InPlaceParser
{
public:
	InPlaceParser(void)
	{
		Init();
	}

	InPlaceParser(char *data,NxI32 len)
	{
		Init();
		SetSourceData(data,len);
	}

	InPlaceParser(const char *fname)
	{
		Init();
		SetFile(fname);
	}

	~InPlaceParser(void);

	void Init(void)
	{
		mQuoteChar = 34;
		mData = 0;
		mLen  = 0;
		mMyAlloc = false;
		for (NxI32 i=0; i<256; i++)
		{
			mHard[i] = ST_DATA;
			mHardString[i*2] = (char)i;
			mHardString[i*2+1] = (char)0;
		}
		mHard[0]  = ST_EOS;
		mHard[32] = ST_SOFT;
		mHard[9]  = ST_SOFT;
		mHard[13] = ST_SOFT;
		mHard[10] = ST_SOFT;
	}

	void SetFile(const char *fname); // use this file as source data to parse.

	void SetSourceData(char *data,NxI32 len)
	{
		mData = data;
		mLen  = len;
		mMyAlloc = false;
	};

#ifdef _DEBUG
  void validateMem(const char *data,NxI32 len)
  {
    for (NxI32 i=0; i<len; i++)
    {
      assert( data[i] );
    }
  }
#else
  void validateMem(const char *,NxI32 )
  {

  }
#endif

	void SetSourceDataCopy(const char *data,NxI32 len)
	{
    if ( len )
    {

      //validateMem(data,len);

      MEMALLOC_FREE(mData);
  		mData = (char *)MEMALLOC_MALLOC(len+1);
      memcpy(mData,data,len);
      mData[len] = 0;

      //validateMem(mData,len);
   		mLen  = len;
  		mMyAlloc = true;
    }
	};

	NxI32  Parse(InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason

	NxI32 ProcessLine(NxI32 lineno,char *line,InPlaceParserInterface *callback);

	void SetHardSeparator(NxU8 c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}

	void SetHard(NxU8 c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}


	void SetCommentSymbol(NxU8 c) // comment character, treated as 'end of string'
	{
		mHard[c] = ST_COMMENT;
	}

	void ClearHardSeparator(NxU8 c)
	{
		mHard[c] = ST_DATA;
	}


	void DefaultSymbols(void); // set up default symbols for hard seperator and comment symbol of the '#' character.

	bool EOS(NxU8 c)
	{
		if ( mHard[c] == ST_EOS || mHard[c] == ST_COMMENT )
		{
			return true;
		}
		return false;
	}

	void SetQuoteChar(char c)
	{
		mQuoteChar = c;
	}


  inline bool IsComment(NxU8 c) const;

private:


	inline char * AddHard(NxI32 &argc,const char **argv,char *foo);
	inline bool   IsHard(NxU8 c);
	inline char * SkipSpaces(char *foo);
	inline bool   IsWhiteSpace(NxU8 c);
	inline bool   IsNonSeparator(NxU8 c); // non seperator,neither hard nor soft

	bool   mMyAlloc; // whether or not *I* allocated the buffer and am responsible for deleting it.
	char  *mData;  // ascii data to parse.
	NxI32    mLen;   // length of data
	SeparatorType  mHard[256];
	char   mHardString[256*2];
	char           mQuoteChar;
};

/*******************************************************************/
/******************** InParser.cpp  ********************************/
/*******************************************************************/
void InPlaceParser::SetFile(const char *fname)
{
	if ( mMyAlloc )
	{
		MEMALLOC_FREE(mData);
	}
	mData = 0;
	mLen  = 0;
	mMyAlloc = false;

	FILE *fph = fopen(fname,"rb");
	if ( fph )
	{
		fseek(fph,0L,SEEK_END);
		mLen = ftell(fph);
		fseek(fph,0L,SEEK_SET);
		if ( mLen )
		{
			mData = (char *) MEMALLOC_MALLOC(sizeof(char)*(mLen+1));
			NxI32 ok = fread(mData, mLen, 1, fph);
			if ( !ok )
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

InPlaceParser::~InPlaceParser(void)
{
	if ( mMyAlloc )
	{
		MEMALLOC_FREE(mData);
	}
}

#define MAXARGS 512

bool InPlaceParser::IsHard(NxU8 c)
{
	return mHard[c] == ST_HARD;
}

char * InPlaceParser::AddHard(NxI32 &argc,const char **argv,char *foo)
{
	while ( IsHard(*foo) )
	{
    NxU8 c = *foo;
		const char *hard = &mHardString[c*2];
		if ( argc < MAXARGS )
		{
			argv[argc++] = hard;
		}
		foo++;
	}
	return foo;
}

bool   InPlaceParser::IsWhiteSpace(NxU8 c)
{
	return mHard[c] == ST_SOFT;
}

char * InPlaceParser::SkipSpaces(char *foo)
{
	while ( !EOS(*foo) && IsWhiteSpace(*foo) ) foo++;
	return foo;
}

bool InPlaceParser::IsNonSeparator(NxU8 c)
{
	if ( !IsHard(c) && !IsWhiteSpace(c) && c != 0 ) return true;
	return false;
}


bool InPlaceParser::IsComment(NxU8 c) const
{
  if ( mHard[c] == ST_COMMENT ) return true;
  return false;
}

NxI32 InPlaceParser::ProcessLine(NxI32 lineno,char *line,InPlaceParserInterface *callback)
{
	NxI32 ret = 0;

	const char *argv[MAXARGS];
	NxI32 argc = 0;

  char *foo = SkipSpaces(line); // skip leading spaces...

  if ( IsComment(*foo) )  // if the leading character is a comment symbol.
    return 0;


  if ( !EOS(*foo) )  // if we are not at the end of string then..
  {
    argv[argc++] = foo;  // this is the key
    foo++;

    while ( !EOS(*foo) )  // now scan forward until we hit an equal sign.
    {
      if ( *foo == '=' ) // if this is the equal sign then...
      {
        *foo = 0; // stomp a zero byte on the equal sign to terminate the key we should search for trailing spaces too...
        // look for trailing whitespaces and trash them.
        char *scan = foo-1;
        while ( IsWhiteSpace(*scan) )
        {
          *scan = 0;
          scan--;
        }

        foo++;
        foo = SkipSpaces(foo);
        if ( !EOS(*foo) )
        {
          argv[argc++] = foo;
          foo++;
          while ( !EOS(*foo) )
          {
            foo++;
          }
          *foo = 0;
          char *scan = foo-1;
          while ( IsWhiteSpace(*scan) )
          {
            *scan = 0;
            scan--;
          }
          break;
        }
      }
      if ( *foo )
        foo++;
    }
  }

  *foo = 0;

	if ( argc )
	{
		ret = callback->ParseLine(lineno, argc, argv );
	}

	return ret;
}

NxI32  InPlaceParser::Parse(InPlaceParserInterface *callback) // returns true if entire file was parsed, false if it aborted for some reason
{
	assert( callback );
	if ( !mData ) return 0;

	NxI32 ret = 0;

	NxI32 lineno = 0;

	char *foo   = mData;
	char *begin = foo;


	while ( *foo )
	{
		if ( *foo == 10 || *foo == 13 )
		{
			lineno++;
			*foo = 0;

			if ( *begin ) // if there is any data to parse at all...
			{
				NxI32 v = ProcessLine(lineno,begin,callback);
				if ( v ) ret = v;
			}

			foo++;
			if ( *foo == 10 ) foo++; // skip line feed, if it is in the carraige-return line-feed format...
			begin = foo;
		}
		else
		{
			foo++;
		}
	}

	lineno++; // lasst line.

	NxI32 v = ProcessLine(lineno,begin,callback);
	if ( v ) ret = v;
	return ret;
}


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


}; // END KEYVALUE INI NAMESPACE

using namespace KEYVALUEINI;


class KeyValue
{
public:
  KeyValue(const char *key,const char *value,NxU32 lineno)
  {
    mKey = key;
    mValue = value;
    mLineNo = lineno;
  }

  const char * getKey(void) const { return mKey; };
  const char * getValue(void) const { return mValue; };
  NxU32 getLineNo(void) const { return mLineNo; };

  void save(FILE_INTERFACE *fph) const
  {
    fi_fprintf(fph,"%-30s = %s\r\n", mKey, mValue );
  }

  void setValue(const char *value)
  {
    mValue = value;
  }

private:
  NxU32 mLineNo;
  const char *mKey;
  const char *mValue;
};

typedef USER_STL::vector< KeyValue > KeyValueVector;

class KeyValueSection
{
public:
  KeyValueSection(const char *section,NxU32 lineno)
  {
    mSection = section;
    mLineNo  = lineno;
  }

  NxU32 getKeyCount(void) const { return mKeys.size(); };
  const char * getSection(void) const { return mSection; };
  NxU32 getLineNo(void) const { return mLineNo; };

  const char * locateValue(const char *key,NxU32 lineno) const
  {
    const char *ret = 0;

    for (NxU32 i=0; i<mKeys.size(); i++)
    {
      const KeyValue &v = mKeys[i];
      if ( stricmp(key,v.getKey()) == 0 )
      {
        ret = v.getValue();
        lineno = v.getLineNo();
        break;
      }
    }
    return ret;
  }

  const char *getKey(NxU32 index,NxU32 &lineno) const
  {
    const char * ret  = 0;
    if ( index >= 0 && index < mKeys.size() )
    {
      const KeyValue &v = mKeys[index];
      ret = v.getKey();
      lineno = v.getLineNo();
    }
    return ret;
  }

  const char *getValue(NxU32 index,NxU32 &lineno) const
  {
    const char * ret  = 0;
    if ( index >= 0 && index < mKeys.size() )
    {
      const KeyValue &v = mKeys[index];
      ret = v.getValue();
      lineno = v.getLineNo();
    }
    return ret;
  }

  void addKeyValue(const char *key,const char *value,NxU32 lineno)
  {
    KeyValue kv(key,value,lineno);
    mKeys.push_back(kv);
  }

  void save(FILE_INTERFACE *fph) const
  {
    if ( strcmp(mSection,"@HEADER") == 0 )
    {
    }
    else
    {
      fi_fprintf(fph,"\r\n");
      fi_fprintf(fph,"\r\n");
      fi_fprintf(fph,"[%s]\r\n", mSection );
    }
    for (NxU32 i=0; i<mKeys.size(); i++)
    {
      mKeys[i].save(fph);
    }
  }


  bool  addKeyValue(const char *key,const char *value) // adds a key-value pair.  These pointers *must* be persistent for the lifetime of the INI file!
  {
    bool ret = false;

    for (NxU32 i=0; i<mKeys.size(); i++)
    {
      KeyValue &kv = mKeys[i];
      if ( strcmp(kv.getKey(),key) == 0 )
      {
        kv.setValue(value);
        ret = true;
        break;
      }
    }

    if ( !ret )
    {
      KeyValue kv(key,value,0);
      mKeys.push_back(kv);
      ret = true;
    }

    return ret;
  }

  void reset(void)
  {
    mKeys.clear();
  }

private:
  NxU32 mLineNo;
  const char *mSection;
  KeyValueVector mKeys;
};

typedef USER_STL::vector< KeyValueSection *> KeyValueSectionVector;

class KeyValueIni : public InPlaceParserInterface
{
public:
  KeyValueIni(const char *fname)
  {
    mData.SetFile(fname);
    mData.SetCommentSymbol('#');
    mData.SetCommentSymbol('!');
    mData.SetCommentSymbol(';');
    mData.SetHard('=');
    mCurrentSection = 0;
    KeyValueSection *kvs = MEMALLOC_NEW(KeyValueSection)("@HEADER",0);
    mSections.push_back(kvs);
    mData.Parse(this);
  }

  KeyValueIni(const char *mem,NxU32 len)
  {
    if ( len )
    {
      mCurrentSection = 0;
      mData.SetSourceDataCopy(mem,len);

      mData.SetCommentSymbol('#');
      mData.SetCommentSymbol('!');
      mData.SetCommentSymbol(';');
      mData.SetHard('=');
      KeyValueSection *kvs = MEMALLOC_NEW(KeyValueSection)("@HEADER",0);
      mSections.push_back(kvs);
      mData.Parse(this);
    }
  }

  KeyValueIni(void)
  {
    mCurrentSection = 0;
    KeyValueSection *kvs = MEMALLOC_NEW(KeyValueSection)("@HEADER",0);
    mSections.push_back(kvs);
  }

  ~KeyValueIni(void)
  {
    reset();
  }

  void reset(void)
  {
    KeyValueSectionVector::iterator i;
    for (i=mSections.begin(); i!=mSections.end(); ++i)
    {
      KeyValueSection *kvs = (*i);
      delete kvs;
    }
    mSections.clear();
    mCurrentSection = 0;
  }

  NxU32 getSectionCount(void) const { return mSections.size(); };

	NxI32 ParseLine(NxI32 lineno,NxI32 argc,const char **argv)  // return TRUE to continue parsing, return FALSE to abort parsing process
  {

    if ( argc )
    {
      const char *key = argv[0];
      if ( key[0] == '[' )
      {
        key++;
        char *scan = (char *) key;
        while ( *scan )
        {
          if ( *scan == ']')
          {
            *scan = 0;
            break;
          }
          scan++;
        }
        mCurrentSection = -1;
        for (NxU32 i=0; i<mSections.size(); i++)
        {
          KeyValueSection &kvs = *mSections[i];
          if ( stricmp(kvs.getSection(),key) == 0 )
          {
            mCurrentSection = (NxI32) i;
            break;
          }
        }
        //...
        if ( mCurrentSection < 0 )
        {
          mCurrentSection = mSections.size();
          KeyValueSection *kvs = MEMALLOC_NEW(KeyValueSection)(key,lineno);
          mSections.push_back(kvs);
        }
      }
      else
      {
        const char *key = argv[0];
        const char *value = 0;
        if ( argc >= 2 )
          value = argv[1];
        mSections[mCurrentSection]->addKeyValue(key,value,lineno);
      }
    }

    return 0;
  }

  KeyValueSection * locateSection(const char *section,NxU32 &keys,NxU32 &lineno) const
  {
    KeyValueSection *ret = 0;
    for (NxU32 i=0; i<mSections.size(); i++)
    {
      KeyValueSection *s = mSections[i];
      if ( stricmp(section,s->getSection()) == 0 )
      {
        ret = s;
        lineno = s->getLineNo();
        keys = s->getKeyCount();
        break;
      }
    }
    return ret;
  }

  const KeyValueSection * getSection(NxU32 index,NxU32 &keys,NxU32 &lineno) const
  {
    const KeyValueSection *ret=0;
    if ( index >= 0 && index < mSections.size() )
    {
      const KeyValueSection &s = *mSections[index];
      ret = &s;
      lineno = s.getLineNo();
      keys = s.getKeyCount();
    }
    return ret;
  }

  bool save(const char *fname) const
  {
    bool ret = false;
    FILE_INTERFACE *fph = fi_fopen(fname,"wb");
    if ( fph )
    {
      for (NxU32 i=0; i<mSections.size(); i++)
      {
        mSections[i]->save(fph);
      }
      fi_fclose(fph);
      ret = true;
    }
    return ret;
  }

  void * saveMem(NxU32 &len) const
  {
    void *ret = 0;
    FILE_INTERFACE *fph = fi_fopen("mem","wmem");
    if ( fph )
    {
      for (NxU32 i=0; i<mSections.size(); i++)
      {
        mSections[i]->save(fph);
      }

      void *temp = fi_getMemBuffer(fph,len);
      if ( temp )
      {
        ret = MEMALLOC_MALLOC(len);
        memcpy(ret,temp,len);
      }

      fi_fclose(fph);
    }
    return ret;
  }


  KeyValueSection  *createKeyValueSection(const char *section_name,bool reset)  // creates, or locates and existing section for editing.  If reset it true, will erase previous contents of the section.
  {
    KeyValueSection *ret = 0;

    for (NxU32 i=0; i<mSections.size(); i++)
    {
      KeyValueSection *kvs = mSections[i];
      if ( strcmp(kvs->getSection(),section_name) == 0 )
      {
        ret = kvs;
        if ( reset )
        {
          ret->reset();
        }
        break;
      }
    }
    if ( ret == 0 )
    {
      ret = MEMALLOC_NEW(KeyValueSection)(section_name,0);
      mSections.push_back(ret);
    }

    return ret;
  }

private:
  NxI32                   mCurrentSection;
  KeyValueSectionVector mSections;
  InPlaceParser         mData;
};



KeyValueIni *loadKeyValueIni(const char *fname,NxU32 &sections)
{
  KeyValueIni *ret = 0;

  ret = MEMALLOC_NEW(KeyValueIni)(fname);
  sections = ret->getSectionCount();
  if ( sections < 2 )
  {
    delete ret;
    ret = 0;
  }

  return ret;
}

KeyValueIni *     loadKeyValueIni(const char *mem,NxU32 len,NxU32 &sections)
{
  KeyValueIni *ret = 0;

  ret = MEMALLOC_NEW(KeyValueIni)(mem,len);
  sections = ret->getSectionCount();
  if ( sections < 2 )
  {
    delete ret;
    ret = 0;
  }

  return ret;
}

const KeyValueSection * locateSection(const KeyValueIni *ini,const char *section,NxU32 &keys,NxU32 &lineno)
{
  KeyValueSection *ret = 0;

  if ( ini )
  {
    ret = ini->locateSection(section,keys,lineno);
  }

  return ret;
}

const KeyValueSection * getSection(const KeyValueIni *ini,NxU32 index,NxU32 &keycount,NxU32 &lineno)
{
  const KeyValueSection *ret = 0;

  if ( ini )
  {
    ret = ini->getSection(index,keycount,lineno);
  }

  return ret;
}

const char *      locateValue(const KeyValueSection *section,const char *key,NxU32 &lineno)
{
  const char *ret = 0;

  if ( section )
  {
    ret = section->locateValue(key,lineno);
  }

  return ret;
}

const char *      getKey(const KeyValueSection *section,NxU32 keyindex,NxU32 &lineno)
{
  const char *ret = 0;

  if ( section )
  {
    ret = section->getKey(keyindex,lineno);
  }

  return ret;
}

const char *      getValue(const KeyValueSection *section,NxU32 keyindex,NxU32 &lineno)
{
  const char *ret = 0;

  if ( section )
  {
    ret = section->getValue(keyindex,lineno);
  }

  return ret;
}

void              releaseKeyValueIni(const KeyValueIni *ini)
{
  KeyValueIni *k = (KeyValueIni *)ini;
  delete k;
}


const char *    getSectionName(const KeyValueSection *section)
{
  const char *ret = 0;
  if ( section )
  {
    ret = section->getSection();
  }
  return ret;
}


bool  saveKeyValueIni(const KeyValueIni *ini,const char *fname)
{
  bool ret = false;

  if ( ini )
    ret = ini->save(fname);

  return ret;
}

void *  saveKeyValueIniMem(const KeyValueIni *ini,NxU32 &len)
{
  void *ret = 0;

  if ( ini )
    ret = ini->saveMem(len);

  return ret;
}

KeyValueSection  *createKeyValueSection(KeyValueIni *ini,const char *section_name,bool reset)
{
  KeyValueSection *ret = 0;

  if ( ini )
  {
    ret = ini->createKeyValueSection(section_name,reset);
  }
  return ret;
}

bool  addKeyValue(KeyValueSection *section,const char *key,const char *value)
{
  bool ret = false;

  if ( section )
  {
    ret = section->addKeyValue(key,value);
  }

  return ret;
}


KeyValueIni      *createKeyValueIni(void) // create an empty .INI file in memory for editing.
{
  KeyValueIni *ret = MEMALLOC_NEW(KeyValueIni);
  return ret;
}

bool              releaseIniMem(void *mem)
{
  bool ret = false;
  if ( mem )
  {
    MEMALLOC_FREE(mem);
    ret = true;
  }
  return ret;
}


#define TEST_MAIN 0

#if TEST_MAIN
void main(NxI32 argc,const char **argv) // test to see if INI files work.
{
  const char *fname = "test.ini";
  NxU32 sections;
  const KeyValueIni *ini = loadKeyValueIni(fname,sections);
  if ( ini )
  {
    printf("INI file '%s' has %d sections.\r\n", fname, sections);
    for (NxU32 i=0; i<sections; i++)
    {
      NxU32 lineno;
      NxU32 keycount;
      const KeyValueSection *s = getSection(ini,i,keycount,lineno);
      assert(s);
      const char *sname  = getSectionName(s);
      assert(sname);
      printf("Section %d=%s starts at line number %d and contains %d keyvalue pairs\r\n", i+1, sname, lineno, keycount );
      for (NxU32 j=0; j<keycount; j++)
      {
        NxU32 lineno;
        const char *key = getKey(s,j,lineno);
        const char *value = getValue(s,j,lineno);
        if ( key && value )
        {
          printf("           %d  %s=%s\r\n", j, key , value );
        }
        else if ( key )
        {
          printf("           %d Key=%s\r\n", j, key );
        }
        else if ( value )
        {
          printf("           %d Value=%s\r\n", j, value );
        }
      }
    }
    releaseKeyValueIni(ini);
  }
  else
  {
    printf("Failed to load ini file '%s'\r\n", fname );
  }
}



#endif


