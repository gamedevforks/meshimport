#ifndef STRINGDICT_H
#define STRINGDICT_H

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


#include <vector>
#include <map>
#include <set>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <set>

#include "stable.h"

extern const char *emptystring;

class StringRef
{
public:
	StringRef(void)
	{
		mString = "";
	}

	inline StringRef(const char *str);
	inline StringRef(const StringRef &str);

	operator const char *() const
	{
		return mString;
	}

	const char * Get(void) const { return mString; };

	void Set(const char *str)
	{
		mString = str;
	}

	const StringRef &operator= (const StringRef& rhs )
	{
		mString = rhs.Get();
		return *this;
	}

	bool operator== ( const StringRef& rhs ) const
	{
		return rhs.mString == mString;
	}

	bool operator< ( const StringRef& rhs ) const
	{
		return rhs.mString < mString;
	}

	bool operator!= ( const StringRef& rhs ) const
	{
		return rhs.mString != mString;
	}

	bool operator> ( const StringRef& rhs ) const
	{
		return rhs.mString > mString;
	}

	bool operator<= ( const StringRef& rhs ) const
	{
		return rhs.mString < mString;
	}

	bool operator>= ( const StringRef& rhs ) const
	{
		return rhs.mString >= mString;
	}

	bool SamePrefix(const char *prefix) const
	{
		unsigned int len = (unsigned int)strlen(prefix);
		if ( len && strncmp(mString,prefix,len) == 0 ) return true;
		return false;
	}

	bool SameSuffix(const StringRef &suf) const
	{
		const char *source = mString;
		const char *suffix = suf.mString;
		unsigned int len1 = (unsigned int)strlen(source);
		unsigned int len2 = (unsigned int)strlen(suffix);
		if ( len1 < len2 ) return false;
		const char *compare = &source[(len1-len2)];
		if ( strcmp(compare,suffix) == 0 ) return true;
		return false;
	}

private:
	const char *mString; // the actual char ptr
};


class StringDict
{
public:
	StringDict(void)
	{
	}

	~StringDict(void)
	{
	}

	StringRef Get(const char *text)
	{
		StringRef ref;
		if ( text )
		{
			if ( strcmp(text,emptystring) == 0 )
			{
				ref.Set(emptystring);
			}
			else
			{
				if ( strcmp(text,emptystring) == 0 )
				{
					ref.Set(emptystring);
				}
				else
				{
					const char *foo = mStringTable.Get(text);
					ref.Set(foo);
				}
			}
		}
		return ref;
	}

	StringRef Get(const char *text,bool &first)
	{
		StringRef ref;
		const char *foo = mStringTable.Get(text,first);
		ref.Set(foo);
		return ref;
	}

private:
	StringTable mStringTable;
};

extern StringDict *gStringDict;

typedef USER_STL::vector< StringRef  >    StringRefVector;
typedef USER_STL::set< StringRef >        StringRefSet;

inline StringRef SGET(const char *foo)
{
	if ( !gStringDict )
	{
		gStringDict = MEMALLOC_NEW(StringDict);
	}
	return gStringDict->Get(foo);
}

inline StringRef::StringRef(const char *str)
{
	StringRef ref = SGET(str);
	mString = ref.mString;
}

inline StringRef::StringRef(const StringRef &str)
{
	mString = str.Get();
}

// This is a helper class so you can easily do an alphabetical sort on an STL vector of StringRefs.
// Usage: USER_STL::sort( list.begin(), list.end(), StringSortRef() );
class StringSortRef
{
	public:

	 bool operator()(const StringRef &a,const StringRef &b) const
	 {
		 const char *str1 = a.Get();
		 const char *str2 = b.Get();
		 int r = stricmp(str1,str2);
		 return r < 0;
	 }
};

#endif
