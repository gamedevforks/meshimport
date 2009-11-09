#ifndef __CELLOS_LV2__
#include <assert.h>
#include <ctype.h>

#pragma warning(disable:4211)
#pragma warning(disable:4244)
#pragma warning(disable:4189)
#pragma warning(disable:4100)
#pragma warning(disable:4706)
#pragma warning(disable:4702)
#pragma warning(disable:4267)

#include "UserMemAlloc.h"
#include <list>
#include <queue>
#include <map>

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
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
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliffscarab@gmail.com
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, freeof charge, to any person obtaining a copy
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

//***********************************************************************************
//*** This code snippet allows you to embed a telnet client or server easily into any
//*** of your applications.  Currently this code only builds for Windows and the XBOX
//*** It is very desirable to me to have this code build for APPLE, Linux, and Iphone
//*** If you wish to contribute to this cause, please let me know and I will add you
//*** as a developer to the google code page.  http://code.google.com/p/telnet
//***
//*** To test the program simply run 'telnet.exe'.
//***
//*** The first time you run it, it will be set up as a telnet server.  You can
//*** connect to it by runing a telnet client and typing 'open localhost 23'
//*** If you run the program a second time, it will detect that a server is already
//*** using Port 23 and will instead start up as a client.  You can now send messages
//*** between the two instances of the application.
//*** You can keep launching as many of them as you wish.
//***********************************************************************************


#if defined(_XBOX)
	#include "NxXBOX.h"
	#include <winsockx.h>
#endif

#if defined(WIN32)
	#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x400
	#endif
	#include <windows.h>
#endif

#if defined(_XBOX)
	#include "NxXBOX.h"
#endif

#if defined(__APPLE__) || defined(LINUX)
	#include <pthread.h>
#endif

#include "telnet.h"

// Common Telnet Functionality.
#ifdef WIN32
#pragma warning(disable:4786 4996)
#pragma comment(lib,"wsock32.lib")
#endif

namespace NVSHARE
{
Telnet *gTelnet=0; // optional global variable representing the TELNET singleton for the application.
};

#define TELNET_NVSHARE TELNET_##NVSHARE

namespace TELNET_NVSHARE
{

///*************** The FastXML code snippet

class FastXmlInterface
{
public:

  // return true to continue processing the XML document, false to skip.
  virtual bool processElement(const char *elementName,         // name of the element
                              NxI32         argc,                // number of attributes
                              const char **argv,               // list of attributes.
                              const char  *elementData,        // element data, null if none
                              NxI32         lineno) = 0;         // line number in the source XML file

};

class FastXml
{
public:
  virtual bool processXml(const char *inputData,NxU32 dataLen,FastXmlInterface *iface) = 0;
  virtual const char * getError(NxI32 &lineno) = 0; // report the reason for a parsing error, and the line number where it occurred.
};

FastXml * createFastXml(void);
void      releaseFastXml(FastXml *f);

class MyFastXml : public FastXml, public NVSHARE::Memalloc
{
public:
  enum CharType
  {
    CT_DATA,
    CT_EOF,
    CT_SOFT,
    CT_END_OF_ELEMENT, // either a forward slash or a greater than symbol
    CT_END_OF_LINE,
  };

  MyFastXml(void)
  {
    mInputData = 0;
    memset(mTypes,CT_DATA,256);
    mTypes[0] = CT_EOF;
    mTypes[32] = CT_SOFT;
    mTypes[9] = CT_SOFT;
    mTypes['/'] = CT_END_OF_ELEMENT;
    mTypes['>'] = CT_END_OF_ELEMENT;
    mTypes['?'] = CT_END_OF_ELEMENT;
    mTypes[10] = CT_END_OF_LINE;
    mTypes[13] = CT_END_OF_LINE;
    mError = 0;
  }
  virtual ~MyFastXml(void)
  {
    release();
  }

  void release(void)
  {
    if ( mInputData )
    {
      free(mInputData);
      mInputData = 0;
    }
    mError = 0;
  }

  inline char *nextSoft(char *scan)
  {
    while ( *scan && mTypes[*scan] != CT_SOFT ) scan++;
    return scan;
  }

  inline char *nextSoftOrClose(char *scan,bool &close)
  {
    while ( *scan && mTypes[*scan] != CT_SOFT && *scan != '>' ) scan++;
    close = *scan == '>';
    return scan;
  }

  inline char *nextSep(char *scan)
  {
    while ( *scan && mTypes[*scan] != CT_SOFT && *scan != '=' ) scan++;
    return scan;
  }

  inline char * skipNextData(char *scan)
  {
    // while we have data, and we encounter soft seperators or line feeds...
    while ( *scan && mTypes[*scan] == CT_SOFT || mTypes[*scan] == CT_END_OF_LINE )
    {
      if ( *scan == 13 ) mLineNo++;
      scan++;
    }
    return scan;
  }

  char * processClose(char c,const char *element,char *scan,NxI32 argc,const char **argv,FastXmlInterface *iface)
  {
    if ( c == '/' || c == '?' )
    {
      if ( *scan != '>' ) // unexepected character!
      {
        mError = "Expected an element close character immediately after the '/' or '?' character.";
        return 0;
      }
      scan++;
      bool ok = iface->processElement(element,argc,argv,0,mLineNo);
      if ( !ok )
      {
        mError = "User aborted the parsing process";
        return 0;
      }
    }
    else
    {
      scan = skipNextData(scan);
      char *data = scan; // this is the data portion of the element, only copies memory if we encounter line feeds
      char *dest_data = 0;
      while ( *scan && *scan != '<' )
      {
        if ( mTypes[*scan] == CT_END_OF_LINE )
        {
          if ( *scan == 13 ) mLineNo++;
          dest_data = scan;
          *dest_data++ = 32; // replace the linefeed with a space...
          scan = skipNextData(scan);
          while ( *scan && *scan != '<' )
          {
            if ( mTypes[*scan] == CT_END_OF_LINE )
            {
             if ( *scan == 13 ) mLineNo++;
             *dest_data++ = 32; // replace the linefeed with a space...
              scan = skipNextData(scan);
            }
            else
            {
              *dest_data++ = *scan++;
            }
          }
          break;
        }
        else
          scan++;
      }
      if ( *scan == '<' )
      {
        if ( dest_data )
        {
          *dest_data = 0;
        }
        else
        {
          *scan = 0;
        }
        scan++; // skip it..
        if ( *data == 0 ) data = 0;
        bool ok = iface->processElement(element,argc,argv,data,mLineNo);
        if ( !ok )
        {
          mError = "User aborted the parsing process";
          return 0;
        }
        if ( *scan == '/' )
        {
          while ( *scan && *scan != '>' ) scan++;
          scan++;
        }
      }
      else
      {
        mError = "Data portion of an element wasn't terminated properly";
        return 0;
      }
    }
    return scan;
  }

