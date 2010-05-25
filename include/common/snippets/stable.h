#ifndef NV_STRING_TABLE_H

#define NV_STRING_TABLE_H

#pragma warning(disable:4786)
#pragma warning(disable:4995)
#pragma warning(disable:4996)

#include "safestdio.h"
#include <assert.h>
#include <string.h>

#include "UserMemAlloc.h"
#include "NvHashMap.h"

#if defined(LINUX)
#define stricmp strcasecmp
#endif

namespace NVSHARE
{

class CRC32
{
public:
  CRC32(void)
  {
    const NxU32 QUOTIENT=0x04c11db7;
    for (NxU32 i = 0; i < 256; i++)
    {
      NxU32 crc = i << 24;
      for (NxU32 j = 0; j < 8; j++)
      {
        if (crc & 0x80000000)
          crc = (crc << 1) ^ QUOTIENT;
        else
          crc = crc << 1;
      }
      mCRCTable[i] = myhtonl(crc);
    }
    mIsLittleEndian = !isBigEndian();
  }


  inline NxU32 myhtonl(NxU32 n_ecx)
  {
    NxU32 n_eax = n_ecx;           //mov         eax,ecx
    NxU32 n_edx = n_ecx;           //mov         edx,ecx
    n_edx = n_edx << 16;           //shl         edx,10h
    n_eax = n_eax & 0x0FF00;       //and         eax,0FF00h
    n_eax = n_eax | n_edx;         //or          eax,edx
    n_edx = n_ecx;                 // mov         edx,ecx
    n_edx = n_edx & 0x0FF0000;     //and edx,0FF0000h
    n_ecx = n_ecx >> 16;           //shr         ecx,10h
    n_edx = n_edx | n_ecx;         //or          edx,ecx
    n_eax = n_eax << 8;            //shl         eax,8
    n_edx = n_edx >> 8;            //shr         edx,8
    n_eax|=n_edx;                  //  71AB2BE9  or          eax,edx
    return n_eax;
  }

  inline void getRand(NxU32 &current) const
  {
    current = (current * 214013L + 2531011L) & 0x7fffffff;
  };

  NxU32 crc32(const NxU8 *data, NxU32 len) const
  {
    NxU32 ret;

    ret =  crc32Internal(data,len);

    return ret;
  }

  NxU32 crc32(const char *data) const
  {
    NxU32 len = (NxU32)strlen(data);
    return crc32( (const NxU8 *)data,len);
  }

private:

  bool inline isBigEndian() { int i = 1; return *((char*)&i)==0; }

  NxU32 crc32Internal(const NxU8 *data,NxU32 len) const
  {
    NxU32        dlen = (len/4)*4;
    NxU32        result=len;
    NxU32        *p = (NxU32 *)data;
    NxU32        *e = (NxU32 *)(data + dlen);
    NxU32         current = len;


	if ( dlen >= 4 )
	{
		result = ~*p++;
		result = result ^ len;

		const NxU32 *tmp = (const NxU32 *) data;
		current = *tmp & len;

		if ( mIsLittleEndian )
		{
		  while( p<e )
		  {
			getRand(current);
			result = mCRCTable[result & 0xff] ^ result >> 8;
			result = mCRCTable[result & 0xff] ^ result >> 8;
			result = mCRCTable[result & 0xff] ^ result >> 8;
			result = mCRCTable[result & 0xff] ^ result >> 8;
			result ^= *p++;
			result ^= current;
			current &= result; // feed the result back into the random number seed, this forces the random sequence to drift with the input charcter stream.
		  }
		}
		else
		{
		  while( p<e )
		  {
			getRand(current);
			result = mCRCTable[result >> 24] ^ result << 8;
			result = mCRCTable[result >> 24] ^ result << 8;
			result = mCRCTable[result >> 24] ^ result << 8;
			result = mCRCTable[result >> 24] ^ result << 8;
			result ^= *p++;
			result ^= current;
			current &= result; // feed the result back into the random number seed, this forces the random sequence to drift with the input charcter stream.
		  }
	  }


    }
    NxU32 partial = len&3;
    if ( partial )
    {
      for (NxU32 i=0; i<partial; i++)
      {
          getRand(current);
          NxU8 v = data[dlen+i];
          result = mCRCTable[v]^result;
          result^=current;
          current &= result;
      }
    }
    return ~result;
  }

  bool  mIsLittleEndian;
  NxU32 mCRCTable[256];
};



class StringHash : public Memalloc
{
public:
  StringHash(const char *str)
  {
    mNextHash = 0;
    NxU32 len = (NxU32)strlen(str);
    mString = (char *)MEMALLOC_MALLOC(len+1);
    strcpy(mString,str);
  }

  ~StringHash(void)
  {
    MEMALLOC_FREE(mString);
  }

  inline const char * getString(void) const { return mString; };
  inline StringHash * getNextHash(void) const { return mNextHash; };
  inline void         setNextHash(StringHash *sh) { mNextHash = sh; };
  inline bool         match(const char *str,bool caseSensitive) const 
  { 
	  return caseSensitive ? (strcmp(mString,str) == 0) : (stricmp(mString,str) == 0);
  };

private:
  StringHash *mNextHash;
  char       *mString;
};


#ifdef __CELLOS_LV2__
#define stdext std
#endif

class StringTable : public Memalloc, public CRC32
{
public:

    typedef HashMap< NxU32, StringHash *> StringHashMap;

	StringTable(void)
	{
        mCaseSensitive = true;
	};

