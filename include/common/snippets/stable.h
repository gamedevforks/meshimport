#ifndef STABLE_H

#define STABLE_H

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




#pragma warning(disable:4786)
#pragma warning(disable:4995)
#pragma warning(disable:4996)


#include <assert.h>
#include <string.h>
#include <string>
#include <set>

#include "UserMemAlloc.h"

#if defined(LINUX)
#define stricmp strcasecmp
#endif
class CharPtrLess
{
public:
	bool operator()(const char *v1,const char *v2) const
	{
		int v = strcmp(v1,v2);
		if ( v < 0 ) return true;
		return false;
	};
};

#if HE_USE_MEMORY_TRACKING
typedef USER_STL::set< const char *, USER_STL::GlobalMemoryPool, CharPtrLess > CharPtrSet;
#else
typedef USER_STL::set< const char *, CharPtrLess > CharPtrSet;
#endif

class StringTable
{
public:
	StringTable(void)
	{
	};

	~StringTable(void)
	{
		CharPtrSet::iterator i;
		for (i=mStrings.begin(); i!=mStrings.end(); i++)
		{
			char *str = (char *)(*i);
			MEMALLOC_FREE(str);
		}
	}

	const char * Get(const char *str)
	{
    const char *ret = 0;
    if ( str )
    {
		  CharPtrSet::iterator found;
		  found = mStrings.find( str );
		  if ( found != mStrings.end() ) return (*found);
		  unsigned int l = (unsigned int)strlen(str);
		  char *mem = (char *) MEMALLOC_MALLOC(sizeof(char)*(l+1));
		  strcpy(mem,str);
		  mStrings.insert( mem );
      ret = mem;
    }
		return ret;
	};

	const char * Get(const char *str,bool &first)
	{
		CharPtrSet::iterator found;
		found = mStrings.find( str );
		if ( found != mStrings.end() )
		{
			first = false;
			return (*found);
		}
		first = true;
		unsigned int l = (unsigned int)strlen(str);
		char *mem = (char *) MEMALLOC_MALLOC(sizeof(char)*(l+1));
		strcpy(mem,str);
		mStrings.insert( mem );
		return mem;
	};

	CharPtrSet& GetSet(void) { return mStrings; };

private:
	CharPtrSet mStrings;
};


class CharPtrInt
{
public:
	const char *mString;
	unsigned int      mId;
};

class CharPtrIntLess1
{
public:
	bool operator()(const CharPtrInt &v1,const CharPtrInt &v2) const
	{
		int v = strcmp(v1.mString,v2.mString);
		if ( v < 0 ) return true;
		return false;
	};
};

class CharPtrIntLess2
{
public:
	bool operator()(const CharPtrInt &v1,const CharPtrInt &v2) const
	{
		int v = stricmp(v1.mString,v2.mString);
		if ( v < 0 ) return true;
		return false;
	};
};

#if HE_USE_MEMORY_TRACKING
typedef USER_STL::set< CharPtrInt, USER_STL::GlobalMemoryPool,  CharPtrIntLess1 > CharPtrIntSet1;
typedef USER_STL::set< CharPtrInt, USER_STL::GlobalMemoryPool,  CharPtrIntLess2 > CharPtrIntSet2;
#else
typedef USER_STL::set< CharPtrInt, CharPtrIntLess1 > CharPtrIntSet1;
typedef USER_STL::set< CharPtrInt, CharPtrIntLess2 > CharPtrIntSet2;
#endif

class StringTableInt
{
public:
	StringTableInt(void)
	{
		mCase = true;
	}

	unsigned int Get(const char *str) const
	{
		unsigned int ret = 0;
		if ( mCase )
		{
			CharPtrIntSet1::const_iterator found;
			CharPtrInt cpi;
			cpi.mString = str;
			cpi.mId     = 0;
			found = mStrings1.find( cpi );
			if ( found != mStrings1.end() )
				ret = (*found).mId;
		}
		else
		{
			CharPtrIntSet2::const_iterator found;
			CharPtrInt cpi;
			cpi.mString = str;
			cpi.mId     = 0;
			found = mStrings2.find( cpi );
			if ( found != mStrings2.end() )
				ret = (*found).mId;
		}
		return ret;
	};

	bool Get(const char *str,unsigned int &ret) const
	{
		bool rfound = false;

		if ( mCase )
		{
			CharPtrIntSet1::const_iterator found;
			CharPtrInt cpi;
			cpi.mString = str;
			cpi.mId     = 0;
			found = mStrings1.find( cpi );
			if ( found != mStrings1.end() )
      {
				ret = (*found).mId;
        rfound = true;
      }
		}
		else
		{
			CharPtrIntSet2::const_iterator found;
			CharPtrInt cpi;
			cpi.mString = str;
			cpi.mId     = 0;
			found = mStrings2.find( cpi );
			if ( found != mStrings2.end() )
      {
				ret = (*found).mId;
        rfound = true;
      }
		}
		return rfound;
	};

	void Add(const char *foo,unsigned int id)
	{
		CharPtrInt cpi;
		cpi.mString = foo;
		cpi.mId     = id;
		if ( mCase )
			mStrings1.insert(cpi);
		else
			mStrings2.insert(cpi);
	}

	const char * Get(unsigned int id) const
	{
		const char *ret = 0;
		if ( mCase )
		{
			CharPtrIntSet1::const_iterator i;
			for (i=mStrings1.begin(); i!=mStrings1.end(); ++i)
			{
				if ( (*i).mId == id )
				{
					ret = (*i).mString;
					break;
				}
			}
		}
		else
		{
			CharPtrIntSet2::const_iterator i;
			for (i=mStrings2.begin(); i!=mStrings2.end(); ++i)
			{
				if ( (*i).mId == id )
				{
					ret = (*i).mString;
					break;
				}
			}
		}
		return ret;
	}

	void SetCaseSensitive(bool s)
	{
		mCase = s;
	}

private:
	bool        mCase;
	CharPtrIntSet1 mStrings1;     // case sensitive
	CharPtrIntSet2 mStrings2;     // case insensitive
};


#endif
