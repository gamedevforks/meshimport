#include <string.h>

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

#include "keyvalue.h"

#pragma warning(disable:4996)


static char *skipSoft(char *scan,char &quote)
{
	quote = 0;

  while ( *scan )
  {
  	if ( *scan == 34 )
  	{
  		quote = 1;
  		scan++;
  		break;
  	}
  	else if ( *scan == '(' )
  	{
  		quote = 2;
  		scan++;
  		break;
  	}
  	else if ( *scan > 32 && *scan < 127 ) // a valid character to process
  	{
  		break;
  	}
  	scan++;
  }
  return scan;
}

static bool endQuote(char c,char quote)
{
  bool ret = false;

  if ( c == 34 && quote == 1 )
    ret = true;
  else if ( c == ')' && quote == 2 )
    ret = true;

  return ret;
}

const char ** KeyValue::getKeyValues(const char *userProperties,NxU32 &count)
{
	const char **ret = 0;
	count = 0;

	if ( userProperties )
	{
		strncpy(keyBuffer,userProperties,MAXKEYVALUEBUFFER);
		char *scan = keyBuffer;
		char quote;

		while ( *scan ) // while there are characters to process..
		{

			scan = skipSoft(scan,quote); // skip leading spaces if any, note if it begins with a quote

			if ( *scan )  // if still data to process, copy the key pointer and search for the termination character.
			{
   			keyValue[count++] = scan; // store the key pointer and advance the counter.
 	  		while ( *scan  )  // search until we hit an 'equal' sign.
 		  	{
 			  	if ( endQuote(*scan,quote) ) // if we hit a quote mark, treat that as an end of string termination, but still look for the equal sign.
 			  	{
   					*scan = 0;
   				}
   				else if ( *scan == '=' ) // if we hit the equal sign, terminate the key string by stomping on the equal and continue to value processing
   				{
   					*scan = 0; // stomp and EOS marker.
   					scan++;    // advance to the next location.
   					break;     // break out of the loop
   				}
   				scan++;
   			}
 			  if ( *scan ) // if there is still data to process.
   			{
 	  			scan = skipSoft(scan,quote); // skip leading spaces, etc. note if there is a key.
   			  keyValue[count++] = scan;  // assign the value.
   			  while ( *scan )
   			  {
   			  	if ( quote )  // if we began with a quote, then only a quote can terminate
   			  	{
   			  		if ( endQuote(*scan,quote) ) // if it is a quote, then terminate the string and continue to the next key
   			  		{
   			  			*scan = 0;
   			  			scan++;
   			  			break;
   			  		}
						}
						else
						{
 			  			// acceptable seperators are a space, a comma, or a semi-colon
 			  			if ( *scan == ';' || *scan == ',' || *scan == '=' || *scan == 32 )
 			  			{
 			  				*scan = 0;
 			  				scan++;
 			  				break;
   			  		}
   			  	}
   			  	scan++;
   			  }
 	  		}
 	  	}
		}
	}

	count = count/2;

	if ( count )
		ret = (const char **)keyValue;

	return ret;
}


const char *  KeyValue::getKeyValue(const char *userProperties,const char *_key,bool caseSensitive)  //
{
  const char *ret = 0;

  NxU32 count=0;
  const char **slist = getKeyValues(userProperties,count);
  if ( count )
  {
    for (NxU32 i=0; i<count; i++)
    {
      const char *key = slist[i*2];
      const char *value = slist[i*2+1];
      if ( caseSensitive && strcmp(key,_key) == 0 )
      {
        ret = value;
        break;
      }
      else if ( stricmp(key,_key) == 0 )
      {
        ret = value;
        break;
      }
    }
  }
  return ret;
}