	~StringTable(void)
	{
    	for (StringHashMap::Iterator i=mStrings.getIterator(); !i.done(); ++i)
        {
            StringHash *sh = (*i).second;
            while ( sh )
            {
                StringHash *next = sh->getNextHash();
                delete sh;
                sh = next;
            }
        }
	}

	const char * Get(const char *str,bool &first)
	{
		first = false;
        const char *ret=0;

        NxU32 hash;

        if ( !mCaseSensitive )
        {
            char temp_string[8192];
            strncpy(temp_string,str,8192);
            strlwr(temp_string);
            hash = crc32(temp_string);
        }
        else
        {
          hash = crc32(str);
        }

        const StringHashMap::Entry *found = mStrings.find(hash);

        if ( found == NULL )
        {
            StringHash *sh = MEMALLOC_NEW(StringHash)(str);
            mStrings[hash] = sh;
            ret = sh->getString();
			first = true;
        }
        else
        {
            StringHash *sh = (*found).second;
            while ( sh )
            {
                if ( sh->match(str,mCaseSensitive) )
                {
                    ret = sh->getString();
                    break;
                }
                sh = sh->getNextHash();
            }
            if ( !ret )
            {
                StringHash *nh = new StringHash(str);
                sh = (*found).second;
                nh->setNextHash(sh);
				mStrings.erase(hash); // erase the old hash.
				mStrings[hash] = nh;  // assign the new hash
				ret = sh->getString();
				first = true;
            }
        }
        return ret;
	};


    void setCaseSensitive(bool state)
    {
        mCaseSensitive = state;
    }

private:
    bool                mCaseSensitive;
    StringHashMap       mStrings;
};

class StringIntHash : public Memalloc
{
public:
  StringIntHash(const char *str,NxU32 id)
  {
    mNextHash = 0;
    mId       = id;
    NxU32 len = (NxU32)strlen(str);
    mString = (char *)MEMALLOC_MALLOC(len+1);
    strcpy(mString,str);
  }

  ~StringIntHash(void)
  {
    MEMALLOC_FREE(mString);
  }

  inline const char *    getString(void) const { return mString; };
  inline StringIntHash * getNextHash(void) const { return mNextHash; };
  inline void            setNextHash(StringIntHash *sh) { mNextHash = sh; };

  inline bool         match(const char *str,bool caseSensitive) const
  { 
	  return caseSensitive ? (strcmp(mString,str) == 0) : (stricmp(mString,str) == 0);
  };

  inline NxU32           getId(void) const { return mId; };

private:
  StringIntHash *mNextHash;
  NxU32          mId;
  char          *mString;
};


class StringTableInt : public Memalloc, public CRC32
{
public:

    typedef HashMap< NxU32, StringIntHash *>  StringIntHashMap;
    typedef HashMap< NxU32, StringIntHash * > IntCharHashMap;

	StringTableInt(void)
	{
		mCaseSensitive = false;
  }
  ~StringTableInt(void)
  {
		for (StringIntHashMap::Iterator i=mStrings.getIterator(); !i.done(); ++i)
        {
            StringIntHash *sh = (*i).second;
            while ( sh )
            {
                StringIntHash *next = sh->getNextHash();
                delete sh;
                sh = next;
            }
        }
	}

	bool Get(const char *str,NxU32 &id) const
	{
		id = Get(str);
		return id != 0;
	}

	NxU32 Get(const char *str) const
	{
		NxU32 ret = 0;

        if ( str )
        {
          NxU32 hash;

          if ( !mCaseSensitive )
          {
              char temp_string[8192];
              strncpy(temp_string,str,8192);
              strlwr(temp_string);
              hash = crc32(temp_string);
          }
          else
          {
              hash = crc32(str); // compute the hash value.
          }
          const StringIntHashMap::Entry *found = mStrings.find(hash);
          if ( found != NULL )
          {
              const StringIntHash *sh = (*found).second;
              while ( sh )
              {
                  if ( sh->match(str,mCaseSensitive) )
                  {
                      ret = sh->getId();
                      break;
                  }
                  sh = sh->getNextHash();
              }
          }
        }


		return ret;
	};

	void Add(const char *str,NxU32 id)
	{
        StringIntHash *sh;
        char temp_string[8192];

        NxU32 hash;
        if ( !mCaseSensitive )
        {
            strncpy(temp_string,str,8192);
            strlwr(temp_string);
            hash = crc32(temp_string);
        }
        else
        {
          hash = crc32(str);
        }
        const StringIntHashMap::Entry *found = mStrings.find(hash);
        if ( found == NULL )
        {
            sh = MEMALLOC_NEW(StringIntHash)(str,id);
            mStrings[hash] = sh;
        }
        else
        {
            sh = (*found).second;
            while ( sh )
            {
                if ( sh->match(str,mCaseSensitive) )
                {
                    assert(0); // same string mapped to multiple ids!!
                    break;
                }
                sh = sh->getNextHash();
            }
            if ( !sh )
            {
                StringIntHash *nh = MEMALLOC_NEW(StringIntHash)(str,id);
                sh = (*found).second;
                nh->setNextHash(sh);
				mStrings.erase(hash);
				mStrings[hash] = nh;
                sh = nh;
            }
        }
        mIds[id] = sh;
	}

	const char * Get(NxU32 id) const
	{
		const char *ret = 0;

        const IntCharHashMap::Entry *found = mIds.find(id);
        if ( found != NULL )
            ret = (*found).second->getString();

		return ret;
	}

	void SetCaseSensitive(bool s)
	{
		mCaseSensitive = s;
	}


    IntCharHashMap      mIds;
private:
    bool                mCaseSensitive;
    StringIntHashMap    mStrings;
};

}; // end of namespace

#endif