  virtual bool processXml(const char *inputData,NxU32 dataLen,FastXmlInterface *iface)
  {
    bool ret = true;

    #define MAX_ATTRIBUTE 2048 // can't imagine having more than 2,048 attributes in a single element right?

    release();
    mInputData = (char *)malloc(dataLen+1);
    memcpy(mInputData,inputData,dataLen);
    mInputData[dataLen] = 0;

    mLineNo = 1;

    char *element;

    char *scan = mInputData;
    if ( *scan == '<' )
    {
      scan++;
      while ( *scan )
      {
        scan = skipNextData(scan);
        if ( *scan == 0 ) return ret;
        if ( *scan == '<' )
        {
          scan++;
        }
        if ( *scan == '/' || *scan == '?' )
        {
          while ( *scan && *scan != '>' ) scan++;
          scan++;
        }
        else
        {
          element = scan;
          NxI32 argc = 0;
          const char *argv[MAX_ATTRIBUTE];
          bool close;
          scan = nextSoftOrClose(scan,close);
          if ( close )
          {
            char c = *(scan-1);
            if ( c != '?' && c != '/' )
            {
              c = '>';
            }
            *scan = 0;
            scan++;
            scan = processClose(c,element,scan,argc,argv,iface);
            if ( !scan ) return false;
          }
          else
          {
            if ( *scan == 0 ) return ret;
            *scan = 0; // place a zero byte to indicate the end of the element name...
            scan++;

            while ( *scan )
            {
              scan = skipNextData(scan); // advance past any soft seperators (tab or space)

              if ( mTypes[*scan] == CT_END_OF_ELEMENT )
              {
                char c = *scan++;
                scan = processClose(c,element,scan,argc,argv,iface);
                if ( !scan ) return false;
                break;
              }
              else
              {
                if ( argc >= MAX_ATTRIBUTE )
                {
                  mError = "encountered too many attributes";
                  return false;
                }
                argv[argc] = scan;
                scan = nextSep(scan);  // scan up to a space, or an equal
                if ( *scan )
                {
                  if ( *scan != '=' )
                  {
                    *scan = 0;
                    scan++;
                    while ( *scan && *scan != '=' ) scan++;
                    if ( *scan == '=' ) scan++;
                  }
                  else
                  {
                    *scan=0;
                    scan++;
                  }
                  if ( *scan ) // if not eof...
                  {
                    scan = skipNextData(scan);
                    if ( *scan == 34 )
                    {
                      scan++;
                      argc++;
                      argv[argc] = scan;
                      argc++;
                      while ( *scan && *scan != 34 ) scan++;
                      if ( *scan == 34 )
                      {
                        *scan = 0;
                        scan++;
                      }
                      else
                      {
                        mError = "Failed to find closing quote for attribute";
                        return false;
                      }
                    }
                    else
                    {
                      mError = "Expected quote to begin attribute";
                      return false;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else
    {
      mError = "Expected the start of an element '<' at this location.";
      ret = false; // unexpected character!?
    }

    return ret;
  }

  const char * getError(NxI32 &lineno)
  {
    const char *ret = mError;
    lineno = mLineNo;
    mError = 0;
    return ret;
  }

private:
  char         mTypes[256];
  char        *mInputData;
  NxI32          mLineNo;
  const char  *mError;
};



FastXml * createFastXml(void)
{
  MyFastXml *f = MEMALLOC_NEW(MyFastXml);
  return static_cast< FastXml *>(f);
}

void      releaseFastXml(FastXml *f)
{
  MyFastXml *m = static_cast< MyFastXml *>(f);
  delete m;
}

//**** The BlobIO code snippet

class BlobIOInterface
{
public:
  virtual void sendBlobText(NxU32 client,const char *fmt,...) = 0;
};

class BlobIO
{
public:
  virtual bool sendBlob(NxU32 client,const char *blobType,const void *blobData,NxU32 blobLen) = 0;
  virtual const char * receiveBlob(NxU32 &client,const void *&data,NxU32 &dlen) = 0;
  virtual bool processIncomingBlobText(NxU32 client,const char *text) = 0;
protected:
  BlobIO(void) { };
};


BlobIO * createBlobIO(BlobIOInterface *iface);
void     releaseBlobIO(BlobIO *b);

#define BLOB_LINE 256

static char gHexTable[16] = { '0', '1', '2', '3','4','5','6','7','8','9','A','B','C','D','E','F' };

static inline char getHex(NxU8 c)
{
    return gHexTable[c];
}

#pragma warning(disable:4100)

static inline bool getHex(char c,NxU8 &v)
{
    bool ret = true;
    if ( c >= '0' && c <= '9' )
    {
        v = c-'0';
    }
    else if ( c >= 'A' && c <= 'F' )
    {
        v = (c-'A')+10;
    }
    else
    {
        ret = false;
    }
    return ret;
}

static inline bool getHexValue(char c1,char c2,NxU8 &v)
{
    bool ret = false;
    NxU8 v1,v2;
    if ( getHex(c1,v1) && getHex(c2,v2) )
    {
        v = v1<<4 | v2;
        ret = true;
    }
    return ret;
}

class Blob : public NVSHARE::Memalloc
{
public:
  Blob(const char *blobType,NxU32 client,NxU32 blobId,NxU32 olen,const char *data)
  {
    NxU32 slen = strlen(blobType);
    mClient   = client;
    mFinished = false;
    mError    = false;
    mBlobType = (char *)MEMALLOC_MALLOC(slen+1);
    strcpy(mBlobType,blobType);
    mBlobId = blobId;
    mBlobLen  = olen;
    mBlobData = (NxU8 *)MEMALLOC_MALLOC(olen);
    mBlobIndex = 0;
    addData(data);
  }

  ~Blob(void)
  {
    MEMALLOC_FREE(mBlobType);
    MEMALLOC_FREE(mBlobData);
  }

  void addData(const char *data)
  {

    while ( mBlobIndex < mBlobLen && *data )
    {
        char c1 = data[0];
        char c2 = data[1];

        if ( getHexValue(c1,c2,mBlobData[mBlobIndex]) )
        {
            mBlobIndex++;
        }
        else
        {
            break;
        }
        data+=2;
    }

    if ( mBlobIndex == mBlobLen )
    {
        mFinished = true;
    }
  }

  void addDataEnd(const char *data)
  {
    addData(data);
    assert( mFinished );
  }

  NxU32 getId(void) const { return mBlobId; };

  bool           mFinished;
  bool           mError;
  char          *mBlobType;
  NxU32   mBlobId;
  NxU32   mBlobLen;
  NxU8 *mBlobData;
  NxU32   mBlobIndex;
  NxU32   mClient;
};

typedef std::list< Blob * > BlobList;

class MyBlobIO : public BlobIO, public FastXmlInterface, public NVSHARE::Memalloc
{
public:
  MyBlobIO(BlobIOInterface *iface)
  {
    mCallback = iface;
    mBlobId   = 0;
    mLastBlob = 0;
    mFastXml = createFastXml();
  }

  virtual ~MyBlobIO(void)
  {
    releaseFastXml(mFastXml);
    BlobList::iterator i;
    for (i=mBlobs.begin(); i!=mBlobs.end(); ++i)
    {
        Blob *b = (*i);
        delete b;
    }
    delete mLastBlob;
  }

  // convert a blob of binary data into multiple lines of ascii data
  virtual bool sendBlob(NxU32 client,const char *blobType,const void *blobData,NxU32 blobLen)
  {
	bool ret = false;
    if ( mCallback && blobLen > 0 )
    {
		assert(blobType);
		assert(blobData);
        if ( blobLen <= BLOB_LINE )
        {
            char blobText[BLOB_LINE*2+1];
            const NxU8 *scan = (const NxU8 *)blobData;
            char *dest = blobText;
            for (NxU32 i=0; i<blobLen; i++)
            {
                NxU8 c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlob blob=\"%s\" len=\"%d\">%s</telnetBlob>\r\n", blobType, blobLen, blobText );
        }
        else
        {
            mBlobId++;
            char blobText[BLOB_LINE*2+1];
            const NxU8 *scan = (const NxU8 *)blobData;
            char *dest = blobText;
            for (NxU32 i=0; i<BLOB_LINE; i++)
            {
                NxU8 c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlob blob=\"%s\" blobId=\"%d\" len=\"%d\">%s</telnetBlob>\r\n", blobType, mBlobId, blobLen, blobText );
            blobLen-=BLOB_LINE;
            while ( blobLen > BLOB_LINE )
            {
              char *dest = blobText;
              for (NxU32 i=0; i<BLOB_LINE; i++)
              {
                  NxU8 c = *scan++;
                  dest[0] = getHex(c>>4);
                  dest[1] = getHex(c&0xF);
                  dest+=2;
              }
              *dest = 0;
              blobLen-=BLOB_LINE;
              mCallback->sendBlobText(client,"<telnetBlobData blobId=\"%d\">%s</telnetBlobData>\r\n", mBlobId, blobText );
            }
            dest = blobText;
            for (NxU32 i=0; i<blobLen; i++)
            {
                NxU8 c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlobEnd blobId=\"%d\">%s</telnetBlobEnd>\r\n", mBlobId, blobText );
        }
    }
	return ret;
  }

  virtual const char * receiveBlob(NxU32 &client,const void *&data,NxU32 &dlen)
  {
    const char *ret  = 0;
	client = 0;
    data = 0;
    dlen = 0;

    delete mLastBlob;
    mLastBlob = 0;

    if ( !mBlobs.empty() )
    {
        BlobList::iterator i;
        for (i=mBlobs.begin(); i!=mBlobs.end(); ++i)
        {
            Blob *b = (*i);
            if ( b->mFinished )
            {
                mLastBlob = b;
                client = b->mClient;
                data   = b->mBlobData;
                dlen   = b->mBlobLen;
                ret    = b->mBlobType;
                mBlobs.erase(i);
                break;
            }
        }
    }

    return ret;
  }

  virtual bool processIncomingBlobText(NxU32 client,const char *text)
  {
	  bool ret = false;

	  if ( strncmp(text,"<telnetBlob",11) == 0 )
	  {
		  size_t len = strlen(text);
          mClient = client;
          ret = mFastXml->processXml(text,len,this);
          if ( !ret )
          {
            NxI32 lineno;
            const char *error = mFastXml->getError(lineno);
            printf("Error: %s at line %d\r\n", error, lineno );
          }
	  }
	  return ret;
  }

  virtual bool processElement(const char *elementName,         // name of the element
                              NxI32         argc,                // number of attributes
                              const char **argv,               // list of attributes.
                              const char  *elementData,        // element data, null if none
                              NxI32         lineno)         // line number in the source XML file
  {
    bool ret = true;

    if ( elementData )
    {

      NxI32 len = 0;
      NxI32 blobId = 0;
      const char *blobName=0;
  	  NxI32 acount = argc/2;
      for (NxI32 i=0; i<acount; i++)
      {
          const char * atr   = argv[i*2];
          const char * value = argv[i*2+1];
          if ( strcmp(atr,"blob") == 0 )
          {
              blobName = value;
          }
          else if ( strcmp(atr,"blobId") == 0 )
          {
              blobId = atoi(value);
          }
          else if ( strcmp(atr,"len") == 0 )
          {
              len = atoi(value);
          }
      }
      {
        if ( strcmp(elementName,"telnetBlob") == 0 )
        {
            Blob *check = locateBlob(blobId,mClient);
            assert(check==0);
			assert(blobName);
            //
            assert(len > 0 );
            if ( len > 0 && check == 0 && blobName )
            {
              Blob *b = MEMALLOC_NEW(Blob)(blobName,mClient,blobId,len,elementData);
              mBlobs.push_back(b);
            }
        }
        else if ( strcmp(elementName,"telnetBlobData") == 0 )
        {
          Blob *b = locateBlob(blobId,mClient);
          if ( b )
          {
            b->addData(elementData);
          }
        }
        else if ( strcmp(elementName,"telnetBlobEnd") == 0 )
        {
          Blob *b = locateBlob(blobId,mClient);
          if ( b )
          {
            b->addDataEnd(elementData);
          }
        }
      }
    }

    return ret;
  }

  Blob * locateBlob(NxU32 id,NxU32 client) const
  {
    Blob *ret = 0;
    if ( id != 0 )
    {
      BlobList::const_iterator i;
      for (i=mBlobs.begin(); i!=mBlobs.end(); i++)
      {
        Blob *b = (*i);
        if ( b->getId() == id && b->mClient == client )
        {
            ret = b;
            break;
        }
	  }
    }
    return ret;
  }

private:
  NxU32    mClient;
  NxU32    mBlobId;
  BlobIOInterface *mCallback;
  FastXml         *mFastXml;
  Blob            *mLastBlob;
  BlobList         mBlobs;
};

BlobIO * createBlobIO(BlobIOInterface *iface)
{
    MyBlobIO * m = MEMALLOC_NEW(MyBlobIO)(iface);
    return static_cast< BlobIO *>(m);
}

void     releaseBlobIO(BlobIO *b)
{
    MyBlobIO *m = static_cast< MyBlobIO *>(b);
    delete m;
}


//******************************************************
//*** Mutex layer
//******************************************************


class OdfMutex
{
	public:
		OdfMutex(void);
		~OdfMutex(void);

	public:
		// Blocking Lock.
		void Lock(void);

		// Non-blocking Lock. Return's false if already locked.
		bool TryLock(void);

		// Unlock.
		void Unlock(void);

	private:
		#if defined(WIN32) || defined(_XBOX)
		CRITICAL_SECTION m_Mutex;
		#elif defined(__APPLE__) || defined(LINUX)
		pthread_mutex_t  m_Mutex;
		#endif
};

OdfMutex::OdfMutex(void)
{
#if defined(WIN32) || defined(_XBOX)
	InitializeCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_init(&m_Mutex, 0);
#endif
}

OdfMutex::~OdfMutex(void)
{
#if defined(WIN32) || defined(_XBOX)
	DeleteCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_destroy(&m_Mutex);
#endif
}

// Blocking Lock.
void OdfMutex::Lock(void)
{
#if defined(WIN32) || defined(_XBOX)
	EnterCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_lock(&m_Mutex);
#endif
}

// Non-blocking Lock. Return's false if already locked.
bool OdfMutex::TryLock(void)
{
	bool bRet = false;
#if defined(WIN32) || defined(_XBOX)
	//assert(("TryEnterCriticalSection seems to not work on XP???", 0));
	bRet = TryEnterCriticalSection(&m_Mutex) ? true : false;
#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_trylock(&m_Mutex)
#endif
	return bRet;
}

// Unlock.
void OdfMutex::Unlock(void)
{
#if defined(WIN32) || defined(_XBOX)
	LeaveCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_unlock(&m_Mutex)
#endif
}

//******************************************************
//*** End Of Mutex layer
//******************************************************

//******************************************************
//*** Threading layer
//******************************************************

class OdfThread
{
	protected:
		// Called when the thread is started. This method runs on the Thread.
		virtual void OnThreadExecute(void)=0;

	public:
		OdfThread(void);
	 virtual ~OdfThread(void);

	public:
		// Start execution of the thread.
		void ThreadExecute(void);

		// Wait for the Thread to terminate.
		void ThreadWait(void);

		// Stop the thread's execution (not safe).
		void ThreadKill(void);

	protected:
		// Safely Quit the thread.
		void ThreadQuit(void);

	private:
			HANDLE m_hThread;

			friend static DWORD WINAPI _ODFThreadFunc(LPVOID arg);
};


static DWORD WINAPI _ODFThreadFunc(LPVOID arg)
{
	((OdfThread *)arg)->OnThreadExecute();
	return 0;
}



OdfThread::OdfThread(void)
{
		m_hThread = 0;
}

OdfThread::~OdfThread(void)
{
		assert(!m_hThread);
}

// Start execution of the thread.
void OdfThread::ThreadExecute(void)
{
		if(!m_hThread)
		{
			m_hThread = CreateThread(0, 0, _ODFThreadFunc, this, 0, 0);
		}
}

// Wait for the Thread to terminate.
void OdfThread::ThreadWait(void)
{
		if(m_hThread)
		{
			WaitForSingleObject(m_hThread, INFINITE);
		}
}

// Safely Quit the thread.
void OdfThread::ThreadQuit(void)
{
		if(m_hThread)
		{
			m_hThread = 0;
			ExitThread(0);
		}
}

// Stop the thread's execution (not safe).
void OdfThread::ThreadKill(void)
{
		if(m_hThread)
		{
			#if defined(WIN32)
			TerminateThread(m_hThread, 0);
			#endif
			#if defined(_XBOX)
			//-- TODO: Please figure out the equivalent of TerminateThread
			assert(false);
			#endif
			CloseHandle(m_hThread);
			m_hThread = 0;
		}
}



//******************************************************
//*** End of Threading layer
//******************************************************

//******************************************************
//** The Telnet header file
//******************************************************


class TelnetLineNode;
typedef std::queue<TelnetLineNode*> TelnetLineNodeQueue;

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif


// Common Telnet Functionality.
class TelnetInterface
{
	public:
   
		// Called when a connection has been established.
		virtual void OnConnect(NxU32 uiClient)
		{
      PushLine("NewConnection", uiClient );
		}

		// Sends text across the telnet connection.
		// returns false on failure.
		virtual bool SendText(NxU32 uiClient, const char *pcLine, ...)=0;

		// Waits until there is a block ready to be read.
		bool WaitForBlock(void);

		// Pops the last line off the local queue.
		// returns 0 if no lines available.
		const char *GetLine(NxU32 &uiClient);

	protected:
		// Add a Line to the Local Queue.
		void PushLine(const char *pcLine, NxU32 uiClient);

	protected:
		TelnetInterface(void);
		TelnetInterface(const TelnetInterface&){}
	 virtual ~TelnetInterface(void);

	private:
		OdfMutex              m_LineMutex;
		TelnetLineNodeQueue   m_Lines;
		char                 *m_pcLineBuffer;

		OdfMutex              m_BlockMutex;
		OdfMutex              m_HaveBlock;
};

#ifdef WIN32
#pragma warning(pop)
#endif


//******************************************************
//** The Telnet header file
//******************************************************


class TelnetLineNode : public NVSHARE::Memalloc
{
	public:
		char         *pcLine;
		NxU32  uiClient;
};

// Pops the last line off the local queue.
// returns 0 if no lines available.
const char *TelnetInterface::GetLine(NxU32 &uiClient)
{
	const char *pRet = 0;
	
	m_LineMutex.Lock();
	if(!m_Lines.empty())
	{
		TelnetLineNode *node = m_Lines.front();
		m_Lines.pop();
		
		if(m_pcLineBuffer)
		{
			::free(m_pcLineBuffer);
			m_pcLineBuffer = 0;
		}
		
		m_pcLineBuffer  = node->pcLine;
		uiClient        = node->uiClient;

		delete node;
		
		pRet            = m_pcLineBuffer;
	}
	m_LineMutex.Unlock();
	
	return pRet;
}


// Add a Line to the Local Queue.
void TelnetInterface::PushLine(const char *pcLine, NxU32 uiClient)
{
	m_LineMutex.Lock();
	TelnetLineNode *node = MEMALLOC_NEW(TelnetLineNode);
	node->pcLine = (char*)::malloc(sizeof(char)*(strlen(pcLine)+1));
	strcpy(node->pcLine, pcLine);
	node->uiClient = uiClient;
	m_Lines.push(node);
	m_LineMutex.Unlock();
}

TelnetInterface::TelnetInterface(void)
{
	m_pcLineBuffer = 0;
	m_HaveBlock.Lock();
}

TelnetInterface::~TelnetInterface(void)
{
	if(m_pcLineBuffer)
	{
		::free(m_pcLineBuffer);
	}

	while(!m_Lines.empty())
	{
		TelnetLineNode *pLine = m_Lines.front();
		::free(pLine->pcLine);
		delete pLine;
		m_Lines.pop();
	}

	m_HaveBlock.TryLock();
	m_HaveBlock.Unlock();
}

//************************************************************************************
//** The Telnet Parser header file.
//************************************************************************************


typedef std::queue<char*> StringQueue;

class _Block
{
	public:
		void         *m_pData;
		NxU32  m_uiDataSize;
};

typedef std::queue<_Block> BlockQueue;


class TelnetParser
{
	public:
		TelnetParser(void)
		{
			m_uiBufferSize  = 1024;
			m_uiBufferUsed  = 0;
      m_pBuffer       = (char *)::malloc(sizeof(char)*m_uiBufferSize);
			m_pcLastLine    = 0;

			m_uiCurrBlockSize = 1024;
			m_uiCurrBlockUsed = 0;
      m_pCurrBlock      = (char *)::malloc(sizeof(char)*m_uiCurrBlockSize);
			
			m_bBlockMode      = false;
		}
		
	 ~TelnetParser(void)
		{
			if(m_pBuffer)
			{
        ::free(m_pBuffer);
			}
			
			if(m_pcLastLine)
			{
				::free(m_pcLastLine);
			}

			if(m_pCurrBlock)
			{
				::free(m_pCurrBlock);
			}
			
			while(!m_Lines.empty())
			{
				char *pTemp = m_Lines.front();
				m_Lines.pop();
				if(pTemp)
				{
					::free(pTemp);
				}
			}
			
			while(!m_Blocks.empty())
			{
				_Block &block = m_Blocks.front();
				if(block.m_pData)
				{
					::free(block.m_pData);
				}
				m_Blocks.pop();
			}
		}
	 
	public:
		void AddBuffer(const char *pcBuffer, NxU32 uiLen)
		{
			NxU32 uiNewSize = uiLen + m_uiBufferUsed;
			if(uiNewSize >= m_uiBufferSize) Resize(uiNewSize);
			
			for(NxU32 i=0; i<uiLen; i++)
			{
				if(m_bBlockMode)
					BlockChar(pcBuffer[i]);
				else
					ParseChar(pcBuffer[i]);
			}
		}
		
		const char *GetLine(void)
		{
			const char *pRet = 0;
			
			if(!m_Lines.empty())
			{
				if(m_pcLastLine)
				{
					::free(m_pcLastLine);
				}
				m_pcLastLine = m_Lines.front();
				m_Lines.pop();
				pRet = m_pcLastLine;
			}
			
			return pRet;
		}
		
		const void *GetBlock(NxU32 &uiSize)
		{
			void *pRet = 0;
			if(!m_Blocks.empty())
			{
				_Block &block = m_Blocks.front();
				pRet = block.m_pData;
				uiSize = block.m_uiDataSize;
				m_Blocks.pop();
			}
			return pRet;
		}
	
	private:
		void ParseChar(char c)
		{
			if(c >= 32 && c < 128)
			{
				// simply add the character.
				m_pBuffer[m_uiBufferUsed++] = c;
			}
			else if(c == '\n' && m_uiBufferUsed)
			{
				// Add a line to the queue.
				char *pcLine = (char*)::malloc(sizeof(char)*(m_uiBufferUsed+1));
				memcpy(pcLine, m_pBuffer, m_uiBufferUsed);
				pcLine[m_uiBufferUsed] = 0;
				m_Lines.push(pcLine);
				m_uiBufferUsed = 0;
			}
			else if(c == 8 && m_uiBufferUsed)
			{
				// if backspace then remove the last character.
				m_uiBufferUsed--;
			}
			else
			{
				// ???
			}
			
			m_pBuffer[m_uiBufferUsed] = 0;
			
			char *pBlockStart = strstr(m_pBuffer, "<NxBlock");
			if(pBlockStart)
			{
				char *equals  = strchr(pBlockStart, '=');
				char *end     = strchr(pBlockStart, '>');
				if(end)
				{
					if(equals && equals < end)
					{
						char *name = equals+1;
						NxU32 len = (NxU32)(end - name) + 1;
					}
					m_bBlockMode = true;
				}
			}
			
		}
		
		void BlockChar(char c)
		{
			if(m_uiCurrBlockUsed+1 >=  m_uiCurrBlockSize)
			{
				NxU32 uiNewSize = m_uiCurrBlockSize * 2;
				char *pBlock = (char*)::malloc(sizeof(char)*uiNewSize);
				memcpy(pBlock, m_pCurrBlock, m_uiCurrBlockUsed);
				if(m_pCurrBlock)
				{
					::free(m_pCurrBlock);
				}
				m_pCurrBlock = pBlock;
				m_uiCurrBlockSize = uiNewSize;
			}
			
			m_pCurrBlock[m_uiCurrBlockUsed++] = c;
			m_pCurrBlock[m_uiCurrBlockUsed]   = 0;
			
			char *pcEnd = strstr(m_pCurrBlock, "</NxBlock>");
			if(pcEnd)
			{
				m_bBlockMode = false;
				NxU32 uiSize = (NxU32)(pcEnd - m_pCurrBlock);
				
				_Block block;
				block.m_uiDataSize  = uiSize;
				block.m_pData       = (char*)::malloc(sizeof(char)*uiSize);
				memcpy(block.m_pData, m_pCurrBlock, uiSize);
				m_Blocks.push(block);
				
				for(NxU32 i=uiSize+9; i<m_uiCurrBlockUsed; i++)
				{
					ParseChar(m_pCurrBlock[i]);
				}
				
				m_uiCurrBlockUsed = 0;
			}
		}

		void Resize(NxU32 uiNewSize)
		{
			char *pNewBuffer = (char*)::malloc(sizeof(char)*uiNewSize);
			if(m_uiBufferUsed)
			{
				memcpy(pNewBuffer, m_pBuffer, m_uiBufferUsed);
			}
			if(m_pBuffer)
			{
				::free(m_pBuffer);
			}
			m_pBuffer = pNewBuffer;
			m_uiBufferSize = uiNewSize;
		}
	
	private:
		char             *m_pBuffer;
		NxU32      m_uiBufferSize;
		NxU32      m_uiBufferUsed;
		
		char             *m_pcLastLine;
		StringQueue       m_Lines;
		
		bool              m_bBlockMode;
		char             *m_pCurrBlock;
		NxU32      m_uiCurrBlockSize;
		NxU32      m_uiCurrBlockUsed;
		BlockQueue        m_Blocks;
};



//****************************************************************************************
//** Telnet Client header file
//****************************************************************************************


// Simple Telnet Client.
class TelnetClient : public TelnetInterface, public NVSHARE::Memalloc
{
	public:
		TelnetClient(void);
	 virtual ~TelnetClient(void);

	public:
		// Connect to a remote Telnet server.
		// returns false on failure.
		bool Connect(const char *pcAddress, unsigned short uiPort);

		// Closes the current connection.
		void Close(void);

	public:
		// Sends text across the telnet connection.
		// returns false on failure.
		virtual bool SendText(NxU32 uiClient, const char *pcLine, ...);

	private:
		void ThreadFunc(void);
		friend DWORD WINAPI _TelnetClientFunc(LPVOID arg);

	private:
		TelnetClient(const TelnetClient &){}

	private:
		OdfMutex  m_Mutex;
		SOCKET    m_Socket;
		HANDLE    m_Thread;
};

//*************************************************************************************
//** The Telnet Client source code
//*************************************************************************************

	static DWORD WINAPI _TelnetClientFunc(LPVOID arg)
	{
		((TelnetClient *)arg)->ThreadFunc();
		ExitThread(0);
		return 0;
	}


void TelnetClient::ThreadFunc(void)
{
	bool bDone = false;

	m_Mutex.Lock();
	SOCKET  clientSocket  = m_Socket;
	bDone = clientSocket == INVALID_SOCKET ? true : false;
	m_Mutex.Unlock();
	
	#define BUFFER_SIZE 8192
	char vcBuffer[BUFFER_SIZE+1];
	TelnetParser parser;
	
	
	while(!bDone)
	{
		NxI32 iBytesRead = 0;
		iBytesRead = recv(clientSocket, vcBuffer, BUFFER_SIZE, 0);

		if(iBytesRead <= 0)
		{
			bDone = true;
			m_Mutex.Lock();
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			m_Socket = clientSocket;
			m_Mutex.Unlock();
		}
		else
		{
			parser.AddBuffer(vcBuffer, iBytesRead);
		}
		
		const char *pcLine = 0;
		while((pcLine = parser.GetLine()))
		{
			PushLine(pcLine, 0);
		}
		
		
		m_Mutex.Lock();
		clientSocket  = m_Socket;
		bDone = clientSocket == INVALID_SOCKET ? true : false;
		m_Mutex.Unlock();
	}
	
}

/*****************
** TelnetClient **
*****************/

TelnetClient::TelnetClient(void)
{
	m_Socket  = INVALID_SOCKET;
	m_Thread  = 0;

	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
}

TelnetClient::~TelnetClient(void)
{
	 Close();
}

// Connect to a remote Telnet server.
// returns false on failure.
bool TelnetClient::Connect(const char *pcAddress, unsigned short uiPort)
{
	bool bRet = false;
	Close();

	m_Thread = 0;

#if defined(WIN32)
	
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_Socket != INVALID_SOCKET)
	{
		sockaddr_in addr = {0};
		addr.sin_family       = AF_INET;
		addr.sin_port         = htons(uiPort);
		addr.sin_addr.s_addr	= inet_addr(pcAddress);
		
		if(addr.sin_addr.s_addr == INADDR_NONE)
		{
			hostent *pHost = gethostbyname(pcAddress);
			memcpy(&addr.sin_addr, pHost->h_addr, pHost->h_length);
			addr.sin_family = pHost->h_addrtype;
		}
		
		if(connect(m_Socket, (sockaddr *)&addr, sizeof(addr)) == 0)
		{
			OnConnect(0);
			m_Thread = CreateThread(0, 0, _TelnetClientFunc, this, 0, 0);
			bRet = true;
		}
		if(!m_Thread)
		{
			NxI32 error = WSAGetLastError();
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
	}
	else
	{
		assert(0);
	}
	
#endif
	
	return bRet;
}

// Closes the current connection.
void TelnetClient::Close(void)
{
	if(m_Socket != INVALID_SOCKET)
	{
		// Close the Socket.
		m_Mutex.Lock();
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		m_Mutex.Unlock();
		
		// Wait for the Listen Thread to stop.
		WaitForSingleObject(m_Thread, INFINITE);
		m_Thread = 0;
	}
}

// Sends text across the telnet connection.
// returns false on failure.
bool TelnetClient::SendText(NxU32 uiClient, const char *pcLine, ...)
{
	char vcBuffer[8192];
	_vsnprintf(vcBuffer,8191, pcLine, (va_list)(&pcLine+1));
	NxU32 uiLen = (NxU32)strlen(vcBuffer);
	
	send(m_Socket, vcBuffer, uiLen, 0);
	
	return true;
}


//****************************************************************************
//** Telnet server header file
//***************************************************************************


class TelnetServer_Client;
typedef std::map<NxU32, TelnetServer_Client*> TelnetServer_ClientMap;

// Simple Telnet Server.
class TelnetServer : public TelnetInterface, public NVSHARE::Memalloc
{
	friend class TelnetServer_Client;

	public:
		TelnetServer(void);
	 ~TelnetServer(void);

	public:
		// Starts the Telnet server listening on
		// the specified port.
		// returns false if failure.
		bool Listen(unsigned short uiPort);

		// Closes all connections.
		void Close(void);

	public:
		// Sends text across the telnet connection.
		// returns false on failure.
		virtual bool SendText(NxU32 uiClient, const char *pcLine, ...);

	private:
		void ThreadFunc(void);
		friend DWORD WINAPI _TelnetServerFunc(LPVOID arg);

	private:
		TelnetServer(const TelnetServer &){}

	private:
		NxU32            m_uiLastClient;

		OdfMutex                m_ListenMutex;
		SOCKET                  m_ListenSocket;
		HANDLE                  m_ListenThread;

		TelnetServer_ClientMap  m_Clients;
};

//****************************************************************************
//** Telnet server source
//****************************************************************************

class TelnetServer_Client : public NVSHARE::Memalloc
{
	public:
		TelnetServer *m_pParent;
		NxU32  m_uiClient;
		OdfMutex      m_Mutex;
		SOCKET        m_Socket;
		HANDLE        m_Thread;
	
	public:
	 ~TelnetServer_Client(void)
		{
			TelnetServer_ClientMap::iterator iter;
			iter = m_pParent->m_Clients.find(m_uiClient);
			
			if(iter != m_pParent->m_Clients.end())
			{
				m_pParent->m_Clients[m_uiClient] = 0;
				//m_pParent->m_Clients.erase(iter);
			}
		}
	
	private:
		void ThreadFunc(void);
		friend DWORD WINAPI _TelnetServerClientFunc(LPVOID arg);
};


/**********************
** Threading Support **
**********************/

	static DWORD WINAPI _TelnetServerClientFunc(LPVOID arg)
	{
		TelnetServer_Client *pClient = (TelnetServer_Client *)arg;
		pClient->ThreadFunc();
		delete pClient;
		ExitThread(0);
		return 0;
	}

	static DWORD WINAPI _TelnetServerFunc(LPVOID arg)
	{
		((TelnetServer *)arg)->ThreadFunc();
		ExitThread(0);
		return 0;
	}

void TelnetServer_Client::ThreadFunc(void)
{
	bool bDone = false;
	OdfMutex &mutex = m_Mutex;
	
	#define BUFFER_SIZE 8192
	char vcBuffer[BUFFER_SIZE];
	
	TelnetParser parser;
	
	mutex.Lock();
	SOCKET  clientSocket  = m_Socket;
	bDone = clientSocket == INVALID_SOCKET ? true : false;
	mutex.Unlock();
	
	while(!bDone)
	{
	NxI32 iBytesRead=0;

	#if !defined(_XBOX)
	iBytesRead = recv(clientSocket, vcBuffer, BUFFER_SIZE, 0);
	#endif
		
		if(iBytesRead > 0)
		{
			parser.AddBuffer(vcBuffer, iBytesRead);
		}

		const char *pcLine = 0;
		
		while((pcLine = parser.GetLine()))
		{
			m_pParent->PushLine(pcLine, m_uiClient);
		}
		
		mutex.Lock();
		clientSocket  = m_Socket;
		bDone = clientSocket == INVALID_SOCKET ? true : false;
		mutex.Unlock();
	}


}

void TelnetServer::ThreadFunc(void)
{
	bool bDone = false;
	
	m_ListenMutex.Lock();
	SOCKET listenSocket = m_ListenSocket;
	bDone = listenSocket == INVALID_SOCKET ? true : false;
	m_ListenMutex.Unlock();
	
	while(!bDone)
	{
		SOCKET clientSocket= INVALID_SOCKET;
		#if !defined(_XBOX)
		clientSocket = accept(listenSocket, 0, 0);
		#endif
		if(clientSocket != INVALID_SOCKET)
		{
			TelnetServer_Client *pClient = MEMALLOC_NEW(TelnetServer_Client);
			pClient->m_pParent  = this;
			pClient->m_uiClient = ++m_uiLastClient;
			pClient->m_Socket   = clientSocket;
			m_Clients[pClient->m_uiClient] = pClient;
			pClient->m_Thread   = CreateThread(0, 0, _TelnetServerClientFunc, pClient, 0, 0);
			
			OnConnect(pClient->m_uiClient);
		}
		
		m_ListenMutex.Lock();
		listenSocket = m_ListenSocket;
		bDone = listenSocket == INVALID_SOCKET ? true : false;
		m_ListenMutex.Unlock();
	}

	TelnetServer_ClientMap::iterator iter;
	for(iter=m_Clients.begin(); iter!=m_Clients.end(); iter++)
	{
		TelnetServer_Client *pClient = (*iter).second;
		if(pClient)
		{
			OdfMutex &mutex = pClient->m_Mutex;
			
			// Signal the socket.
			mutex.Lock();
			#if !defined(_XBOX)
			closesocket(pClient->m_Socket);			
			#endif
			pClient->m_Socket = INVALID_SOCKET;
			mutex.Unlock();
			
			// Wait for the thread to stop.
			WaitForSingleObject(pClient->m_Thread, INFINITE);
		}
	}
	m_Clients.clear();
	
}

/*****************
** TelnetServer **
*****************/

TelnetServer::TelnetServer(void)
{
	m_uiLastClient = 1;

	m_ListenSocket  = INVALID_SOCKET;
	m_ListenThread  = 0;

	#if !defined(_XBOX)
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	#endif
}

TelnetServer::~TelnetServer(void)
{
	Close();
}

// Starts the Telnet server listening on
// the specified port.
// returns false if failure.
bool TelnetServer::Listen(unsigned short uiPort)
{
	bool bRet = false;

	Close();
	
	m_ListenThread  = 0;

	#if !defined(_XBOX)
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(m_ListenSocket != INVALID_SOCKET)
	{
		sockaddr_in addr      = {0};
		addr.sin_family       = AF_INET;
		addr.sin_port         = htons(uiPort);
		addr.sin_addr.s_addr  = htonl(INADDR_ANY);
		
		if(bind(m_ListenSocket, (sockaddr*)&addr, sizeof(addr)) == 0)
		{
			if(listen(m_ListenSocket, SOMAXCONN) == 0)
			{
				m_ListenThread = CreateThread(0, 0, _TelnetServerFunc, this, 0, 0);
				bRet = true;
			}
			else
			{
				NxI32 error = WSAGetLastError();
				assert(0);
			}
		}
		else // if(bind(m_ListenSocket, (sockaddr*)&addr, sizeof(addr)) == 0)
		{
			NxI32 error = WSAGetLastError();
//			assert(0);
		}
		
		if(!m_ListenThread)
		{
			NxI32 error = WSAGetLastError();
			closesocket(m_ListenSocket);
			m_ListenSocket = INVALID_SOCKET;
		}
	}
	else // if(m_ListenSocket != INVALID_SOCKET)
	{
		NxI32 error = WSAGetLastError();
		assert(0);
	}
	

	
//	assert(m_ListenSocket != INVALID_SOCKET);

	#endif

	
	return bRet;
}

// Closes all connections.
void TelnetServer::Close(void)
{
	if(m_ListenSocket != INVALID_SOCKET)
	{
		// Close the Socket.
		m_ListenMutex.Lock();
		#if !defined(_XBOX)		
		closesocket(m_ListenSocket);
		#endif
		m_ListenSocket = INVALID_SOCKET;
		m_ListenMutex.Unlock();
		
		// Wait for the Listen Thread to stop.
		WaitForSingleObject(m_ListenThread, INFINITE);
		m_ListenThread = 0;
	}
}

// Sends text across the telnet connection.
// returns false on failure.
bool TelnetServer::SendText(NxU32 uiClient, const char *pcLine, ...)
{
	bool bRet = false;

	TelnetServer_ClientMap::iterator iter;

	char vcBuffer[8192];
	_vsnprintf(vcBuffer, 8191, pcLine, (va_list)(&pcLine+1));
	NxU32 uiLen = (NxU32)strlen(vcBuffer);

	if(!uiClient)
	{
		for(iter=m_Clients.begin(); iter!=m_Clients.end(); iter++)
		{
			TelnetServer_Client *pClient  = (*iter).second;
			pClient->m_Mutex.Lock();
			#if !defined(_XBOX)
			send(pClient->m_Socket, vcBuffer, uiLen, 0);
			#endif
			pClient->m_Mutex.Unlock();
			bRet = true;
		}
	}
	else
	{
		iter = m_Clients.find(uiClient);
		if(iter != m_Clients.end())
		{
			TelnetServer_Client *pClient  = (*iter).second;
			pClient->m_Mutex.Lock();
			#if !defined(_XBOX)			
			send(pClient->m_Socket, vcBuffer, uiLen, 0);
			#endif
			pClient->m_Mutex.Unlock();
			bRet = true;
		}
	}
	
	return bRet;
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
	ST_EOS          // is a comment symbol, and everything past this character should be ignored
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
			mHardString[i*2+1] = 0;
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

	NxI32  Parse(InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason

	NxI32 ProcessLine(NxI32 lineno,char *line,InPlaceParserInterface *callback);

	const char ** GetArglist(char *source,NxI32 &count); // convert source string into an arg list, this is a destructive parse.

	void SetHardSeparator(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}

	void SetHard(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
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

private:


	inline char * AddHard(NxI32 &argc,const char **argv,char *foo);
	inline bool   IsHard(char c);
	inline char * SkipSpaces(char *foo);
	inline bool   IsWhiteSpace(char c);
	inline bool   IsNonSeparator(char c); // non seperator,neither hard nor soft

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
		::free(mData);
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
			mData = (char *) ::malloc(sizeof(char)*(mLen+1));
			NxI32 ok = (NxI32)fread(mData, mLen, 1, fph);
			if ( !ok )
			{
				::free(mData);
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
		::free(mData);
	}
}

#define MAXARGS 512

bool InPlaceParser::IsHard(char c)
{
	return mHard[c] == ST_HARD;
}

char * InPlaceParser::AddHard(NxI32 &argc,const char **argv,char *foo)
{
	while ( IsHard(*foo) )
	{
		const char *hard = &mHardString[*foo*2];
		if ( argc < MAXARGS )
		{
			argv[argc++] = hard;
		}
		foo++;
	}
	return foo;
}

bool   InPlaceParser::IsWhiteSpace(char c)
{
	return mHard[c] == ST_SOFT;
}

char * InPlaceParser::SkipSpaces(char *foo)
{
	while ( !EOS(*foo) && IsWhiteSpace(*foo) ) foo++;
	return foo;
}

bool InPlaceParser::IsNonSeparator(char c)
{
	if ( !IsHard(c) && !IsWhiteSpace(c) && c != 0 ) return true;
	return false;
}


NxI32 InPlaceParser::ProcessLine(NxI32 lineno,char *line,InPlaceParserInterface *callback)
{
	NxI32 ret = 0;

	const char *argv[MAXARGS];
	NxI32 argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{

		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) ) break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			foo++;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) foo++;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				foo++;
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
					foo++;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) foo++;
					if ( *foo ) *foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						foo++;
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
						foo++;
						break;
					}
					foo++;
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
	SetHardSeparator('<');
	SetHardSeparator('>');
	SetHardSeparator(':');
	SetHardSeparator('=');
	SetHardSeparator('[');
	SetHardSeparator(']');
	SetHardSeparator('{');
	SetHardSeparator('}');
	SetCommentSymbol('#');
}


const char ** InPlaceParser::GetArglist(char *line,NxI32 &count) // convert source string into an arg list, this is a destructive parse.
{
	const char **ret = 0;

	static const char *argv[MAXARGS];
	NxI32 argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{

		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) ) break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			foo++;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) foo++;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				foo++;
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
					foo++;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) foo++;
					if ( *foo ) *foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						foo++;
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
						foo++;
						break;
					}
					foo++;
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

#define MAXPARSEBUFFER 2048

class BlobLine
{
public:
  BlobLine(NxU32 client,const char *msg)
  {
    mClient = client;
    size_t l = strlen(msg);
    mBlobText = (char *)MEMALLOC_MALLOC(l+1);
    memcpy(mBlobText,msg,l+1);
  }
  ~BlobLine(void)
  {
    MEMALLOC_FREE(mBlobText);
  }

  BlobLine(const BlobLine &b)
  {
    mClient = b.mClient;
    size_t l = strlen(b.mBlobText);
    mBlobText = (char *)MEMALLOC_MALLOC(l+1);
    memcpy(mBlobText,b.mBlobText,l+1);
  }

  NxU32 mClient;
  char        *mBlobText;
};

typedef std::queue< BlobLine > BlobLineQueue;

class MyTelnet : public NVSHARE::Telnet, public BlobIOInterface, public NVSHARE::Memalloc
{
public:
  MyTelnet(NVSHARE::TelnetType type,const char *address,NxU32 port)
  {
    mBlobIO = createBlobIO(this);
    mParser.DefaultSymbols();
    mClient = 0;
    mInterface = 0;
	mServer = 0;
	mIsServer = false;
	mHaveConnection = false;

	if ( type == NVSHARE::TT_SERVER_ONLY || type == NVSHARE::TT_CLIENT_OR_SERVER )
	{
      mServer = MEMALLOC_NEW(TelnetServer);
      mIsServer = mServer->Listen(port);
      mHaveConnection = true;
	}
    if ( mIsServer )
    {
      mInterface = static_cast< TelnetInterface *>(mServer);
    }
    else if ( type == NVSHARE::TT_CLIENT_ONLY || type == NVSHARE::TT_CLIENT_OR_SERVER )
    {
      delete mServer;
      mServer = 0;
      mClient = MEMALLOC_NEW(TelnetClient);
      mHaveConnection = mClient->Connect(address,port);
      if ( !mHaveConnection )
      {
        delete mClient;
        mClient = 0;
      }
      else
      {
        mInterface = static_cast< TelnetInterface *>(mClient);
      }
    }
  }

  virtual bool          isServer(void) // returns true if we are a server or a client.  First one created on a machine is a server, additional copies are clients.
  {
    return mIsServer;
  }

  virtual bool haveConnection(void)
  {
    return mHaveConnection;
  }

  virtual bool         sendMessage(NxU32 client,const char *fmt,...)
  {
    bool ret = false;
    if ( mInterface )
    {
    	char wbuff[MAXPARSEBUFFER];
        wbuff[MAXPARSEBUFFER-1] = 0;
    	_vsnprintf(wbuff,MAXPARSEBUFFER-1, fmt, (char *)(&fmt+1));
        ret = mInterface->SendText(client,"%s",wbuff);
    }
    return ret;
  }

  virtual const char *  receiveMessage(NxU32 &client)
  {
    const char *ret = 0;
    client = 0;

    if ( !mBlobLines.empty() )
    {
        BlobLine &bl = mBlobLines.front();
        mInterface->SendText(bl.mClient,"%s", bl.mBlobText );
        mBlobLines.pop();
    }

    if ( mInterface )
    {
      ret = mInterface->GetLine(client);
      if ( ret && mBlobIO )
      {
        bool snarfed = mBlobIO->processIncomingBlobText(client,ret);
        if ( snarfed )
        {
            ret = 0;
        }
      }
    }

    return ret;
  }

  virtual const char ** getArgs(const char *input,NxU32 &argc) // parse string into a series of arguments.
  {
    strncpy(mParseBuffer,input,MAXPARSEBUFFER);
    mParseBuffer[MAXPARSEBUFFER-1] = 0;
	NxI32 ac;
    const char **ret = mParser.GetArglist(mParseBuffer,ac);
	argc = (NxU32)ac;
	return ret;
  }

  virtual ~MyTelnet(void)
  {
    delete mClient;
    delete mServer;
    releaseBlobIO(mBlobIO);
  }

  virtual bool          sendBlob(NxU32 client,const char *blobType,const void *data,NxU32 dlen)
  {
    bool ret = false;

    if ( mBlobIO )
    {
        ret = mBlobIO->sendBlob(client,blobType,data,dlen);
    }


    return ret;
  }

  virtual const char *  receiveBlob(NxU32 &client,const void *&data,NxU32 &dlen)
  {
    const char *ret = 0;

    client = 0;
    dlen = 0;
    data = 0;

    if ( mBlobIO )
    {
        ret = mBlobIO->receiveBlob(client,data,dlen);
    }


    return ret;
  }

  virtual void sendBlobText(NxU32 client,const char *fmt,...)
  {
    if ( mInterface )
    {
    	char wbuff[MAXPARSEBUFFER];
        wbuff[MAXPARSEBUFFER-1] = 0;
    	_vsnprintf(wbuff,MAXPARSEBUFFER-1, fmt, (char *)(&fmt+1));
        BlobLine bl(client,wbuff);
        mBlobLines.push(bl);
		if ( mBlobIO )
		{
			mBlobIO->processIncomingBlobText(client,wbuff);
		}
    }
  }

private:
  bool          mIsServer;
  bool          mHaveConnection;
  TelnetInterface *mInterface;
  TelnetClient *mClient;
  TelnetServer *mServer;
  char          mParseBuffer[MAXPARSEBUFFER];
  InPlaceParser mParser;
  BlobIO       *mBlobIO;
  BlobLineQueue mBlobLines;
};



}; // end of namespace

namespace NVSHARE
{

	Telnet * createTelnet(TelnetType type,const char *address,NxU32 port)
{
	TELNET_NVSHARE::MyTelnet *m = MEMALLOC_NEW(TELNET_NVSHARE::MyTelnet)(type,address,port);
	return static_cast< Telnet *>(m);
}

void     releaseTelnet(Telnet *t)
{
	TELNET_NVSHARE::MyTelnet *m = static_cast< TELNET_NVSHARE::MyTelnet *>(t);
	delete m;
}

}; // end of namespace

#endif