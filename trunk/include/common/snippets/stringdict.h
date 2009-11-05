#ifndef NV_STRINGDICT_H
#define NV_STRINGDICT_H

#include "UserMemAlloc.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "stable.h"

namespace NVSHARE
{

extern const char *emptystring;

class StringRef
{
public:
	StringRef(void)
	{
		mString = "";
	}

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
		NxU32 len = (NxU32)strlen(prefix);
		if ( len && strncmp(mString,prefix,len) == 0 ) return true;
		return false;
	}

	bool SameSuffix(const StringRef &suf) const
	{
		const char *source = mString;
		const char *suffix = suf.mString;
		NxU32 len1 = (NxU32)strlen(source);
		NxU32 len2 = (NxU32)strlen(suffix);
		if ( len1 < len2 ) return false;
		const char *compare = &source[(len1-len2)];
		if ( strcmp(compare,suffix) == 0 ) return true;
		return false;
	}

#ifdef NX64
  NxU64 getHash(void) const
  {
    return (NxU64) mString;
  }
#else
#pragma warning(push)
#pragma warning(disable:4311)
  NxU32 getHash(void) const
  {
    return (NxU32) mString;
  }
#pragma warning(pop)
#endif

private:
	const char *mString; // the actual char ptr
};


class StringDict : public Memalloc
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
					bool first;
					const char *foo = mStringTable.Get(text,first);
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

  void setCaseSensitive(bool state)
  {
    mStringTable.setCaseSensitive(state);
  }

private:
	StringTable mStringTable;
};

typedef Array< StringRef  >  StringRefVector;

inline StringRef::StringRef(const StringRef &str)
{
	mString = str.Get();
}

// This is a helper class so you can easily do an alphabetical sort on an STL vector of StringRefs.
// Usage: USER_STL::sort( list.begin(), list.end(), StringSortRef() );
class StringSortRef : public Memalloc
{
	public:

	 bool operator()(const StringRef &a,const StringRef &b) const
	 {
		 const char *str1 = a.Get();
		 const char *str2 = b.Get();
		 NxI32 r = stricmp(str1,str2);
		 return r < 0;
	 }
};

extern StringDict *gStringDict;

};




static inline NVSHARE::StringDict * getGlobalStringDict(void)
{
	if ( NVSHARE::gStringDict == 0 )
		NVSHARE::gStringDict = new NVSHARE::StringDict;
	return NVSHARE::gStringDict;
}

#define SGET(x) getGlobalStringDict()->Get(x)

#endif
