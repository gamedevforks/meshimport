#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "LegacyFastXml.h"
#pragma warning(disable:4996)


/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
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

namespace NVSHARE
{
class MyFastXml : public FastXml
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

  virtual bool processXmlFile(const char* filename, FastXmlInterface* iface)
  {
	  bool ret = true;

	  release();

	  FILE *infile = fopen(filename, "rb");

	  if(infile == NULL)
		  return NULL;

	  fseek(infile, 0L, SEEK_END);
	  NxI32 numbytes = ftell(infile);
	  fseek(infile, 0L, SEEK_SET);	

	  mInputData = (char*)malloc((numbytes + 1) * sizeof(char));	

	  if(mInputData == NULL)
		  return NULL;

	  memset(mInputData, 0, (numbytes + 1) * sizeof(char));

	  /* copy all the text into the buffer */
	  fread(mInputData, sizeof(char), numbytes, infile);

	  fclose(infile);

	  ret =  processXml(iface);

	  return ret;
  }

  virtual bool processXml(const char *inputData,NxU32 dataLen,FastXmlInterface *iface)
  {
	  release();

	  mInputData = (char *)malloc(dataLen+1);
	  memcpy(mInputData,inputData,dataLen);
	  mInputData[dataLen] = 0;

	  return processXml(iface);
  }

  bool processXml(FastXmlInterface *iface)
  {
    bool ret = true;

    #define MAX_ATTRIBUTE 2048 // can't imagine having more than 2,048 attributes in a single element right?

    mLineNo = 1;

    char *element;

    char *scan = mInputData;
    if ( *scan == '<' )
    {
      scan++;
      while ( *scan )
      {
        scan = skipNextData(scan);
        if ( *scan == 0 ) 
			return ret;
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
            if ( !scan ) 
				return false;
          }
          else
          {
            if ( *scan == 0 ) 
				return ret;
            *scan = 0; // place a zero byte to indicate the end of the element name...
            scan++;

            while ( *scan )
            {
              scan = skipNextData(scan); // advance past any soft seperators (tab or space)

              if ( mTypes[*scan] == CT_END_OF_ELEMENT )
              {
                char c = *scan++;
                scan = processClose(c,element,scan,argc,argv,iface);
                if ( !scan ) 
					return false;
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
                      //mError = "Expected quote to begin attribute";
					  //return false;
					  // PH: let's try to have a more graceful fallback
				      argc--;
					  while (*scan != '/' && *scan != '>' && *scan != 0)
						  scan++;
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
  MyFastXml *f = new MyFastXml;
  return static_cast< FastXml *>(f);
}

void      releaseFastXml(FastXml *f)
{
  MyFastXml *m = static_cast< MyFastXml *>(f);
  delete m;
}

}; // end of namespace